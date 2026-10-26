"""Run in PIE: early input, accepted input, then normal attack recovery."""
import unreal

def run():
    w = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
    p = unreal.GameplayStatics.get_player_character(w, 0)
    assert p
    sub = next(s for s in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem) if not s.get_name().startswith('Default__'))
    enemy = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Actor) if a.get_component_by_class(unreal.EnemyStatus))
    stats = enemy.get_component_by_class(unreal.EnemyStatus)
    anim = p.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance()
    finish = unreal.load_asset('/Game/Input/Break/keyboardMousee/IA_BreakAttack')
    attack = unreal.load_asset('/Game/Input/Actions/IA_Attack')
    def press(action):
        sub.inject_input_vector_for_action(action, unreal.Vector(1, 0, 0), [], [])
    def check(expected, label):
        m = anim.get_current_active_montage()
        name = m.get_name() if m else 'None'
        assert (name == expected if expected else name != 'AM_SwordRegularCombo'), (label, name)
        unreal.log_warning('FINISHER_GATE|PASS|%s|montage=%s' % (label, name))
    events = [(1.0, lambda: stats.add_break_point(1000)),
              (2.0, lambda: press(finish)),
              (2.1, lambda: check(None, 'UI+1s ignored')),
              (4.0, lambda: press(finish)),
              (4.1, lambda: check('AM_SwordRegularCombo', 'UI+3s starts before timeout')),
              (10.0, lambda: press(attack)),
              (10.1, lambda: check('SwordAndShieldSlash_UE_Anim_Montage', 'normal attack recovers'))]
    s = {'time': 0.0, 'index': 0}
    def tick(dt):
        try:
            s['time'] += dt
            if s['time'] >= events[s['index']][0]:
                events[s['index']][1]()
                s['index'] += 1
                if s['index'] == len(events):
                    unreal.unregister_slate_post_tick_callback(s['handle'])
        except Exception:
            unreal.unregister_slate_post_tick_callback(s['handle'])
            raise
    s['handle'] = unreal.register_slate_post_tick_callback(tick)

run()
