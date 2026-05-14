// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatus.h"

void UPlayerStatus::BeginPlay()
{
	//親クラスの処理を呼び出す
	Super::BeginPlay();

	//プレイヤーを最大体力にする
	CurrentHP = MaxHP;

}

void UPlayerStatus::NormalAttack()
{
	
}