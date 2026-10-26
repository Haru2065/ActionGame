import unreal

B = unreal.BlueprintEditorLibrary
P = unreal.BlueprintGraphPinLibrary
bp = unreal.load_asset('/Game/Animation/NormalAttackComboSystem/BPC_AttackCombo')
graph = B.find_event_graph(bp)
nodes = [n for n in unreal.ObjectIterator(unreal.K2Node) if n.get_outer() == graph]
montage = next(n for n in nodes if n.get_name() == 'K2Node_PlayMontage_1')
cleanup = [n for n in nodes if isinstance(n, unreal.K2Node_CallFunction) and B.get_node_title(n).replace(' ', '') == 'ClearCombo']
assert len(cleanup) == 1, cleanup
cleanup = cleanup[0]
branch = next(n for n in nodes if isinstance(n, unreal.K2Node_IfThenElse) and not P.list_connected_pins(B.find_execute_pin(n)))
active = next(n for n in nodes if n.get_name() == 'K2Node_CallFunction_5')
assert P.try_create_connection(B.find_output_pin(montage, 'OnCompleted'), B.find_execute_pin(cleanup))
assert P.try_create_connection(B.find_output_pin(montage, 'OnInterrupted'), B.find_execute_pin(branch))
assert P.try_create_connection(B.find_output_pin(active, 'ReturnValue'), B.find_input_pin(branch, 'Condition'))
assert P.try_create_connection(B.find_then_pin(branch), B.find_execute_pin(cleanup))
assert [P.get_owning_node(p) for p in P.list_connected_pins(B.find_output_pin(montage, 'OnInterrupted'))] == [branch]
pos = B.get_node_pos(montage)
B.set_node_pos(branch, unreal.IntPoint(pos.x + 640, pos.y + 240))
B.set_node_pos(cleanup, unreal.IntPoint(pos.x + 1000, pos.y + 160))
B.compile_blueprint(bp)
unreal.log_warning('ATTACK_RECOVERY|compile=%s' % bp.get_editor_property('status'))
assert 'ERROR' not in str(bp.get_editor_property('status')).upper()
assert unreal.EditorAssetLibrary.save_loaded_asset(bp)
unreal.log_warning('ATTACK_RECOVERY|saved|Completed -> ClearCombo; Interrupted -> IsBreakSequenceActive -> ClearCombo')
