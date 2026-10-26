"""Guard montage callbacks against owner destruction during PIE/world teardown."""
import unreal
B = unreal.BlueprintEditorLibrary
P = unreal.BlueprintGraphPinLibrary
bp = unreal.load_asset('/Game/Animation/NormalAttackComboSystem/BPC_AttackCombo')
e = unreal.BlueprintGraphEditor.get_graph_editor(B.find_event_graph(bp))
nodes = {n.get_name(): n for n in e.list_all_nodes()}
owner = nodes['K2Node_CallFunction_7']
component = nodes['K2Node_CallFunction_4']
player = nodes['K2Node_CallFunction_13']
montage = nodes['K2Node_PlayMontage_1']
interrupted = nodes['K2Node_IfThenElse_0']
input_print = nodes['K2Node_CallFunction_0']
attack_branch = nodes['K2Node_IfThenElse_3']
assert [P.get_owning_node(p) for p in P.list_connected_pins(B.find_input_pin(component, 'self'))] == [player]
assert [P.get_owning_node(p) for p in P.list_connected_pins(B.find_output_pin(montage, 'OnInterrupted'))] == [interrupted]
assert [P.get_owning_node(p) for p in P.list_connected_pins(B.find_then_pin(input_print))] == [attack_branch]

def link(a, b):
    assert P.try_create_connection(a, b), (P.get_pin_name(a), P.get_pin_name(b))

def result(n):
    return B.find_output_pin(n, 'ReturnValue')

with unreal.ScopedEditorTransaction('Fix AttackCombo references during montage interruption'):
    P.break_pin_links(B.find_input_pin(component, 'self'))
    link(result(owner), B.find_input_pin(component, 'self'))
    e.remove_nodes([player])
    owner_valid = e.add_call_function_node('/Script/Engine.KismetSystemLibrary.IsValid')
    component_valid = e.add_call_function_node('/Script/Engine.KismetSystemLibrary.IsValid')
    assert owner_valid and component_valid
    link(result(owner), B.find_input_pin(owner_valid, 'Object'))
    link(result(component), B.find_input_pin(component_valid, 'Object'))
    B.set_node_pos(owner_valid, unreal.IntPoint(1500, 1800))
    B.set_node_pos(component_valid, unreal.IntPoint(1800, 1800))
    for source, destination, y in [
        (B.find_then_pin(input_print), attack_branch, 1200),
        (B.find_output_pin(montage, 'OnInterrupted'), interrupted, 1500),
    ]:
        guard_owner = e.add_branch_node()
        guard_component = e.add_branch_node()
        assert guard_owner and guard_component
        B.set_node_pos(guard_owner, unreal.IntPoint(1500, y))
        B.set_node_pos(guard_component, unreal.IntPoint(1800, y))
        P.break_pin_links(source)
        link(source, B.find_execute_pin(guard_owner))
        link(result(owner_valid), B.find_condition_pin(guard_owner))
        link(B.find_then_pin(guard_owner), B.find_execute_pin(guard_component))
        link(result(component_valid), B.find_condition_pin(guard_component))
        link(B.find_then_pin(guard_component), B.find_execute_pin(destination))
        e.add_comment_to_nodes(
            'Validate owner, then break sequence component. Ignore callbacks after destruction.'
            if y == 1500 else 'Validate owner and break sequence component before reading attack state.',
            [guard_owner, guard_component], 40)
    e.add_comment_to_nodes('Use the actor that owns this combo component; validate references before use.', [owner_valid, component_valid], 40)
    B.compile_blueprint(bp)
    assert 'ERROR' not in str(bp.get_editor_property('status')).upper(), bp.get_editor_property('status')
    assert unreal.EditorAssetLibrary.save_loaded_asset(bp)
    B.refresh_open_editors_for_blueprint(bp)
unreal.log_warning('ATTACK_COMBO_FIX|saved|status=%s' % bp.get_editor_property('status'))
