import unreal
w = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_editor_world()
cls = unreal.load_class(None, '/Game/ThirdPerson/Blueprints/BP_Player.BP_Player_C')
p = unreal.EditorLevelLibrary.spawn_actor_from_class(cls, unreal.Vector(0,0,10000))
for c in p.get_components_by_class(unreal.SceneComponent):
    if isinstance(c, (unreal.CameraComponent, unreal.SpringArmComponent)):
        d = dict(name=c.get_name(),parent=str(c.get_attach_parent()),socket=str(c.get_attach_socket_name()))
        for k in ['relative_location','relative_rotation','absolute_location','absolute_rotation','use_pawn_control_rotation','target_arm_length','socket_offset','target_offset','auto_activate','component_tick']:
            try: d[k]=str(c.get_editor_property(k))
            except: pass
        unreal.log_warning('CAMERA_DIAG '+str(d))
unreal.EditorLevelLibrary.destroy_actor(p)
