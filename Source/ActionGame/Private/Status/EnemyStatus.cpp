// Fill out your copyright notice in the Description page of Project Settings.

#include "Status/EnemyStatus.h"
#include "BreakSequenceManager.h"

// コンストラクタ：各パラメータの初期値を設定する
UEnemyStatus::UEnemyStatus()
{
	// TickComponentを毎フレーム呼ぶ設定（使わないならfalseにすると軽くなる）
	PrimaryComponentTick.bCanEverTick = true;

	// 最大体力を100に設定
	EnemyMaxHP = 1000.0f;

	// 現在の体力も最大に設定
	EnemyCurrentHP = EnemyMaxHP;

	// 攻撃力は50に設定
	EnemyAttackPower = 50.0f;

	// ブレイク値の初期値は0
	InitBreak = 0.0f;

	// 現在のブレイク値を初期値にする
	CurrentBreak = InitBreak;

	// 最大ブレイク値は100に設定
	MaxBreak = 100.0f;

	// 生存状態に設定
	bIsEnemyDead = false;

	// 非スタン状態（Breakしていない状態）で初期化
	bIsStunned = false;
}

// ゲーム開始時に呼ばれる
void UEnemyStatus::BeginPlay()
{
	Super::BeginPlay();

	// このワールドの仲介役（Subsystem）を取得
	if (UWorld* world = GetWorld())
	{
		if (UBreakSequenceManager* BreakManager = world->GetSubsystem<UBreakSequenceManager>())
		{
			// 自分がBreakしたら仲介役のHandleEnemyBreakが呼ばれるよう登録
			EnemyOnBreak.AddDynamic(BreakManager, &UBreakSequenceManager::HandleEnemyBreak);
		}
	}
}

// 毎フレーム呼ばれる（現在は中身なし）
void UEnemyStatus::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/// <summary>
/// 通常攻撃によるダメージ処理
/// </summary>
/// <param name="totalDamage">プレイヤーの攻撃の計算結果（会心率、会心ダメージ、攻撃力など）</param>
void UEnemyStatus::EnemyOnDamage(float totalDamage)
{
	// 既に死んでいる場合は何もしない
	if (bIsEnemyDead) return;

	// 先にHPを減らし、死亡判定まで済ませる
	ApplyHPDamage(totalDamage);

	// ダメージを与えるたびにブレイクゲージも一定量加算する
	// ※今の一撃で死亡した場合は、AddBreakPoint内の死亡チェックで弾かれる
	AddBreakPoint(10);
}

/// <summary>
/// HPを減らす内部処理（HP変更はこの関数だけが行う）
/// </summary>
/// <param name="Damage">減らすHP量</param>
void UEnemyStatus::ApplyHPDamage(float Damage)
{
	// 既に死んでいる場合は何もしない
	if (bIsEnemyDead) return;

	// HPからダメージを引き、0〜最大HPの範囲に収める
	// FMath::Clamp(値, 最小値, 最大値)
	EnemyCurrentHP = FMath::Clamp(EnemyCurrentHP - Damage, 0.0f, EnemyMaxHP);

	// HPが変化したので、登録されているリスナー（UIなど）に通知する
	EnemyHPBarChanged.Broadcast(GetEnemyHPPercent());

	// HPが0になったら死亡扱いにする
	if (EnemyCurrentHP <= 0.0f)
	{
		bIsEnemyDead = true;
	}
}

/// <summary>
/// Break演出中の「確定ヒット」専用ダメージ処理
/// </summary>
/// <param name="ForcedDamage">確定ヒットのダメージ量</param>
void UEnemyStatus::ApplyForcedBreakDamage(float ForcedDamage)
{
	// Break中でなければ無効（通常時に誤って呼ばれても何も起きない）
	if (!bIsStunned) return;

	// 既に死んでいる場合は何もしない
	if (bIsEnemyDead) return;

	// ブレイクゲージは加算せず、HPだけ減らす
	ApplyHPDamage(ForcedDamage);

	// 「確定ヒットが当たった」ことをBP側（カメラ演出・被弾モーション）へ通知する
	EnemyOnForcedBreakHit.Broadcast(ForcedDamage);
}

/// <summary>
/// ブレイク値を加算する。満タンになったらBreak状態へ移行する
/// </summary>
/// <param name="amount">加算するブレイク値</param>
void UEnemyStatus::AddBreakPoint(float amount)
{
	// 敵が死亡済みならブレイクゲージを操作する必要がないので早期リターン
	if (bIsEnemyDead) return;

	// 引数分だけブレイクゲージを加算
	CurrentBreak += amount;

	// 0〜最大値の範囲に収める
	CurrentBreak = FMath::Clamp(CurrentBreak, 0.0f, MaxBreak);

	// ブレイクゲージが変化したことをUIへ通知する
	EnemyBreakBarChanged.Broadcast(GetBreakPercent());

	// 最大値に到達したらBreak状態へ移行する
	if (CurrentBreak >= MaxBreak)
	{
		EnterBreakState();
	}
}

// ブレイクゲージが満タンに達した際に呼ばれる、Break状態への移行処理
void UEnemyStatus::EnterBreakState()
{
	// すでにスタン中なら何もしない（二重発火防止）
	if (bIsStunned) return;

	// 敵をスタン（Break）状態にする
	bIsStunned = true;

	// Breakが発生したことを、登録されている全てのリスナーに通知する
	// GetOwner() で「このコンポーネントを持っている敵アクター」自身を渡す
	EnemyOnBreak.Broadcast(GetOwner());
}

// Break状態を解除し、ブレイクゲージを初期値に戻す
void UEnemyStatus::ExitBreakState()
{
	// Break中でなければ何もしない
	if (!bIsStunned) return;

	// スタン解除
	bIsStunned = false;

	// ブレイクゲージを初期値に戻す
	CurrentBreak = InitBreak;

	// ゲージが0に戻ったことをUIへ通知する
	EnemyBreakBarChanged.Broadcast(GetBreakPercent());
}

// QTE終了後、一定時間でBreak状態を解除するためのタイマー処理
// ※未実装。解除タイミングが決まったら、ここでExitBreakStateを呼ぶ予定
void UEnemyStatus::BreakTimer() {}

/// <summary>
/// CSV等のデータから敵のパラメータを設定する
/// ※将来実装予定。現在は関数のみ用意
/// </summary>
void UEnemyStatus::setEnemyStatus() {}

// 現在のHP割合（0.0〜1.0）を取得する
float UEnemyStatus::GetEnemyHPPercent() const
{
	// 最大HPが0以下だと0除算になるため、0で固定して返す
	if (EnemyMaxHP <= 0.0f) return 0.0f;

	return EnemyCurrentHP / EnemyMaxHP;
}

// 現在のブレイクゲージ割合（0.0〜1.0）を取得する
float UEnemyStatus::GetBreakPercent() const
{
	// 最大ブレイク値が0以下だと0除算になるため、0で固定して返す
	if (MaxBreak <= 0.0f) return 0.0f;

	return CurrentBreak / MaxBreak;
}