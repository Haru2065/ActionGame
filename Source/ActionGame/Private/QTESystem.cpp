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
	
	//毎フレームTickComponent呼ぶ設定
	PrimaryComponentTick.bCanEverTick = true;

	//コントローラー接続フラグの初期値。判定処理が実装されるまでは仮でfalse扱い
	IsConnected = false;

	// ...
}


// Called when the game starts
void UQTESystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	APlayerController* playerController = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;

	if (playerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());

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
	// DataTableが設定されていなければ処理できないので中断
	if (!QTEPatternTable) return;

	//このコンポーネントを持っているアクター(プレイヤー)からPlayerControllerを取得
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APlayerController* playerController = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;

	if (!playerController) return;

	// Enhanced InputのSubsystemを取得
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());

	if (!Subsystem) return;

	// 現在のデバイス接続状態に応じて、絞り込むDeviceTypeを決定
	// (IsConnectedの中身は今後、実際の接続判定処理で更新する想定。現状は仮のtrue/false切替用)
	// IsConnected == true なら Gamepad、false なら KeyboardMouse
	EQTEDeviceType targetDeveoceType = IsConnected ? EQTEDeviceType::Gamepad : EQTEDeviceType::KeyboardMouse;

	// DataTableから全行を取得
	TArray<FQTEPattern*> AllPatterns;
	QTEPatternTable->GetAllRows<FQTEPattern>(TEXT("StartQTE"), AllPatterns);

	//デバイスタイプが一致する行だけを候補として絞り込む
	TArray<FQTEPattern*> FilteredPatterns;
	for (FQTEPattern* Pattern : AllPatterns)
	{
		if (Pattern && Pattern->DeviceType == targetDeveoceType)
		{
			FilteredPatterns.Add(Pattern);
		}
	}

	// 候補が1つもなければ処理を中断
	if (FilteredPatterns.Num() == 0) return;

	//候補の中からランダムに１つのインデックスを選択
	int32 RandomIndex = FMath::RandRange(0, FilteredPatterns.Num() - 1);

	//選ばれたインデックスから、実際のパターンデータを取り出す
	FQTEPattern* SelectedPattern = FilteredPatterns[RandomIndex];

	//万が一中身がnullだったときの保険nullチェック
	if (!SelectedPattern) return;

	//選ばれたパターンの中身をメンバー変数にコピーして保持
	//Datatableが再読み込み時にポインターの場合無効を防ぐために値としてコピーする
	CurrentQTEPattern = *SelectedPattern;

	// 通常操作用のIMCを一時的に外す(QTE中は移動・攻撃を受け付けないようにする)
	if (DefaultMappingContext)
	{
		Subsystem->RemoveMappingContext(DefaultMappingContext);
	}

	// QTE専用のIMCを追加する(優先度1、DefaultMappingContextの0より高く設定)
	if (QTEMappingContext)
	{
		Subsystem->AddMappingContext(QTEMappingContext, 1);
	}

	// 抽選が終わったので、実際の演出・入力受付処理へ進む
	RandomShowQTE();
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

/// <summary>
/// 実際のスロー演出やQTEUIのアイコン表示と入力受付を行うメソッド
/// </summary>
void UQTESystem::RandomShowQTE()
{

}

/// <summary>
/// QTEを終了させるメソッド
/// 成功、失敗のどちらかの結果でも必ずここで実行される
/// QTE用IMCを解除し、通常操作時のIMCに切り替える
/// </summary>
void UQTESystem::EndQTE()
{
	// プレイヤー本体からPlayerControllerを取得
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APlayerController* PlayerController = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
	if (!PlayerController) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	if (!Subsystem) return;

	// QTE用のIMCを外す
	if (QTEMappingContext)
	{
		Subsystem->RemoveMappingContext(QTEMappingContext);
	}

	// 通常操作用のIMCを復活させる
	if (DefaultMappingContext)
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}