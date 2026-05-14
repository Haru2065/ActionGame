// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharaStatus.h"

// Sets default values for this component's properties
UBaseCharaStatus::UBaseCharaStatus()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBaseCharaStatus::BeginPlay()
{
	Super::BeginPlay();

	// ...

	//生存状態にする
	IsAlive = true;

	MaxHP = 100;
	
	
}


// Called every frame
void UBaseCharaStatus::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/// <summary>
/// ダメージメソッド
/// </summary>
/// <param name="damage"></param>
void UBaseCharaStatus::TakeDamage(int64 damage)
{
	CurrentHP -= damage;

	if (CurrentHP < 0)
	{
		
	}
}

/// <summary>
/// デバフメソッド
/// </summary>
/// <param name="debuff"></param>
void UBaseCharaStatus::OnDebuff(int64 debuff)
{

}

/// <summary>
/// バフメソッド
/// </summary>
/// <param name="buff"></param>
void UBaseCharaStatus::OnBuff(int64 buff)
{

}


