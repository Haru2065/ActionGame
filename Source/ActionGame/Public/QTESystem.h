// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QTESystem.generated.h"



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONGAME_API UQTESystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQTESystem();

	/// <summary>
	/// 敵のブレイクイベントを受け取る関数
	/// </summary>
	/// <param name="BrokenEnemyActor"></param>
	UFUNCTION()
	void HandleEnemyBreak(AActor* BrokenEnemyActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//現在のQTEの対象になっている敵
	UPROPERTY(BlueprintReadOnly, Category = "QTE")
	AActor* CurrentBreakTargetEnemy;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void StartQTE();
};
