// Fill out your copyright notice in the Description page of Project Settings.


#include "Status/EnemyStatus.h"

// Sets default values for this component's properties
//コンストラクタ初期化処理
UEnemyStatus::UEnemyStatus()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//最大体力を100へ
	EnemyMaxHP = 100.0f;

	//現在の体力も最大に設定
	EnemyCurrentHP = EnemyMaxHP;

	//攻撃力は50に設定
	EnemyAttackPower = 50.0f;

	//生存状態に設定
	bIsEnemyDead = false;

	// ...
}


// Called when the game starts
void UEnemyStatus::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UEnemyStatus::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/// <summary>
/// 敵のダメージ処理
/// </summary>
/// <param name="totalDamage">プレイヤーの攻撃の計算結果(会心率、会心ダメージ、攻撃など）</param>
void UEnemyStatus::EnemyOnDamage(float totalDamage)
{
	//既に敵が死んでいる場合は何もしない
	if (bIsEnemyDead) return;

	//プレイヤーの攻撃の計算結果分体力を減らす
	EnemyCurrentHP -= totalDamage;

	//体力が0未満やMaxHPを超えないように制限
	//FMath::Clamp(値,最小値,最大値)
	EnemyCurrentHP = FMath::Clamp(EnemyCurrentHP, 0.0f, EnemyMaxHP);

	// HPが変化したら登録されている全てのリスナー(UIなど)に通知する
	EnemyHPBarChanged.Broadcast(GetEnemyHPPercent());

	if (EnemyCurrentHP <= 0.0f)
	{
		bIsEnemyDead = true;
	}
}

/// <summary>
/// プレイヤーをCSV等のデータから取得し、パラメータを設定するメソッド
/// ※将来的に実装するため現在はメソッドのみ用意
/// </summary>
void UEnemyStatus::setEnemyStatus(){}

float UEnemyStatus::GetEnemyHPPercent() const
{
	//０になったらエラーが起こらないように０で固定させる
	if (EnemyMaxHP <= 0.0) return 0.0f;
	
	return EnemyCurrentHP / EnemyMaxHP;
}

