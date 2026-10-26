import unreal
B=unreal.BlueprintEditorLibrary;P=unreal.BlueprintGraphPinLibrary
bp=unreal.load_asset('/Game/ThirdPerson/Blueprints/BP_Player');g=B.find_event_graph(bp)
nodes=[n for n in unreal.ObjectIterator(unreal.K2Node) if n.get_outer()==g]
for name in ['K2Node_CallFunction_58','K2Node_CallFunction_60']:
 n=next(n for n in nodes if n.get_name()==name)
 chain=[n]+[P.get_owning_node(p) for p in P.list_connected_pins(B.find_then_pin(n))]
 for a in chain:
  unreal.log_warning('CINE_DETAIL '+a.get_name()+' '+B.get_node_title(a))
  for pin in B.list_input_pins(a):
   unreal.log_warning('CINE_PIN '+str(P.get_pin_name(pin))+'='+str(P.get_pin_value(pin)))
unreal.log_warning('CINE_DOC '+str(P.set_pin_value.__doc__))
