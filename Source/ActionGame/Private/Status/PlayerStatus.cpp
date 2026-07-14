// Fill out your copyright notice in the Description page of Project Settings.


#include "Status/PlayerStatus.h"


// Sets default values for this component's properties
//コンストラクタ初期化処理
UPlayerStatus::UPlayerStatus()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	//デバックテスト用初期化

	//最大体力を100へ
	MaxHP = 100.0f;

	//現在の体力も最大に設定
	CurrentHP = MaxHP;

	//攻撃力は50に設定
	AttackPower = 50.0f;

	//会心を0.5に設定
	CurrentCritical = 0.05f;

	//会心ダメージを1.5
	CurrentCriticalDamage = 1.5f;

	//生存状態に設定
	bIsDead = false;

	// ...
}


// Called when the game starts
void UPlayerStatus::BeginPlay()
{
	Super::BeginPlay();

	

	// ...
	
}


// Called every frame
void UPlayerStatus::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

/// <summary>
/// ダメージメソッド
/// </summary>
/// <param name="totalDamage">敵の攻撃等の計算結果（ダメージ数）</param>
void UPlayerStatus::OnDamage(float totalDamage)
{
	//既に死んでいる場合は何もしない
	if (bIsDead)return;

	//敵の攻撃力類の計算結果分体力を減らす
	CurrentHP -= totalDamage;

	//体力が0未満やMaxHPを超えないように制限
	//FMath::Clamp(値,最小値,最大値)
	CurrentHP = FMath::Clamp(CurrentHP, 0.0f, MaxHP);

	// HPが変化したら登録されている全てのリスナー(UIなど)に通知する
	HPBarChanged.Broadcast(GetHPPercent());

	//体力が0になったら死亡フラグをtrueに
	if (CurrentHP <= 0.0f)
	{
		bIsDead = true;
	}
}

/// <summary>
/// プレイヤーをCSV等のデータから取得し、パラメータを設定するメソッド
/// ※将来的に実装するため現在はメソッドのみ用意
/// </summary>
void UPlayerStatus::setPlayerStatus(){}

/// <summary>
/// 現在のHP割合(0.0～1.0)を計算して返す関数
/// </summary>
/// <returns></returns>
float UPlayerStatus::GetHPPercent()const
{
	//０になったらエラーが起こらないように０で固定させる
	if (MaxHP <= 0.0) return 0.0f;

	return CurrentHP / MaxHP;
}

