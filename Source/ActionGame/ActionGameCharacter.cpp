// Copyright Epic Games, Inc. All Rights Reserved.

#include "ActionGameCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ActionGame.h"

AActionGameCharacter::AActionGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AActionGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AActionGameCharacter::Look);
	}
	else
	{
		UE_LOG(LogActionGame, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

	// SetupPlayerInputComponent 内
	//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AActionGameCharacter::Attack);
}

void AActionGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AActionGameCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AActionGameCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AActionGameCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AActionGameCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AActionGameCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AActionGameCharacter::SkillControll()
{
	//スキルが使える場合はスキルを発動
	if (BIsUseSkill())
	{
		//アニメーションをできるようにする
		BCanSkillAnimation = true;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("SkillUSE!"));
		}
	}

	//スキルが使えない場合は何もしない
	else
	{
		//アニメーションをできないようにする
		BCanSkillAnimation = false;

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("NOSKILL!!"));
		}
	}
}

//スキルが使えるかどうかチェックするメソッド
bool AActionGameCharacter::BIsUseSkill()
{
	//もしスキルポイントが最大になったらtrueで返す
	return SkillPoint >= MaxSkillPoint;
}

/// <summary>
///軌跡入力の開始準備を行うメソッド
/// </summary>
void AActionGameCharacter::StartTrace()
{
	//前回のデータが残っている可能性があるため、配列を空にする
	TracePoints.Empty();

	//ログを出力して、システムが開始されたかエディター上で確認する
	UE_LOG(LogTemp, Warning, TEXT("TraceStart!"));
}

/// <summary>
///毎フレームスティック座標を記録するメソッド
/// </summary>
/// <param name="Point"></param>
void AActionGameCharacter::AddTracePoint(FVector2D Point)
{
	//「現在上のスティック位置を配列の末尾に追加していく」
	TracePoints.Add(Point);
}

/// <summary>
///貯まった座標データを解析し、攻撃の種類を決定するメソッド
/// </summary>
void AActionGameCharacter::AnalyzeTrace()
{
	//データが２点未満(一瞬しか触れていない等)の場合は解析不能なので中断する
	if (TracePoints.Num() < 2) return;

	// 配列の最初（指を入れた瞬間）と最後（指を離した瞬間）の座標を取得
	FVector2D Start = TracePoints[0];
	FVector2D End = TracePoints.Last();

	//開始点から終了点へのベクトルを計算(これが「描いた方向」になる)
	FVector2D Direction = End - Start;


	//解析結果をログに出力
	// Points: 記録された点の総数 / Direction: どの方向にどれだけ動いたか
	UE_LOG(LogTemp, Warning, TEXT("Trace Analyzed! Points: %d, Direction: %s"), TracePoints.Num(), *Direction.ToString());
}