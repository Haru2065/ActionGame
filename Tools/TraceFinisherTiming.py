import unreal

world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
player = unreal.GameplayStatics.get_player_character(world, 0)
sequence = player.get_component_by_class(unreal.PlayerBreakSequenceComponent)
sequence.set_editor_property('BreakWaitTime', 2.0)
sequence.end_break_sequence()
enemy = [a for a in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor) if a.get_component_by_class(unreal.EnemyStatus)][0]
status = enemy.get_component_by_class(unreal.EnemyStatus)
status.exit_break_state()
trace = {'elapsed': 0.0, 'next': 0.0, 'triggered': False}
def trace_tick(dt):
    trace['elapsed'] += dt
    if not trace['triggered'] and trace['elapsed'] >= 1.0:
        status.add_break_point(1000.0)
        trace['triggered'] = True
    if trace['elapsed'] >= trace['next']:
        trace['next'] += 0.5
        unreal.log_warning('REAL_INPUT_TRACE|t=%.2f|active=%s|waiting=%s|can=%s|remaining=%.3f|dilation=%.3f' % (
            trace['elapsed'], sequence.is_break_sequence_active(), sequence.is_waiting_for_finisher(),
            sequence.can_start_finisher(), sequence.get_wait_remaining_time(), unreal.GameplayStatics.get_global_time_dilation(world)))
    if trace['elapsed'] > 8.0:
        unreal.unregister_slate_post_tick_callback(trace['handle'])
trace['handle'] = unreal.register_slate_post_tick_callback(trace_tick)
