"""PIE-only input probe; does not save or modify assets."""
import unreal

def run():
    w = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
    p = unreal.GameplayStatics.get_player_character(w, 0)
    assert p, 'Start PIE first'
    sub = next(s for s in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem) if not s.get_name().startswith('Default__'))
    action = unreal.load_asset('/Game/Input/Actions/IA_Attack')
    anim = p.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
    s = {'time': 0.0, 'next': 1.0, 'last': ''}
    def tick(dt):
        try:
            s['time'] += dt
            if s['time'] >= s['next'] and s['time'] < 6.0:
                sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
                s['next'] += 0.25
            montage = anim.get_current_active_montage()
            name = montage.get_name() if montage else 'None'
            if name != s['last']:
                unreal.log_warning('COMBO_TEST|t=%.3f|montage=%s' % (s['time'], name))
                s['last'] = name
            if s['time'] > 9.0:
                unreal.unregister_slate_post_tick_callback(s['handle'])
                unreal.log_warning('COMBO_TEST|finished')
        except Exception:
            unreal.unregister_slate_post_tick_callback(s['handle'])
            raise
    s['handle'] = unreal.register_slate_post_tick_callback(tick)

run()
