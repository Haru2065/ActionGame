#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerBreakSequenceComponent.generated.h"

class AActor;

// Break関連の通知に使うデリゲート（引数：Breakした敵アクター）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerBreakSequence, AActor*, BrokenEnemyActor);

/**
 * プレイヤー側でBreak演出の「ボタン待ち・開始・入力ロック・確定ヒット」を管理するコンポーネント。
 * 流れ：敵がBreak → ボタン待ち → ボタン or 時間切れ → 演出中(入力ロック) → 終了
 * 見た目の演出（UI・モンタージュ・カメラ）はデリゲート経由でBP側が行う。
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UPlayerBreakSequenceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// コンストラクタ
	UPlayerBreakSequenceComponent();

	// ---------------------------------------------------------------
	// 調整用パラメータ（BPのDetailsパネルで変更できる）
	// ---------------------------------------------------------------

	// ボタン待ちの時間（秒）。
	// 0 = 待たずに即発動 / 正の値 = その秒数後に自動発動 / 負の値 = ボタンを押すまで待つ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Break")
	float BreakWaitTime = 3.0f;

	// ---------------------------------------------------------------
	// BPから呼ぶ関数
	// ---------------------------------------------------------------

	// フィニッシュ発動ボタンが押された時に呼ぶ（待機中でなければ何もしない）
	UFUNCTION(BlueprintCallable, Category = "Break")
	void TryStartFinisher();

	// モンタージュのヒットNotifyから呼び、現在のBreak対象へ確定ダメージを与える
	UFUNCTION(BlueprintCallable, Category = "Break")
	void ApplyForcedBreakDamage(float ForcedDamage);

	// フィニッシャー演出の完了時にBPから呼び、通常操作へ戻す（敵のBreak状態も解除する）
	UFUNCTION(BlueprintCallable, Category = "Break")
	void EndBreakSequence();

	// ---------------------------------------------------------------
	// 状態の取得
	// ---------------------------------------------------------------

	// Break関連の処理中か（ボタン待ち〜演出終了まで true）
	UFUNCTION(BlueprintPure, Category = "Break")
	bool IsBreakSequenceActive() const { return bIsBreakSequenceActive; }

	// ボタン待ち中か（UIの表示判定などに使う）
	UFUNCTION(BlueprintPure, Category = "Break")
	bool IsWaitingForFinisher() const { return bIsWaitingForFinisher; }

	// 現在のBreak対象の敵を取得する（いなければnullptr）
	UFUNCTION(BlueprintPure, Category = "Break")
	AActor* GetCurrentBreakTarget() const { return CurrentBreakTarget.Get(); }

	// ---------------------------------------------------------------
	// デリゲート（BP側がバインドして見た目の演出を行う）
	// ---------------------------------------------------------------

	// ボタン待ち開始。「ボタンアイコンUIを出す」などに使う
	UPROPERTY(BlueprintAssignable, Category = "Break")
	FOnPlayerBreakSequence OnBreakWaitStarted;

	// ボタン待ち終了。「ボタンアイコンUIを消す」などに使う
	UPROPERTY(BlueprintAssignable, Category = "Break")
	FOnPlayerBreakSequence OnBreakWaitEnded;

	// フィニッシャー演出開始。モンタージュ再生・カメラワークなどに使う
	UPROPERTY(BlueprintAssignable, Category = "Break")
	FOnPlayerBreakSequence OnBreakSequenceStarted;

	// 通常操作へ戻った後の見た目の後処理に使う
	UPROPERTY(BlueprintAssignable, Category = "Break")
	FOnPlayerBreakSequence OnBreakSequenceEnded;

protected:
	// ゲーム開始時：仲介役の通知に登録する
	virtual void BeginPlay() override;

	// ゲーム終了時：登録解除・タイマー停止・入力ロック解除
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	// 仲介役から「敵がBreakした」通知を受け取る（AddDynamicで登録するのでUFUNCTION必須）
	UFUNCTION()
	void HandleBreakSequenceStart(AActor* BrokenEnemyActor);

	// 待ち時間が過ぎた時に呼ばれる（自動でフィニッシュを開始する）
	void OnWaitTimeExpired();

	// フィニッシュを実際に開始する（待機終了 → 入力ロック → 演出開始の合図）
	void StartFinisher();

	// 移動・視点の入力を無効化 / 有効化する
	void SetGameplayInputLocked(bool bLocked);

	// 現在のBreak対象（敵が先に消えても安全なように弱参照で持つ）
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> CurrentBreakTarget;

	// ボタン待ちの時間を計るタイマー
	FTimerHandle WaitTimerHandle;

	// Break関連の処理中か（二重開始のガードにも使う）
	bool bIsBreakSequenceActive = false;

	// ボタン待ち中か
	bool bIsWaitingForFinisher = false;

	// 入力ロック中か
	bool bHasLockedGameplayInput = false;
};