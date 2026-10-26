import unreal
cls = unreal.load_class(None, '/Game/ThirdPerson/Blueprints/BP_Player.BP_Player_C')
for cl in [cls,unreal.ActionGameCharacter]:
 p=unreal.get_default_object(cl)
 unreal.log_warning('CAM_CDO '+p.get_path_name())
 for c in p.get_components_by_class(unreal.SceneComponent):
  unreal.log_warning('CAM_CDO_COMP '+c.get_name()+' parent='+str(c.get_attach_parent())+' socket='+str(c.get_attach_socket_name()))
bp=unreal.load_asset('/Game/ThirdPerson/Blueprints/BP_Player')
s=unreal.get_engine_subsystem(unreal.SubobjectDataSubsystem)
for h in s.k2_gather_subobject_data_for_blueprint(bp):
 d=unreal.SubobjectDataBlueprintFunctionLibrary.get_data(h)
 o=unreal.SubobjectDataBlueprintFunctionLibrary.get_object(d)
 unreal.log_warning('CAM_TEMPLATE '+str(o))
 if isinstance(o,unreal.SpringArmComponent): unreal.log_warning('CAM_TEMPLATE_PARENT '+str(o.get_attach_parent()))
