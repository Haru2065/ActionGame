// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QTEPattern.h"
#include "QTESystem.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UQTESystem : public UActorComponent
{
	GENERATED_BODY()

public:

	/// <summary>
	/// 敵のブレイクイベントを受け取る関数
	/// EnemyStatus.hのEnemyOnBreakデリケートに、敵のBeginPlayeでバインド
	/// </summary>
	/// <param name="BrokenEnemyActor">ブレイクした敵のアクター</param>
	UFUNCTION()
	void HandleEnemyBreak(AActor* BrokenEnemyActor);

protected:

	UQTESystem();

	virtual void BeginPlay() override;

	//現在のQTEの対象になっている敵(ブレイクした敵そのもの)
	UPROPERTY(BlueprintReadOnly, Category = "QTE")
	AActor* CurrentBreakTargetEnemy;

	//QTEpattern一覧が登録されているDatatableアセット(ここにQTEのデータアセットをエディター上で割り当て)
	UPROPERTY(EditDefaultsOnly, Category = "QTE")
	UDataTable* QTEPatternTable;

	/// <summary>
	/// QTE中に使う入力マッピング
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "QTE")
	class UInputMappingContext* QTEMappingContext;

	/// <summary>
	/// 通常操作(移動・攻撃)時に使う入力マッピング(IMC_Defalut)
	/// </summary>
	UPROPERTY(EditAnywhere, Category = "QTE")
	class UInputMappingContext* DefaultMappingContext;

	//StartQTE()で抽選された「今回のQTEパターン」を保持しておく変数。
	//RandomShowQTE()や入力受付から参照する。
	UPROPERTY(BlueprintReadOnly, Category = "QTE")
	FQTEPattern CurrentQTEPattern;

	//現在コントローラーが接続されているかどうかを保持するフラグ
	UPROPERTY(BlueprintReadOnly, Category = "QTE")
	bool IsConnected;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	/// <summary>
	/// QTEを開始するメソッド
	/// Datatableからパターンを絞りこみ→ランダム抽選→IMC切り替えまで担当
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "QTE")
	void StartQTE();

	/// <summary>
	/// StartQTE()で抽選されたCurrentQTEPatternを使って
	/// 実際のスロー演出開始・入力受付登録を行うメソッド
	/// </summary>
	void RandomShowQTE();

	/// <summary>
	/// QTEの入力が正しく実行された時に呼ぶメソッド
	/// 入力は１つなため、呼ばれた時点でQTE成功として扱う
	/// </summary>
	UFUNCTION()
	void OnQTEInputReceived();


	/// <summary>
	/// QTEを終了させるメソッド
	/// 成功時失敗時どちらのルートからも必ずここを通す想定
	/// QTE用IMCを外し通常操作IMCへ戻す、スロー演出も戻す。
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "QTE")
	void EndQTE();
};