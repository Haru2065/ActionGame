#include "Status/PlayerBreakSequenceComponent.h"

#include "BreakSequenceManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Status/EnemyStatus.h"
#include "TimerManager.h"

// コンストラクタ
UPlayerBreakSequenceComponent::UPlayerBreakSequenceComponent()
{
	// Tickは使わないのでオフにする（軽量化）
	PrimaryComponentTick.bCanEverTick = false;
}

// ゲーム開始時：仲介役の通知に登録する
void UPlayerBreakSequenceComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		if (UBreakSequenceManager* BreakManager = World->GetSubsystem<UBreakSequenceManager>())
		{
			// 敵がBreakしたら HandleBreakSequenceStart が呼ばれるよう登録する
			BreakManager->OnBreakSequenceStart.AddDynamic(this, &UPlayerBreakSequenceComponent::HandleBreakSequenceStart);
		}
	}
}

// ゲーム終了時：登録解除・タイマー停止・入力ロック解除
void UPlayerBreakSequenceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		// 待機タイマーが残っていると、破棄後に呼ばれて危険なので止める
		World->GetTimerManager().ClearTimer(WaitTimerHandle);

		if (UBreakSequenceManager* BreakManager = World->GetSubsystem<UBreakSequenceManager>())
		{
			// 仲介役への登録を解除する
			BreakManager->OnBreakSequenceStart.RemoveDynamic(this, &UPlayerBreakSequenceComponent::HandleBreakSequenceStart);
		}
	}

	// 入力ロックが残ったままにならないよう解除する
	SetGameplayInputLocked(false);
	Super::EndPlay(EndPlayReason);
}

// 仲介役から「敵がBreakした」通知を受け取った時の処理
void UPlayerBreakSequenceComponent::HandleBreakSequenceStart(AActor* BrokenEnemyActor)
{
	// 無効な敵、または既に別のBreak処理中なら無視する（同時に1つだけ処理する）
	if (!IsValid(BrokenEnemyActor) || bIsBreakSequenceActive)
	{
		return;
	}

	// Break関連の処理中にする（二重開始のガード）
	bIsBreakSequenceActive = true;

	// Breakした敵を覚えておく
	CurrentBreakTarget = BrokenEnemyActor;

	// ボタン待ち状態に入る（この間、入力はロックしない＝通常攻撃で追撃できる）
	bIsWaitingForFinisher = true;

	// BP側へ合図（ボタンアイコンUIの表示など）
	OnBreakWaitStarted.Broadcast(BrokenEnemyActor);

	if (BreakWaitTime == 0.0f)
	{
		// 待ち時間0：待たずに即発動する（注意：比較は「==」。「=」は代入になってしまう）
		StartFinisher();
	}
	else if (BreakWaitTime > 0.0f)
	{
		// 正の値：その秒数後に自動発動するタイマーをセットする
		GetWorld()->GetTimerManager().SetTimer(
			WaitTimerHandle, this, &UPlayerBreakSequenceComponent::OnWaitTimeExpired, BreakWaitTime, false);
	}
	// 負の値：タイマーをセットしない（ボタンが押されるまで待つ）
}

// ボタンが押された時（BPの入力イベントから呼ばれる）
void UPlayerBreakSequenceComponent::TryStartFinisher()
{
	// 待機中でなければ何もしない（通常時にボタンを押しても無反応）
	if (!bIsWaitingForFinisher)
	{
		return;
	}

	StartFinisher();
}

// 待ち時間が過ぎた時（タイマーから呼ばれる）
void UPlayerBreakSequenceComponent::OnWaitTimeExpired()
{
	// 念のため待機中か確認してから自動発動する
	if (!bIsWaitingForFinisher)
	{
		return;
	}

	StartFinisher();
}

// フィニッシュを開始する
void UPlayerBreakSequenceComponent::StartFinisher()
{
	// タイマーが残っていれば止める（ボタンが先に押された場合など）
	GetWorld()->GetTimerManager().ClearTimer(WaitTimerHandle);
	// 待機を終える
	bIsWaitingForFinisher = false;

	AActor* BreakTarget = CurrentBreakTarget.Get();

	// 待機中に敵が倒された・消えた場合は、フィニッシュせず通常状態へ戻る
	if (!IsValid(BreakTarget))
	{
		OnBreakWaitEnded.Broadcast(nullptr);
		bIsBreakSequenceActive = false;
		CurrentBreakTarget.Reset();
		return;
	}

	// BP側へ合図（ボタンアイコンUIを消す）
	OnBreakWaitEnded.Broadcast(BreakTarget);

	// 演出中は入力を受け付けない
	SetGameplayInputLocked(true);

	// BP側へ合図（モンタージュ再生・カメラワークの開始）
	OnBreakSequenceStarted.Broadcast(BreakTarget);
}

// 確定ヒットのダメージを敵へ与える（モンタージュのNotifyから呼ぶ）
void UPlayerBreakSequenceComponent::ApplyForcedBreakDamage(float ForcedDamage)
{
	// 演出中（待機中は含まない）かつダメージが正のときだけ有効
	if (!bIsBreakSequenceActive || bIsWaitingForFinisher || ForcedDamage <= 0.0f)
	{
		return;
	}

	// 敵のステータスコンポーネントを取得して、確定ヒットダメージを渡す
	if (AActor* BreakTarget = CurrentBreakTarget.Get())
	{
		if (UEnemyStatus* EnemyStatus = BreakTarget->FindComponentByClass<UEnemyStatus>())
		{
			EnemyStatus->ApplyForcedBreakDamage(ForcedDamage);
		}
	}
}

// フィニッシュ演出の終了処理
void UPlayerBreakSequenceComponent::EndBreakSequence()
{
	// 演出中でなければ何もしない。待機中に呼ばれても無視する
	if (!bIsBreakSequenceActive || bIsWaitingForFinisher)
	{
		return;
	}

	AActor* FinishedTarget = CurrentBreakTarget.Get();

	// 敵のBreak状態を解除する（※解除のタイミングは仮。後で変更してもOK）
	if (IsValid(FinishedTarget))
	{
		if (UEnemyStatus* EnemyStatus = FinishedTarget->FindComponentByClass<UEnemyStatus>())
		{
			EnemyStatus->ExitBreakState();
		}
	}

	// 後始末をして通常状態に戻る
	bIsBreakSequenceActive = false;
	CurrentBreakTarget.Reset();
	SetGameplayInputLocked(false);

	// BP側へ合図（見た目の後処理）
	OnBreakSequenceEnded.Broadcast(FinishedTarget);
}

// 移動・視点の入力を無効化 / 有効化する
void UPlayerBreakSequenceComponent::SetGameplayInputLocked(bool bLocked)
{
	// 既に同じ状態なら何もしない
	if (bLocked == bHasLockedGameplayInput)
	{
		return;
	}

	// オーナー(プレイヤーキャラ) → コントローラー の順にたどる
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APlayerController* PlayerController = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
	if (!PlayerController)
	{
		return;
	}

	// 移動入力と視点入力を無視する / 戻す
	PlayerController->SetIgnoreMoveInput(bLocked);
	PlayerController->SetIgnoreLookInput(bLocked);
	bHasLockedGameplayInput = bLocked;
}