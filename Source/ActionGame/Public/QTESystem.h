// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QTEPattern.h" // FQTEPattern�\���̂��g�����߂ɕK�v
//#include "Framework/Application/SlateApplication.h"
#include "QTESystem.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UQTESystem : public UActorComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UQTESystem();

	/// <summary>
	/// 敵のブレイクイベントを受け取る関数
	/// </summary>
	/// <param name="BrokenEnemyActor"></param>
	UFUNCTION()
	void HandleEnemyBreak(AActor* BrokenEnemyActor);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//現在のQTEの対象になっている敵
	UPROPERTY(BlueprintReadOnly, Category = "QTE")
	AActor* CurrentBreakTargetEnemy;

	// QTE�p�^�[�����o�^����Ă���DataTable�A�Z�b�g�ւ̎Q��
	// �G�f�B�^���DT_QTEPattern�����蓖�ĂĎg��
	UPROPERTY(EditDefaultsOnly, Category = "QTE")
	UDataTable* QTEPatternTable;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	//QTEを開始するメソッド

	UFUNCTION(BlueprintCallable, Category = "QTE")
	void StartQTE();

	/// <summary>
	/// �Q�[���p�b�h�����肳��Ă��邩�ǂ����𔻒肷��bool�^�̃��\�b�h
	/// </summary>
	/// <returns>���݂̃Q�[���p�b�h�̐ڑ���Ԃ���Ԃ�</returns>
	//bool IsGamePadpadConnected() const;
};