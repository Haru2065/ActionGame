#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Status/PlayerBreakSequenceComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFinisherInputTimingTest,
	"ActionGame.Break.FinisherInputTiming",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFinisherInputTimingTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	AActor* Owner = World->SpawnActor<AActor>();
	AActor* Target = World->SpawnActor<AActor>();
	UPlayerBreakSequenceComponent* Sequence = NewObject<UPlayerBreakSequenceComponent>(Owner);
	Sequence->RegisterComponent();
	Sequence->FinisherInputDelay = 2.5f;
	// BP_Player overrides the native 3-second default to 2 seconds.
	Sequence->BreakWaitTime = 2.0f;
	Sequence->FinisherFallbackDuration = 0.0f;

	auto BeginWait = [&]()
	{
		struct { AActor* BrokenEnemyActor; } Args{Target};
		Sequence->ProcessEvent(Sequence->FindFunctionChecked(TEXT("HandleBreakSequenceStart")), &Args);
	};
	auto Advance = [&](int32 Steps)
	{
		for (int32 Index = 0; Index < Steps; ++Index)
		{
			// TimerManager only advances once per engine frame, including synthetic ticks.
			++GFrameCounter;
			World->Tick(LEVELTICK_All, 0.1f);
		}
	};

	BeginWait();
	TestTrue(TEXT("UI starts with a full countdown"), Sequence->GetWaitRemainingPercent() > 0.99f);
	TestTrue(TEXT("Input is accepted immediately even with a legacy delay override"), Sequence->CanStartFinisher());
	Sequence->TryStartFinisher();
	TestTrue(TEXT("An immediate press starts the finisher"), Sequence->IsFinisherPlaying());
	Sequence->EndBreakSequence();
	TestFalse(TEXT("Finisher completion clears the normal attack break guard"), Sequence->IsBreakSequenceActive());

	BeginWait();
	Advance(10);
	Sequence->TryStartFinisher();
	TestTrue(TEXT("A press after one second starts the finisher"), Sequence->IsFinisherPlaying());
	Sequence->EndBreakSequence();
	BeginWait();
	Advance(19);
	TestTrue(TEXT("Input remains available before two seconds"), Sequence->CanStartFinisher());
	Advance(2);
	TestTrue(TEXT("Timeout starts the finisher at the original two-second deadline"), Sequence->IsFinisherPlaying());
	Sequence->EndBreakSequence();

	Sequence->BreakWaitTime = -1.0f;
	BeginWait();
	TestTrue(TEXT("Infinite wait accepts input immediately"), Sequence->CanStartFinisher());
	Sequence->TryStartFinisher();
	Sequence->EndBreakSequence();

	World->DestroyWorld(false);
	GEngine->DestroyWorldContext(World);
	return true;
}

#endif
