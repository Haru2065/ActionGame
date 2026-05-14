// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharaStatus.h"
#include "PlayerStatus.generated.h"

/**
 * 親クラスを継承
 * プレイヤーステータス
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UPlayerStatus : public UBaseCharaStatus
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable,Category = "Methood")
	virtual void NormalAttack();
};