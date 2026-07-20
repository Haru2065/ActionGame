// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyStatus.generated.h"

// HPが変化した時にBP側へ通知するためのデリゲートを宣言
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam = 「引数を1つ持つ、BPからバインド可能な通知」という意味
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FenemyHPBarChanged, float, HPPercent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FenemyBreakBarChanged, float, BreakPercent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FenemyOnBreak);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONGAME_API UEnemyStatus : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyStatus();

	//BP側でHPUIを減らしたり、増やしたりするイベントをバインドする
	UPROPERTY(BlueprintAssignable, Category = "StatusEnemy")
	FenemyHPBarChanged EnemyHPBarChanged;

	UPROPERTY(BlueprintAssignable, Category = "StatusEnemy")
	FenemyBreakBarChanged EnemyBreakBarChanged;

	UPROPERTY(BlueprintAssignable, Category = "StatusEnemy")
	FenemyOnBreak EnemyOnBreak;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "StatusEnemy")
	void EnemyOnDamage(float totalDamage);

	void setEnemyStatus();

	//ブレイク値を上げるメソッド
	UFUNCTION(BlueprintCallable, Category = "StatusEnemy")
	void AddBreakPoint(float amount);

	UFUNCTION(BlueprintPure, Category = "StatusEnemy")
	float GetEnemyHPPercent() const;

	UFUNCTION(BlueprintPure, Category = "StatusEnemy")
	float GetBreakPercent() const;

	//ブレイク状態か
	bool bIsBreak;

protected:

	/*敵のパラメータ*/

	//敵の現在の体力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusEnemy")
	float EnemyCurrentHP;

	//敵の最大体力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusEnemy")
	float EnemyMaxHP;

	//敵の攻撃力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusEnemy")
	float EnemyAttackPower;

	//敵のブレイクの初期値
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ブレイク値")
	float InitBreak;

	//現在のブレイク値
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ブレイク値")
	float CurrentBreak;

	//敵の最大ブレイク値
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ブレイク値")
	float MaxBreak;

	//敵が死亡したか
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusEnemy")
	bool bIsEnemyDead;

	//QTEが終了した後に作動するブレイク状態を解除を計測するタイマーメソッド
	void BreakTimer();
};
