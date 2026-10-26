"""PIE regression: normal completion, combo interruption, stop while attacking."""
import unreal
import time
level = unreal.get_editor_subsystem(unreal.LevelEditorSubsystem)
editor = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
assert editor.get_game_world() is None, 'End the current PIE session before this test.'
state = {'start': time.monotonic(), 'phase': 'starting', 'next_input': 0.0, 'last_montage': '', 'montages': set(), 'inputs': 0}
action = unreal.load_asset('/Game/Input/Actions/IA_Attack')

def tick(dt):
    try:
        elapsed = time.monotonic() - state['start']
        if elapsed > 35:
            raise RuntimeError('PIE test timed out')
        world = editor.get_game_world()
        if state['phase'] == 'stopping':
            if world is None:
                unreal.unregister_slate_post_tick_callback(state['handle'])
                unreal.log_warning('ATTACK_COMBO_TEST|PASS|inputs=%d|montages=%s|stopped_during_attack=True' % (state['inputs'], sorted(state['montages'])))
            return
        if not world:
            return
        player = unreal.GameplayStatics.get_player_character(world, 0)
        if not player:
            return
        if state['phase'] == 'starting':
            state['phase'] = 'testing'
            state['play_start'] = time.monotonic()
            unreal.log_warning('ATTACK_COMBO_TEST|begin')
        t = time.monotonic() - state['play_start']
        anim = player.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
        montage = anim.get_current_active_montage()
        name = montage.get_name() if montage else 'None'
        if name != state['last_montage']:
            unreal.log_warning('ATTACK_COMBO_TEST|t=%.3f|montage=%s' % (t, name))
            state['last_montage'] = name
            if montage:
                state['montages'].add(name)
        # Let the first attack complete; then repeat input to chain combo stages.
        should_input = (state['inputs'] == 0 and t > 1.0) or (t > 4.0 and t > state['next_input'])
        if should_input:
            subs = [s for s in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem) if not s.get_name().startswith('Default__')]
            assert len(subs) == 1, subs
            subs[0].inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
            state['inputs'] += 1
            state['next_input'] = t + 0.25
        if t > 8.0 and montage:
            assert len(state['montages']) >= 2, 'Combo did not advance: %s' % state['montages']
            unreal.log_warning('ATTACK_COMBO_TEST|request_stop|active_montage=%s' % name)
            state['phase'] = 'stopping'
            level.editor_request_end_play()
    except Exception as exc:
        unreal.unregister_slate_post_tick_callback(state['handle'])
        unreal.log_error('ATTACK_COMBO_TEST|FAIL|%s' % exc)
        level.editor_request_end_play()
        raise

unreal.log_warning('ATTACK_COMBO_TEST|request_begin')
state['handle'] = unreal.register_slate_post_tick_callback(tick)
level.editor_request_begin_play()
