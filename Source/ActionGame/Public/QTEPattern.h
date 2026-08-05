// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Datatable.h"
#include "InputAction.h" 
#include "QTEPattern.generated.h"

/**
 * 
 */

//このパターンがどちらのデバイス向けかを区別するための列挙体
UENUM(BlueprintType)
enum class EQTEDeviceType : uint8
{
	Gamepad			UMETA(DisplayName = "GamePad"),
	KeyboardMouse	UMETA(DisplayName = "KeyboardMouse")
};

//QTEの「１回分の入力」を表す構造体
USTRUCT(BlueprintType)
struct FQTEInoutStep
{
	GENERATED_BODY()
	
public:

	//入力で要求するInputAction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	UInputAction* RequiredAction = nullptr;

	//画面に表示させるQTEのボタンアイコン
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	UTexture2D* DisplayIcon = nullptr;
};

/// <summary>
/// DataTableの実際のデータ情報(1行分の=QTEパターン１つ分のデータ)
/// </summary>
USTRUCT(BlueprintType)
struct FQTEPattern : public FTableRowBase
{
	GENERATED_BODY()

public:

	//デバイスパターン
	//QTEシステムで、StartQTE()を実行時に接続デバイスに応じてこのフィールドでプールを絞り込む
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	EQTEDeviceType DeviceType = EQTEDeviceType::Gamepad;

	//入力するInputActionの設定
	//実際に要求する入力の並び、アイコンとInputActionを割り当てできるように配列化する
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	TArray<FQTEInoutStep> InputSequence;

	//将来敵に難易度拡張に使用
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	int32 LevelInput = 0;
};