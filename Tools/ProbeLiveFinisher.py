import unreal

world = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
player = unreal.GameplayStatics.get_player_character(world, 0)
sequence = player.get_component_by_class(unreal.PlayerBreakSequenceComponent)
sequence.set_editor_property('BreakWaitTime', -1.0)
targets = [a for a in unreal.GameplayStatics.get_all_actors_of_class(world, unreal.Actor)
           if a.get_component_by_class(unreal.EnemyStatus)]
unreal.log_warning('LIVE_FINISHER|player=%s|targets=%s|waiting=%s|can=%s' % (
    player.get_name(), [a.get_name() for a in targets], sequence.is_waiting_for_finisher(), sequence.can_start_finisher()))
if not sequence.is_break_sequence_active():
    targets[0].get_component_by_class(unreal.EnemyStatus).add_break_point(1000.0)
unreal.log_warning('LIVE_FINISHER|after_break|active=%s|waiting=%s|can=%s' % (
    sequence.is_break_sequence_active(), sequence.is_waiting_for_finisher(), sequence.can_start_finisher()))
unreal.log_warning('LIVE_FINISHER|weapon=%s' % player.get_editor_property('WeponActor'))
