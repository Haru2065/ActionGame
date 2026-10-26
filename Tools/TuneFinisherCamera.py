import unreal, shutil, os, json
B=unreal.BlueprintEditorLibrary;P=unreal.BlueprintGraphPinLibrary
bp=unreal.load_asset('/Game/ThirdPerson/Blueprints/BP_Player');g=B.find_event_graph(bp)
# Preserve the user's latest unsaved graph before tuning.
B.compile_blueprint(bp)
assert unreal.EditorAssetLibrary.save_loaded_asset(bp)
os.makedirs('E:/UnrealProject/AcrionGame 5.8/Saved/FinisherCameraTuning',exist_ok=True)
shutil.copy2('E:/UnrealProject/AcrionGame 5.8/Content/ThirdPerson/Blueprints/BP_Player.uasset','E:/UnrealProject/AcrionGame 5.8/Saved/FinisherCameraTuning/BP_Player.before.uasset')
nodes=[n for n in unreal.ObjectIterator(unreal.K2Node) if n.get_outer()==g]
changes=[]
def setpin(n,k,v):
 pin=B.find_input_pin(n,k); old=P.get_pin_value(pin)
 assert P.set_pin_value(pin,v),(k,v)
 changes.append([n.get_name(),k,old,v])
with unreal.ScopedEditorTransaction('Tune finisher camera framing and transitions'):
 bp.modify()
 for eventname,duration in [('Finisher_StartCmeraAction','0.32'),('Finisher_EndCameraAction','0.55')]:
  ev=next(n for n in nodes if B.get_node_title(n)==eventname)
  at=P.get_owning_node(P.list_connected_pins(B.find_then_pin(ev))[0])
  move=P.get_owning_node(P.list_connected_pins(B.find_then_pin(at))[0])
  at.modify(); move.modify()
  for k,v in [('SocketName','SpringEndpoint'),('LocationRule','KeepWorld'),('RotationRule','KeepWorld'),('bWeldSimulatedBodies','false')]:setpin(at,k,v)
  for k,v in [('OverTime',duration),('bEaseIn','true'),('bEaseOut','true'),('bForceShortestRotationPath','true')]:setpin(move,k,v)
 ss=unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
 arm=None
 for h in ss.k2_gather_subobject_data_for_blueprint(bp):
  o=unreal.SubobjectDataBlueprintFunctionLibrary.get_object(unreal.SubobjectDataBlueprintFunctionLibrary.get_data(h))
  if isinstance(o,unreal.SpringArmComponent) and o.get_name().startswith('FinisherArm'):arm=o
 assert arm
 arm.modify()
 props={'relative_location':unreal.Vector(65,0,15),'relative_rotation':unreal.Rotator(pitch=-6,yaw=135,roll=0),'target_arm_length':260.0,'use_pawn_control_rotation':False}
 for k,v in props.items():
  changes.append(['FinisherArm',k,str(arm.get_editor_property(k)),str(v)])
  arm.set_editor_property(k,v)
 B.compile_blueprint(bp)
 assert 'ERROR' not in str(bp.get_editor_property('status')).upper()
 assert unreal.EditorAssetLibrary.save_loaded_asset(bp)
open('E:/UnrealProject/AcrionGame 5.8/Saved/FinisherCameraTuning/changes.json','w').write(json.dumps(changes,indent=2))
unreal.log_warning('CINE_TUNED '+str(changes))
