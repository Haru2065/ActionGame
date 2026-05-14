// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ActionGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AActionGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

public:

	/** Constructor */
	AActionGameCharacter();	

	

	//スキルを使えるかどうか
	//bool BisUseSkill;

	/** 現在のスキルポイント */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float SkillPoint = 0.0f;

	/** スキルポイントの最大値 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	float MaxSkillPoint = 100.0f;

	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//座標をためる配列
	UPROPERTY(BlueprintReadOnly, Category = "TraceSystem")
	TArray<FVector2D> TracePoints;

	//軌跡入力できるかどうか
	UPROPERTY(BluePrintReadOnly, Category = "TraceSystem")
	bool BIsTracing;

	//スキルの状態を管理するメソッド
	UFUNCTION(BlueprintCallable, Category = "Skill")
	virtual void SkillControll();

	//スキルを使えるかどうかのフラグメソッド
	bool BIsUseSkill();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillAnimbool")
	bool BCanSkillAnimation;

	//軌跡モード開始メソッド
	UFUNCTION(BlueprintCallable, Category = "Trace")
	virtual void StartTrace();

	//座標を追加する
	UFUNCTION(BlueprintCallable, Category = "TraceSystem")
	void AddTracePoint(FVector2D Point);

	// 解析して攻撃タイプを出す（とりあえず今はログを出すだけ）
	UFUNCTION(BlueprintCallable, Category = "TraceSystem")
	void AnalyzeTrace();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:


	
};