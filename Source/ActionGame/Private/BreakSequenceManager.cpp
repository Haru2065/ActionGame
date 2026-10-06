// Fill out your copyright notice in the Description page of Project Settings.


#include "BreakSequenceManager.h"

/// <summary>
/// Enemy側から届いたBreak通知を、そのままプレイヤー/カメラ側に中継するメソッド
/// </summary>
/// <param name="BrokenEnemyActor">ブレイクした対象の敵</param>
void UBreakSequenceManager::HandleEnemyBreak(AActor* BrokenEnemyActor)
{
	OnBreakSequenceStart.Broadcast(BrokenEnemyActor);
}