// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackAssistComponent.generated.h"

class ACharacter;
class UMotionWarpingComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONGAME_API UAttackAssistComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttackAssistComponent();

	// 範囲内からベストな敵を1体選ぶ。
	UFUNCTION(BlueprintCallable, Category = "AttackAssist")
	AActor* FindBestTarget();

	// 補助移動先の位置と向きを計算する。対象が無効な場合は false。
	UFUNCTION(BlueprintCallable, Category = "AttackAssist")
	bool GetWarpLocationAndRotation(AActor* Target, FVector& OutLocation, FRotator& OutRotaion);

	// ターゲットを選び Motion Warping に登録する。
	UFUNCTION(BlueprintCallable, Category = "AttackAssist")
	bool UpdateAttackAssist();

	// 指定した敵の位置を Motion Warping に登録する。
	UFUNCTION(BlueprintCallable, Category = "AttackAssist")
	void ApplyWarpTarget(AActor* target);

	// 敵を探す半径。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackAssist")
	float SearchRadius;

	// 基準方向（入力方向または正面）から対象にする片側の角度。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackAssist")
	float MaxAngle;

	// 敵の何 cm 手前で止まるか。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackAssist")
	float StopDistance;

	// スコアにおける角度の重み。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackAssist")
	float AngleWeight;

	// スコアにおける距離の重み。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackAssist")
	float DistanceWeight;

	// モンタージュの Motion Warping Notify の Warp Target Name と合わせる。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackAssist")
	FName WarpTargetName = TEXT("AttackTarget");

	// 対象にする敵のクラス。未設定なら Pawn を対象にする。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackAssist")
	TSubclassOf<AActor> EnemyClass;

	// 補助移動先をデバッグ表示する。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackAssist/Debug")
	bool bDrawDebug = false;

protected:
	virtual void BeginPlay() override;

private:
	// このコンポーネントを所有するキャラクター。
	UPROPERTY(Transient)
	TObjectPtr<ACharacter> OwnerCharacter;

	// 所有者の Motion Warping コンポーネント。
	UPROPERTY(Transient)
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComp;

	// 現在のターゲットへの弱参照。
	TWeakObjectPtr<AActor> CurrentTarget;
};
