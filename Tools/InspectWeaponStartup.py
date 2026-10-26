import unreal

w = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
p = unreal.GameplayStatics.get_player_character(w, 0)
unreal.log_warning('WEAPON_STARTUP|player=%s|weapon=%s' % (p, p.get_editor_property('WeponActor')))
cls = unreal.load_class(None, '/Game/Wepon/Sowrd.Sowrd_C')
unreal.log_warning('WEAPON_STARTUP|actors=%s' % unreal.GameplayStatics.get_all_actors_of_class(w, cls))
bp = unreal.load_asset('/Game/ThirdPerson/Blueprints/BP_Player')
graphs = list(unreal.BlueprintEditorLibrary.list_graphs(bp))
for node in unreal.ObjectIterator(unreal.K2Node):
    if node.get_outer() in graphs and node.get_name() in ['K2Node_Event_0','K2Node_ExecutionSequence_2','K2Node_SpawnActorFromClass_0']:
        try:
            enabled = node.get_editor_property('EnabledState')
        except Exception as exc:
            enabled = str(exc)
        unreal.log_warning('WEAPON_NODE|%s|%s|%s' % (node.get_name(), unreal.BlueprintEditorLibrary.get_node_title(node), enabled))
