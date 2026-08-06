// Fill out your copyright notice in the Description page of Project Settings.


#include "QTESystem.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"

//#include "Framework/Application/SlateApplication.h"

// Sets default values for this component's properties
UQTESystem::UQTESystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UQTESystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	APlayerController* PC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;

	if (PC)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

		if (Subsystem)
		{
			Subsystem->AddMappingContext(QTEMappingContext, 0);
		}
	}
}


// Called every frame
void UQTESystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UQTESystem::HandleEnemyBreak(AActor* BrokenEnemyActor)
{
	//nullチェック
	if (!BrokenEnemyActor) return;

	//QTE攻撃を与える対象を設定
	CurrentBreakTargetEnemy = BrokenEnemyActor;

	//QTE開始
	StartQTE();
}

/// <summary>
/// QTE開始メソッド
/// </summary>
void UQTESystem::StartQTE()
{
	if (!QTEPatternTable) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	APlayerController* PC = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;

	if (!PC) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	if (!Subsystem) return;

	// DataTableから全行を取得
	TArray<FQTEPattern*> AllPatterns;
}

void UQTESystem::EndQTE()
{

}

/// <summary>
/// ゲームパッドが判定されているかどうかを判定するbool型のメソッド
/// </summary>
/// <returns>現在のゲームパッドの接続状態かを返す</returns>
//bool UQTESystem::IsGamePadpadConnected() const
//{
//	//現在ゲームパッドが接続されているかどうかを判定
//	return FSlateApplication::Get().IsGamepadAttached();
//}

void UQTESystem::RandomShowQTE()
{

}

void GetPattern()
{

}