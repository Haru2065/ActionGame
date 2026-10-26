// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyStatus.generated.h"

// ---------------------------------------------------------------
// デリゲート宣言（「何かが起きた」ことを他のクラスへ知らせる仕組み）
// DYNAMIC_MULTICAST = BPからバインドでき、複数の受信者に同時に通知できる
// ---------------------------------------------------------------

// HPが変化した時にUIへ通知する（HP割合 0.0〜1.0）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyHPBarChanged, float, HPPercent);

// ブレイクゲージが変化した時にUIへ通知する（ブレイク割合 0.0〜1.0）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyBreakBarChanged, float, BreakPercent);

// 敵がBreak状態に突入した瞬間に発火する
// 引数のAActor*は「どの敵がBreakしたか」を区別するためのもの（複数の敵が同時にいても対応できる）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyOnBreak, AActor*, BrokenEnemyActor);

// Break演出中の「確定ヒット」が着弾した時に発火する
// カメラ演出や専用被弾モーションの再生トリガーとして使う（与えたダメージ量）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnemyOnForcedBreakHit, float, ForcedDamageAmount);

/// <summary>
/// 敵のステータス（HP・ブレイクゲージ）を管理するコンポーネント。
/// 敵アクターにアタッチして使う。
/// </summary>
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ACTIONGAME_API UEnemyStatus : public UActorComponent
{
	GENERATED_BODY()

public:

	// コンストラクタ（各パラメータの初期値を設定する）
	UEnemyStatus();

	// 毎フレーム呼ばれる（現在は中身なし）
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ---------------------------------------------------------------
	// デリゲート（BPや他クラスがバインドして通知を受け取る）
	// ---------------------------------------------------------------

	// HPUIの更新用。HPが変化するたびに通知される
	UPROPERTY(BlueprintAssignable, Category = "StatusEnemy")
	FEnemyHPBarChanged EnemyHPBarChanged;

	// ブレイクゲージUIの更新用。ブレイク値が変化するたびに通知される
	UPROPERTY(BlueprintAssignable, Category = "StatusEnemy")
	FEnemyBreakBarChanged EnemyBreakBarChanged;

	// Break状態突入の通知。仲介役（UBreakSequenceManager）がこれを受け取る
	UPROPERTY(BlueprintAssignable, Category = "StatusEnemy")
	FEnemyOnBreak EnemyOnBreak;

	// Break確定ヒット着弾の通知。BP側のカメラ演出や被弾モーション再生に使う
	UPROPERTY(BlueprintAssignable, Category = "Break")
	FEnemyOnForcedBreakHit EnemyOnForcedBreakHit;

	// ---------------------------------------------------------------
	// ダメージ・ブレイク関連の関数
	// ---------------------------------------------------------------

	/// <summary>
	/// 通常攻撃によるダメージ処理。HPを減らし、ブレイクゲージも加算する
	/// </summary>
	/// <param name="totalDamage">プレイヤーの攻撃の計算結果（会心率、会心ダメージ、攻撃力など）</param>
	UFUNCTION(BlueprintCallable, Category = "StatusEnemy")
	void EnemyOnDamage(float totalDamage);

	/// <summary>
	/// ブレイクゲージを加算する。満タンになったらBreak状態へ移行する
	/// </summary>
	/// <param name="amount">加算するブレイク値</param>
	UFUNCTION(BlueprintCallable, Category = "StatusEnemy")
	void AddBreakPoint(float amount);

	/// <summary>
	/// Break演出中の「確定ヒット」専用ダメージ処理。
	/// 通常攻撃と呼び出し元を分けるため別関数にしている。
	/// HPのみ減らし、ブレイクゲージは加算しない
	/// </summary>
	/// <param name="ForcedDamage">確定ヒットのダメージ量</param>
	UFUNCTION(BlueprintCallable, Category = "Break")
	void ApplyForcedBreakDamage(float ForcedDamage);

	// Break状態を解除し、ブレイクゲージを初期値に戻す（演出終了時に呼ぶ）
	UFUNCTION(BlueprintCallable, Category = "Break")
	void ExitBreakState();

	// 現在のHP割合（0.0〜1.0）を取得する
	UFUNCTION(BlueprintPure, Category = "StatusEnemy")
	float GetEnemyHPPercent() const;

	// 現在のブレイクゲージ割合（0.0〜1.0）を取得する
	UFUNCTION(BlueprintPure, Category = "StatusEnemy")
	float GetBreakPercent() const;

	// Break（スタン）中かどうかを取得する。値を読むだけなのでPureにしている
	UFUNCTION(BlueprintPure, Category = "Break")
	bool GetIsStunned() const { return bIsStunned; }

	/// <summary>
	/// CSV等のデータから敵のパラメータを設定する
	/// ※将来実装予定。現在は関数のみ用意
	/// </summary>
	void setEnemyStatus();

protected:
	// ゲーム開始時に呼ばれる（仲介役への登録をここで行う）
	virtual void BeginPlay() override;

	// ブレイクゲージが満タンになった時に呼ばれる、Break状態への移行処理
	void EnterBreakState();

	/// <summary>
	/// HPを減らす内部処理。
	/// HPの変更は必ずこの関数1か所を通す（単一エントリーポイント）
	/// </summary>
	/// <param name="Damage">減らすHP量</param>
	void ApplyHPDamage(float Damage);

	// QTE終了後、一定時間でBreak状態を解除するためのタイマー処理
	// ※未実装。解除タイミングが決まったら中身を書く
	void BreakTimer();

	// ---------------------------------------------------------------
	// 敵のパラメータ
	// ---------------------------------------------------------------

	// 敵の現在の体力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusEnemy")
	float EnemyCurrentHP;

	// 敵の最大体力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusEnemy")
	float EnemyMaxHP;

	// 敵の攻撃力
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusEnemy")
	float EnemyAttackPower;

	// 敵が死亡したか
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StatusEnemy")
	bool bIsEnemyDead;

	// ブレイク値の初期値（Break解除後はこの値に戻る）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ブレイク値")
	float InitBreak;

	// 現在のブレイク値
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ブレイク値")
	float CurrentBreak;

	// 最大ブレイク値（これに達するとBreak状態になる）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ブレイク値")
	float MaxBreak;

	// Break（スタン）状態か。二重発火ガードにも使う
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Break")
	bool bIsStunned;
};