# Break finisher investigation (2026-10-19)

## Confirmed cause and asset fix

`BPC_AttackCombo` sets its attack flags before playing the normal attack montage.
Its Play Montage node had no Completed/Interrupted cleanup. Interrupting it with
the finisher before its animation notifications ran left normal attacks blocked.

The saved graph now routes:

- OnCompleted -> ClearCombo.
- OnInterrupted -> Branch(IsBreakSequenceActive) -> ClearCombo on True.

The branch is important: chaining one normal attack montage into the next must
not clear the new attack's combo state.

## Actual editor tests

UE 5.8.3, TestLevel, PIE, Enhanced Input action injection:

- Before fix (October 18): normal attack -> immediate finisher -> normal attack
  input after finisher: no montage played.
- After fix, including fresh editor launch (October 19): the same input sequence
  plays SwordAndShieldSlash_UE_Anim_Montage again after the finisher.
- Conditional cleanup retested at 03:13 UTC: recovery succeeds; repeated input
  progresses through SwordAndShieldSlash_UE_Anim_Montage, Attack_01, and
  SwordAndShieldSlash_2__UE_Anim_Montage, twice.

These prove animation/input recovery, not enemy damage correctness.

Fresh PIE timing test at 03:15 UTC also passed: UI+1s input ignored,
UI+3s input starts the finisher before timeout, and normal attack plays afterward.

## Open issues / precautions

- BP_Player emits Accessed None for WeponActor in weapon visibility/collision
  calls. A Sowrd actor exists in PIE, but the reference reads None. Not yet fixed.
- Native code currently adds FinisherInputDelay (2.5 seconds) to BreakWaitTime
  (BP override 2 seconds), producing a 4.5-second total window. This conflicts
  with preserving the original total timeout and requires a user timing choice.
- Python property reads have disagreed with native runtime logs for several
  Blueprint instances. TestFinisherInterruption deliberately sets delay to zero;
  discard that PIE session afterward. Do not use its property-based restoration
  as proof that timing defaults were restored.
- StartFinisherTest currently starts a fresh PIE and runs TestFinisherGate.
  TestFinisherInterruption and TestNormalCombo can be run separately in PIE.
- Editor launched with temporary -DisablePlugins=StylusInput to avoid the earlier
  73-percent startup hang. No project plugin setting was changed for this.

Pre-edit asset backups: Saved/Backups/FinisherRecovery_20261018_2051/.
Detailed results: Saved/Logs/ActionGame.log.
