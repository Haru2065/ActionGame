// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerStatus.generated.h"

// HPが変化した時にBP側へ通知するためのデリゲートを宣言
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam = 「引数を1つ持つ、BPからバインド可能な通知」という意味
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHPBarChanged, float, HPPercent);

/// <summary>
/// プレイヤーステータス
/// </summary>
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))

class ACTIONGAME_API UPlayerStatus : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	//コンストラクタ
	UPlayerStatus();

	//BP側でHPUIを減らしたり増やしたりするイベントをバインドする
	UPROPERTY(BlueprintAssignable, Category = "StatusPlayer")
	FHPBarChanged HPBarChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//ダメージメソッド
	UFUNCTION(BlueprintCallable, Category = "StatusPlayer")
	void OnDamage(float totalDamage);

	/// <summary>
	///　プレイヤーをCSV等のデータから取得し、パラメータを設定するメソッド
	///  ※将来的に実装するため現在はメソッドのみ用意
	/// </summary>
	void setPlayerStatus();

	UFUNCTION(BlueprintPure, Category = "StatusPlayer")
	float GetHPPercent() const;


protected:

	///プレイヤーのパラメータ

	//現在の体力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusRunTime")
	float CurrentHP;


	//最大体力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusRunTime")
	float MaxHP;


	//攻撃力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusRunTime")
	float AttackPower;

	//会心率
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusRuntime")
	float CurrentCritical;

	//会心ダメージ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusRuntime")
	float CurrentCriticalDamage;

	//死亡したか
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusRuntime")
	bool bIsDead;
};
