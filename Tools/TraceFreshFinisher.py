import unreal

def start_trace():
    w = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
    p = unreal.GameplayStatics.get_player_character(w, 0)
    comps = list(p.get_components_by_class(unreal.PlayerBreakSequenceComponent))
    assert len(comps) == 1, 'Restart PIE to remove diagnostic components'
    seq = comps[0]
    seq.set_editor_property('BreakWaitTime', 2.0)
    target = [a for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Actor) if a.get_component_by_class(unreal.EnemyStatus)][0]
    stats = target.get_component_by_class(unreal.EnemyStatus)
    info = {'elapsed': 0.0, 'next': 0.0, 'triggered': False}
    def tick(dt):
        info['elapsed'] += dt
        if not info['triggered'] and info['elapsed'] >= 20.0:
            stats.add_break_point(1000.0)
            info['triggered'] = True
        if info['elapsed'] >= info['next']:
            info['next'] += 0.5
            unreal.log_warning('FRESH_TRACE|t=%.2f|active=%s|waiting=%s|can=%s|remaining=%.3f|dilation=%.3f|weapon=%s' % (
                info['elapsed'], seq.is_break_sequence_active(), seq.is_waiting_for_finisher(), seq.can_start_finisher(),
                seq.get_wait_remaining_time(), unreal.GameplayStatics.get_global_time_dilation(w), p.get_editor_property('WeponActor')))
        if info['elapsed'] > 32.0:
            unreal.unregister_slate_post_tick_callback(info['handle'])
    info['handle'] = unreal.register_slate_post_tick_callback(tick)
start_trace()
