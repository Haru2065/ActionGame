// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BreakSequenceManager.generated.h"

/// <summary>
/// Break演出開始をプレイヤー/カメラ側に中継するためのデリゲート
/// </summary>
/// <param name="">「誰がBreakしたか」を伝えるアクター自身</param>
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBreakSequenceStart, AActor*, BrokenEnemyActor);


/**
 * 
 */
UCLASS()
class ACTIONGAME_API UBreakSequenceManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	// プレイヤー側がゲーム開始時に一度だけBindしておくデリゲート。
	UPROPERTY(BlueprintAssignable, Category = "Break")
	FOnBreakSequenceStart OnBreakSequenceStart;

	/// <summary>
	/// 各Enemyが自分のEnemyOnBreakデリケートから、バインドするメソッド
	/// </summary>
	/// <param name="BrokenEnemyActor">ブレイクした対象の敵</param>
	UFUNCTION()
	void HandleEnemyBreak(AActor* BrokenEnemyActor);
};
