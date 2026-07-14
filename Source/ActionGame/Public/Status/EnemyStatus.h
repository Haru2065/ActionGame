// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyStatus.generated.h"

// HPが変化した時にBP側へ通知するためのデリゲートを宣言
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam = 「引数を1つ持つ、BPからバインド可能な通知」という意味
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FenemyHPBarChanged, float, HPPercent);


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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "StatusEnemy")
	void EnemyOnDamage(float totalDamage);

	void setEnemyStatus();

	UFUNCTION(BlueprintPure, Category = "StatusEnemy")
	float GetEnemyHPPercent() const;

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

	//敵が死亡したか
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusEnemy")
	bool bIsEnemyDead;
		
};
