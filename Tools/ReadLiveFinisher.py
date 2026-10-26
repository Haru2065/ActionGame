import unreal

w = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
p = unreal.GameplayStatics.get_player_character(w, 0)
for c in p.get_components_by_class(unreal.PlayerBreakSequenceComponent):
    unreal.log_warning('LIVE_STATE|%s|active=%s|waiting=%s|playing=%s|can=%s|remaining=%s|delay=%s|wait=%s|target=%s' % (
        c.get_path_name(), c.is_break_sequence_active(), c.is_waiting_for_finisher(), c.is_finisher_playing(), c.can_start_finisher(),
        c.get_wait_remaining_time(), c.get_editor_property('FinisherInputDelay'), c.get_editor_property('BreakWaitTime'), c.get_current_break_target()))
    unreal.log_warning('LIVE_PERCENT|%s' % c.get_wait_remaining_percent())
unreal.log_warning('LIVE_WEAPON|%s' % p.get_editor_property('WeponActor'))
target = [a for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Actor) if a.get_component_by_class(unreal.EnemyStatus)][0]
probe = unreal.PlayerBreakSequenceComponent(outer=p)
probe.set_editor_property('BreakWaitTime', -1.0)
probe.call_method('HandleBreakSequenceStart', args=(target,))
unreal.log_warning('LIVE_NATIVE_PROBE|active=%s|waiting=%s|percent=%s|target=%s' % (
    probe.is_break_sequence_active(), probe.is_waiting_for_finisher(), probe.get_wait_remaining_percent(), probe.get_current_break_target()))
probe.end_break_sequence()
cls = unreal.load_class(None, '/Game/BreakSystem/BP_PlayerBreakSequence.BP_PlayerBreakSequence_C')
derived = unreal.new_object(cls, outer=p)
derived.set_editor_property('BreakWaitTime', -1.0)
derived.call_method('HandleBreakSequenceStart', args=(target,))
unreal.log_warning('LIVE_DERIVED_PROBE|active=%s|waiting=%s|percent=%s|target=%s' % (
    derived.is_break_sequence_active(), derived.is_waiting_for_finisher(), derived.get_wait_remaining_percent(), derived.get_current_break_target()))
