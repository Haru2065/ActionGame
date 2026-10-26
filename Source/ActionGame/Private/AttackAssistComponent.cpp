#include "AttackAssistComponent.h"

#include "GameFramework/Character.h"
#include "MotionWarpingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

UAttackAssistComponent::UAttackAssistComponent()
{
	// Tick は不要(攻撃した瞬間だけ計算する)
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttackAssistComponent::BeginPlay()
{
	Super::BeginPlay();

	// 持ち主をキャラクターとして取得
	OwnerCharacter = Cast<ACharacter>(GetOwner());

	// 持ち主に付いている Motion Warping コンポーネントを探して保持
	if (OwnerCharacter)
	{
		MotionWarpingComp = OwnerCharacter->FindComponentByClass<UMotionWarpingComponent>();
	}
}

// ============================================================
// ① どの敵をターゲットにするか決める
// ============================================================
AActor* UAttackAssistComponent::FindBestTarget()
{
	if (!OwnerCharacter) return nullptr;

	const FVector MyLocation = OwnerCharacter->GetActorLocation();

	// --- 基準方向を決める(入力があれば入力方向、なければキャラの正面) ---
	FVector BaseDir = OwnerCharacter->GetLastMovementInputVector();
	BaseDir.Z = 0.f; // 高さは無視して水平だけで考える
	if (BaseDir.IsNearlyZero(0.1f))
	{
		BaseDir = OwnerCharacter->GetActorForwardVector();
		BaseDir.Z = 0.f;
	}
	BaseDir = BaseDir.GetSafeNormal();

	// --- 半径 SearchRadius の球に入っている Pawn を集める ---
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(OwnerCharacter); // 自分自身は除外

	TArray<AActor*> OverlappedActors;
	UKismetSystemLibrary::SphereOverlapActors(
		this, MyLocation, SearchRadius, ObjectTypes, EnemyClass, IgnoreActors, OverlappedActors);

	// --- 1体ずつ採点し、スコアが一番小さい(=良い)敵を選ぶ ---
	AActor* BestTarget = nullptr;
	float BestScore = TNumericLimits<float>::Max();

	for (AActor* Enemy : OverlappedActors)
	{
		if (!IsValid(Enemy))
		{
			continue;
		}

		// 自分→敵のベクトル(水平のみ)
		FVector ToEnemy = Enemy->GetActorLocation() - MyLocation;
		ToEnemy.Z = 0.f;

		const float Distance = ToEnemy.Size();
		if (Distance < KINDA_SMALL_NUMBER)
		{
			continue; // 方向が出せないほど近い敵は除外
		}
		const FVector Dir = ToEnemy / Distance;

		// 基準方向と敵方向のなす角(度)。0度に近いほど基準方向にいる
		const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(BaseDir, Dir)));
		if (AngleDeg > MaxAngle)
		{
			continue; // 角度が外れすぎている敵(背後など)は除外
		}

		// スコア(小さいほど良い):角度と距離を 0~1 に正規化して重み付けして足す
		const float Score = (AngleDeg / MaxAngle) * AngleWeight
			+ (Distance / SearchRadius) * DistanceWeight;

		if (Score < BestScore)
		{
			BestScore = Score;
			BestTarget = Enemy;
		}
	}

	return BestTarget;
}

// ============================================================
// 吸い付き先の位置と向きを計算する(計算だけ)
// ============================================================
bool UAttackAssistComponent::GetWarpLocationAndRotation(AActor* Target, FVector& OutLocation, FRotator& OutRotation)
{
	if (!OwnerCharacter || !IsValid(Target))
	{
		return false;
	}

	const FVector MyLoc = OwnerCharacter->GetActorLocation();
	const FVector EnemyLoc = Target->GetActorLocation();

	// 自分→敵の水平ベクトル、距離、向き
	FVector ToEnemy = EnemyLoc - MyLoc;
	ToEnemy.Z = 0.f;
	const float Distance = ToEnemy.Size();
	const FVector Dir = ToEnemy.GetSafeNormal();

	// 吸い付き先:敵の StopDistance 手前。すでに近ければ動かず向きだけ合わせる
	OutLocation = MyLoc;
	if (Distance > StopDistance)
	{
		OutLocation = EnemyLoc - Dir * StopDistance;
		OutLocation.Z = MyLoc.Z; // 高さは今の自分のまま
	}

	// 敵の方を向く回転(Yaw のみ)
	OutRotation = Dir.Rotation();

	if (bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), OutLocation, 30.f, 12, FColor::Green, false, 1.0f);
	}

	return true;
}

// ============================================================
// ② 位置を Motion Warping に登録する(C++で渡す場合)
// ============================================================
void UAttackAssistComponent::ApplyWarpTarget(AActor* Target)
{
	if (!MotionWarpingComp)
	{
		return;
	}

	FVector WarpLoc;
	FRotator WarpRot;

	// ターゲットが無効なら、前回の登録を消して通常のルートモーションで再生させる
	if (!GetWarpLocationAndRotation(Target, WarpLoc, WarpRot))
	{
		MotionWarpingComp->RemoveWarpTarget(WarpTargetName);
		CurrentTarget.Reset();
		return;
	}

	CurrentTarget = Target;

	// Motion Warping にターゲットを登録(同じ名前のノティファイがこれを使う)
	MotionWarpingComp->AddOrUpdateWarpTargetFromLocationAndRotation(WarpTargetName, WarpLoc, WarpRot);
}

// ============================================================
// BP から呼ぶ窓口(C++で渡す場合):① → ② の順に実行
// ============================================================
bool UAttackAssistComponent::UpdateAttackAssist()
{
	AActor* Target = FindBestTarget();
	ApplyWarpTarget(Target);
	return Target != nullptr;
}