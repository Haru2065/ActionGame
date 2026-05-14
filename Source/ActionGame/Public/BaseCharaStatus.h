// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseCharaStatus.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UBaseCharaStatus : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseCharaStatus();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:

	//int32はプラットフォームごとにサイズが変わる可能性があるため、64bitに固定する
	//BlueprintReadWriteはUPROPERTYよう]

	//BlueprintReadWriteはGetterとSetter
	//BlueprintReadOnlyはGetter

	//キャラクターの最大体力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	int32 MaxHP;

	//現在の体力
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category = "Parameter")
	int32 CurrentHP;

	//攻撃力
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Parameter")
	int32 AttackPower;

	//バフ力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float BuffPower;

	//デバフ力
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parameter")
	float DebuffPower;

	
private :

	//生存しているか
	bool IsAlive;

protected:
	
	//BlueprintCallable関数用
	//BlueprintCallableはUFUNCTIONよう


	//ダメージメソッド
	UFUNCTION(BlueprintCallable,Category = "Status")
	virtual void TakeDamage(int64 damge);

	//デバフメソッド
	UFUNCTION(BlueprintCallable, Category = "Status")
	virtual void OnDebuff(int64 debuff);

	//バフメソッド
	UFUNCTION(BlueprintCallable, Category = "Status")
	virtual void OnBuff(int64 buff);

};