#include "Status/PlayerBreakSequenceComponent.h"

#include "BreakSequenceManager.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Status/EnemyStatus.h"
#include "TimerManager.h"

// コンストラクタ
UPlayerBreakSequenceComponent::UPlayerBreakSequenceComponent()
{
	// Tickは使わないのでオフにする（軽量化）
	PrimaryComponentTick.bCanEverTick = false;
}

// ゲーム開始時：仲介役の通知に登録する
void UPlayerBreakSequenceComponent::BeginPlay()
{
	Super::BeginPlay();

	//ワールドを取得できた場合のみ処理このワールド内のSubsystemを取得
	if (UWorld* World = GetWorld())
	{
		if (UBreakSequenceManager* BreakManager = World->GetSubsystem<UBreakSequenceManager>())
		{
			// 敵がBreakしたら HandleBreakSequenceStart が呼ばれるよう登録する
			BreakManager->OnBreakSequenceStart.AddDynamic(this, &UPlayerBreakSequenceComponent::HandleBreakSequenceStart);
		}
	}
}

// ゲーム終了時：登録解除・タイマー停止・入力ロック解除
void UPlayerBreakSequenceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		// 待機タイマーが残っていると、破棄後に呼ばれて危険なので止める
		World->GetTimerManager().ClearTimer(WaitTimerHandle);
		World->GetTimerManager().ClearTimer(FinisherFallbackTimerHandle);

		if (UBreakSequenceManager* BreakManager = World->GetSubsystem<UBreakSequenceManager>())
		{
			// 仲介役への登録を解除する
			BreakManager->OnBreakSequenceStart.RemoveDynamic(this, &UPlayerBreakSequenceComponent::HandleBreakSequenceStart);
		}
	}

	// 入力ロックが残ったままにならないよう解除する
	SetGameplayInputLocked(false);

	Super::EndPlay(EndPlayReason);
}

// 仲介役から「敵がBreakした」通知を受け取った時の処理
void UPlayerBreakSequenceComponent::HandleBreakSequenceStart(AActor* BrokenEnemyActor)
{
	// 無効な敵、または既に別のBreak処理中なら無視する（同時に1つだけ処理する）
	if (!IsValid(BrokenEnemyActor) || bIsBreakSequenceActive)
	{
		// 「前のBreakが終わっていない」状態で無視されたことが分かるようにする
		UE_LOG(LogTemp, Warning, TEXT("[Break] Break通知を無視した: 敵有効=%d すでにActive=%d"),
			IsValid(BrokenEnemyActor), bIsBreakSequenceActive);
		return;
	}

	// Break関連の処理中にする（二重開始のガード）
	bIsBreakSequenceActive = true;

	// Breakした敵を覚えておく
	CurrentBreakTarget = BrokenEnemyActor;

	// ボタン待ち状態に入る
	bIsWaitingForFinisher = true;
	FinisherInputUnlockTime = GetWorld()->GetTimeSeconds();
	FinisherWaitDuration = FMath::Max(BreakWaitTime, 0.0f);
	UE_LOG(LogTemp, Display, TEXT("FinisherWait [%s]: GameTime=%.3f RealTime=%.3f Unlock=%.3f Duration=%.3f"),
		*GetPathName(), GetWorld()->GetTimeSeconds(), GetWorld()->GetRealTimeSeconds(), FinisherInputUnlockTime, FinisherWaitDuration);

	// ボタン待ちの間も、移動・視点の入力を止める
	// ※攻撃などのキー入力は、この関数では止まらない（BP側で止める）
	SetGameplayInputLocked(true);

	if (BreakWaitTime > 0.0f)
	{
		// UI表示から設定された秒数だけ待つ（入力禁止時間は加算しない）。
		// UIの表示イベント内からも残り時間を読めるよう、通知前に開始する。
		GetWorld()->GetTimerManager().SetTimer(
			WaitTimerHandle, this, &UPlayerBreakSequenceComponent::OnWaitTimeExpired, FinisherWaitDuration, false);
	}

	// BP側へ合図（ボタンアイコンUIの表示など）
	OnBreakWaitStarted.Broadcast(BrokenEnemyActor);

	if (BreakWaitTime == 0.0f && bIsWaitingForFinisher)
	{
		// 0は明示的な即時自動発動設定。
		StartFinisher();
	}
	// 負の値：タイマーをセットしない（ボタンが押されるまで待つ）
}

// UI表示直後から入力を受け付ける。
bool UPlayerBreakSequenceComponent::CanStartFinisher() const
{
	// Worldチェックは不要。Break状態でWaitingForFinisherなら常に入力を受け付ける。
	// ※攻撃等のキー入力はBP側で管理するが、Break待機中のXキー入力は常に有効にする。
	return bIsBreakSequenceActive && bIsWaitingForFinisher;
}

// ボタンが押された時（BPの入力イベントから呼ばれる）
void UPlayerBreakSequenceComponent::TryStartFinisher()
{
	// Xキーの入力がC++まで届いたか確認するログ（Warningなので黄色で目立つ）
	UE_LOG(LogTemp, Warning, TEXT("[Break] TryStartFinisher呼ばれた: Active=%d Waiting=%d"),
		bIsBreakSequenceActive, bIsWaitingForFinisher);

	// 発動できない状態なら、理由をログに出して終了
	if (!CanStartFinisher())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Break] 発動できない（Break待機中ではない）"));
		return;
	}

	// 真っ先に待機フラグを倒し、タイマーを停止して二重発火を防ぐ
	bIsWaitingForFinisher = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaitTimerHandle);
	}

	// 待機を終えて、演出を開始
	StartFinisher();
}

// 待ち時間が過ぎた時（タイマーから呼ばれる）
void UPlayerBreakSequenceComponent::OnWaitTimeExpired()
{
	// 念のため待機中か確認してから自動発動する
	if (!bIsWaitingForFinisher) return;

	//真っ先に待機フラグを倒す
	bIsWaitingForFinisher = false;

	//自動で演出開始
	StartFinisher();
}

// 演出を開始する
void UPlayerBreakSequenceComponent::StartFinisher()
{
	// Break対象の敵を取得する
	AActor* BreakTarget = CurrentBreakTarget.Get();

	// タイマーが残っていれば止める（ボタンが先に押された場合など）
	GetWorld()->GetTimerManager().ClearTimer(WaitTimerHandle);

	// 待機を終える
	bIsWaitingForFinisher = false;

	// 待機中に敵が倒された・消えた場合は、フィニッシュせず通常状態へ戻る
	if (!IsValid(BreakTarget))
	{
		// BP側へ通知する（ボタンアイコンUIを消す）
		OnBreakWaitEnded.Broadcast(nullptr);

		SetGameplayInputLocked(false);

		// 状態をリセットする
		bIsBreakSequenceActive = false;
		CurrentBreakTarget.Reset();
		return;
	}

	// BP側へ合図（ボタンアイコンUIを消す）
	OnBreakWaitEnded.Broadcast(BreakTarget);

	// 演出中は入力を受け付けない
	SetGameplayInputLocked(true);

	// BP側へ合図（モンタージュ再生・カメラワークの開始）
	OnBreakSequenceStarted.Broadcast(BreakTarget);

	// モンタージュが他の入力で中断された場合、BPのOnCompletedは呼ばれず
	// EndBreakSequenceが実行されないことがある。そうすると通常攻撃側の
	// IsBreakSequenceActive ガードが永久にtrueのままになるため、必ず解除する。
	if (FinisherFallbackDuration > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			FinisherFallbackTimerHandle,
			this,
			&UPlayerBreakSequenceComponent::EndBreakSequence,
			FinisherFallbackDuration,
			false);
	}
}

// 確定ヒットのダメージを敵へ与える（モンタージュのNotifyから呼ぶ）
void UPlayerBreakSequenceComponent::ApplyForcedBreakDamage(float ForcedDamage)
{
	// 演出中でない（待機中を含む）、またはダメージが0以下なら何もしない
	if (!bIsBreakSequenceActive || bIsWaitingForFinisher || ForcedDamage <= 0.0f)
	{
		return;
	}

	// Break対象の敵が有効な場合のみ処理する
	if (AActor* BreakTarget = CurrentBreakTarget.Get())
	{
		//敵のステータスコンポネントを取得
		if (UEnemyStatus* EnemyStatus = BreakTarget->FindComponentByClass<UEnemyStatus>())
		{
			//確定ヒットダメージを渡す
			EnemyStatus->ApplyForcedBreakDamage(ForcedDamage);
		}
	}
}

// 演出の終了処理
void UPlayerBreakSequenceComponent::EndBreakSequence()
{
	// 演出中でなければ何もしない。待機中に呼ばれても無視する
	if (!bIsBreakSequenceActive || bIsWaitingForFinisher)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(WaitTimerHandle);
		World->GetTimerManager().ClearTimer(FinisherFallbackTimerHandle);
	}

	// 通知に渡すため、終了する対象の敵を先に取り出しておく
	AActor* FinishedTarget = CurrentBreakTarget.Get();

	// 敵のBreak状態を解除する（※解除のタイミングは仮。後で変更してもOK）
	if (IsValid(FinishedTarget))
	{

		if (UEnemyStatus* EnemyStatus = FinishedTarget->FindComponentByClass<UEnemyStatus>())
		{
			EnemyStatus->ExitBreakState();
		}
	}
	// リセット前の状態をログに出す
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Before Reset: bIsBreakSequenceActive = TRUE"));

	// 状態をリセット通常状態に戻る
	bIsBreakSequenceActive = false;
	CurrentBreakTarget.Reset();

	// リセット直後の状態をログに出す（本当にfalseになったか確認）
	if (GEngine)
	{
		FString ResultMsg = FString::Printf(TEXT("After Reset: bIsBreakSequenceActive = %s"), bIsBreakSequenceActive ? TEXT("TRUE (Bug!)") : TEXT("FALSE (OK)"));
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, ResultMsg);
	}

	//入力をっくを解除
	SetGameplayInputLocked(false);

	// BP側へ通知（見た目の後処理）
	OnBreakSequenceEnded.Broadcast(FinishedTarget);
}

// 移動・視点の入力を無効化 / 有効化する
void UPlayerBreakSequenceComponent::SetGameplayInputLocked(bool bLocked)
{
	// 既に同じ状態なら何もしない
	if (bLocked == bHasLockedGameplayInput)
	{
		return;
	}

	// オーナー(プレイヤーキャラ) → コントローラー の順にたどる
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	APlayerController* PlayerController = OwnerPawn ? Cast<APlayerController>(OwnerPawn->GetController()) : nullptr;
	
	// コントローラーが取得できなければ何もしない
	if (!PlayerController)
	{
		return;
	}

	// 移動入力と視点入力を無視する / 戻す
	PlayerController->SetIgnoreMoveInput(bLocked);
	PlayerController->SetIgnoreLookInput(bLocked);

	// 現在のロック状態を記録する
	bHasLockedGameplayInput = bLocked;
}

// ボタン待ちの残り時間（秒）を返す
float UPlayerBreakSequenceComponent::GetWaitRemainingTime() const
{
	// 待機中でなければ0
	if (!bIsWaitingForFinisher)
	{
		return 0.0f;
	}

	// ワールドが取得できなければ0
	const UWorld* World = GetWorld();
	if (!World)
	{
		return 0.0f;
	}

	// タイマーの残り時間を取得する（タイマーが動いていないときは負の値が返るので、0以上に直す）
	const float Remaining = World->GetTimerManager().GetTimerRemaining(WaitTimerHandle);
	return FMath::Max(Remaining, 0.0f);
}

// ボタン待ちの残り割合を返す（1.0 → 0.0）
float UPlayerBreakSequenceComponent::GetWaitRemainingPercent() const
{
	// 待機中でなければ0
	if (!bIsWaitingForFinisher)
	{
		return 0.0f;
	}

	// 無限待ち（負の値）は時間で減らないので、常に満タンで返す
	if (BreakWaitTime < 0.0f)
	{
		return 1.0f;
	}

	// 待ち時間0（即発動）は残りなし。0除算の防止にもなる
	if (FinisherWaitDuration <= 0.0f)
	{
		return 0.0f;
	}

	// 残り時間 ÷ 全体の待ち時間 = 残り割合（0〜1に収める）
	return FMath::Clamp(GetWaitRemainingTime() / FinisherWaitDuration, 0.0f, 1.0f);
}
