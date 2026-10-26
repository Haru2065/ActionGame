import unreal
import gc
import types

# Remove an earlier diagnostic callback even if a failed assertion aborted it.
for fn in gc.get_objects():
    if isinstance(fn, types.FunctionType) and fn.__code__.co_filename == __file__ and fn.__name__ == 'tick':
        for cell in fn.__closure__ or ():
            value = cell.cell_contents
            if isinstance(value, dict) and 'handle' in value:
                unreal.unregister_slate_post_tick_callback(value['handle'])

def run():
    w = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
    p = unreal.GameplayStatics.get_player_character(w, 0)
    pc = unreal.GameplayStatics.get_player_controller(w, 0)
    subs = [s for s in unreal.ObjectIterator(unreal.EnhancedInputLocalPlayerSubsystem) if not s.get_name().startswith('Default__')]
    assert len(subs) == 1, subs
    sub = subs[0]
    seq = p.get_component_by_class(unreal.PlayerBreakSequenceComponent)
    combo = next(c for c in p.get_components_by_class(unreal.ActorComponent) if c.get_class().get_name() == 'BPC_AttackCombo_C')
    enemy = next(a for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Actor) if a.get_component_by_class(unreal.EnemyStatus))
    status = enemy.get_component_by_class(unreal.EnemyStatus)
    attack = unreal.load_asset('/Game/Input/Actions/IA_Attack')
    finish = unreal.load_asset('/Game/Input/Break/keyboardMousee/IA_BreakAttack')
    old_delay = seq.get_editor_property('FinisherInputDelay')
    old_wait = seq.get_editor_property('BreakWaitTime')
    seq.set_editor_property('FinisherInputDelay', 0.0)
    seq.set_editor_property('BreakWaitTime', 10.0)
    combo.call_method('ClearCombo')
    status.exit_break_state()
    s = {'time': 0.0, 'step': 0}
    def report(label):
        unreal.log_warning('INTERRUPTION_TEST|%s|canattack=%s|attacking=%s|montage=%s' % (label, combo.get_editor_property('CanAttack?'), combo.get_editor_property('IsAtatcking?'), p.get_component_by_class(unreal.SkeletalMeshComponent).get_anim_instance().get_current_active_montage()))
    def tick(dt):
        s['time'] += dt
        if s['step'] == 0 and s['time'] > 1.0:
            report('before attack')
            sub.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0), [], [])
            s['step'] = 1
        elif s['step'] == 1 and s['time'] > 1.06:
            report('attack started')
            status.add_break_point(1000)
            sub.inject_input_vector_for_action(finish, unreal.Vector(1, 0, 0), [], [])
            s['step'] = 2
        elif s['step'] == 2 and s['time'] > 1.15:
            report('finisher started')
            s['step'] = 3
        elif s['step'] == 3 and s['time'] > 8.0:
            report('finisher ended')
            sub.inject_input_vector_for_action(attack, unreal.Vector(1, 0, 0), [], [])
            s['step'] = 4
        elif s['step'] == 4 and s['time'] > 8.1:
            report('normal attack retry')
            seq.set_editor_property('FinisherInputDelay', old_delay)
            seq.set_editor_property('BreakWaitTime', old_wait)
            unreal.unregister_slate_post_tick_callback(s['handle'])
            import runpy
            runpy.run_path('E:/UnrealProject/AcrionGame 5.8/Tools/TestNormalCombo.py')
    s['handle'] = unreal.register_slate_post_tick_callback(tick)

run()
