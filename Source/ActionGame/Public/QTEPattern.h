// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Datatable.h"
#include "InputAction.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraShakeBase.h"
#include "QTEPattern.generated.h"

/**
 * 
 */

/// <summary>
/// このパターンがどちらのデバイス向けかを区別するための列挙体
/// StartQTE実行時に、接続デバイスに応じてこのフィールドでプールを絞り込む
/// </summary>
UENUM(BlueprintType)
enum class EQTEDeviceType : uint8
{
	Gamepad				UMETA(DisplayName = "GamePad"),
	KeyboardMouse		UMETA(DisplayName = "KeyboardMouse")
};

USTRUCT(BlueprintType)
struct FQTEPattern : public FTableRowBase
{
	GENERATED_BODY()

public:

	//このパターンがゲームパッド用かキーボード用か
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	EQTEDeviceType DeviceType = EQTEDeviceType::Gamepad;

	//このパターンで要求する入力(1つだけ、シンプルな単体ボタン)
	//キーボード/ゲームパッドどちらの割り当てかはInputMappingContext側で解決させる
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	UInputAction* RequiredAction = nullptr;

	//画面に表示するボタンアイコン
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE")
	UTexture2D* DisplayIcon = nullptr;

	//このパターンが選ばれたときに再生する専用フィニッシュモーション
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "QTE|Presentation")
	class UAnimMontage* FinisherMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QTE|Presentation")
	TSubclassOf<class UCameraShakeBase> CameraShakeClass;

};
