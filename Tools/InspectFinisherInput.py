import unreal

B = unreal.BlueprintEditorLibrary
P = unreal.BlueprintGraphPinLibrary
for asset in ['/Game/ThirdPerson/Blueprints/BP_Player', '/Game/Animation/NormalAttackComboSystem/BPC_AttackCombo']:
    bp = unreal.load_asset(asset)
    graphs = list(B.list_graphs(bp))
    for node in unreal.ObjectIterator(unreal.K2Node):
        if node.get_outer() not in graphs:
            continue
        unreal.log_warning('INPUT_NODE|%s|%s|%s' % (asset, node.get_name(), B.get_node_title(node)))
        for pin in B.list_all_pins(node):
            links = P.list_connected_pins(pin)
            unreal.log_warning('INPUT_PIN|%s|%s|%s|%s' % (
                node.get_name(), P.get_pin_name(pin), P.get_pin_value(pin),
                ','.join('%s:%s' % (P.get_owning_node(p).get_name(), P.get_pin_name(p)) for p in links)))

context = unreal.load_asset('/Game/Input/IMC_Default')
for mapping in context.get_editor_property('mappings'):
    unreal.log_warning('INPUT_MAPPING|%s|%s' % (mapping.get_editor_property('key'), mapping.get_editor_property('action')))
