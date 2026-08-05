// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QTEPattern.h" // FQTEPattern構造体を使うために必要
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

	// QTEパターンが登録されているDataTableアセットへの参照
	// エディタ上でDT_QTEPatternを割り当てて使う
	UPROPERTY(EditDefaultsOnly, Category = "QTE")
	UDataTable* QTEPatternTable;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "QTE")
	void StartQTE();

	/// <summary>
	/// ゲームパッドが判定されているかどうかを判定するbool型のメソッド
	/// </summary>
	/// <returns>現在のゲームパッドの接続状態かを返す</returns>
	//bool IsGamePadpadConnected() const;
};