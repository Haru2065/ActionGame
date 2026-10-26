import unreal, json
s={'time':0.,'stage':0,'results':{}}
def tick(dt):
 s['time']+=dt
 try:
  if s['stage']==0 and s['time']>1:
   unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
   s['stage']=1
  w=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
  if s['stage']==1 and s['time']>4 and w:
   p=unreal.GameplayStatics.get_player_character(w,0)
   if not p:return
   b=next(c for c in p.get_components_by_class(unreal.SpringArmComponent) if c.get_name()=='CameraBoom')
   c=p.get_component_by_class(unreal.CameraComponent)
   pc=unreal.GameplayStatics.get_player_controller(w,0)
   assert b.get_attach_parent()==p.get_component_by_class(unreal.CapsuleComponent)
   assert c.get_attach_parent()==b and str(c.get_attach_socket_name())=='SpringEndpoint'
   assert pc.get_view_target()==p
   s.update(p=p,b=b,c=c,pc=pc,oldp=p.get_actor_location(),oldc=c.get_world_location(),oldrot=c.get_world_rotation())
   p.set_actor_location(s['oldp']+unreal.Vector(0,150,0),False,True)
   s['results']['attachment']='PASS'
   s['stage']=2
  elif s['stage']==2 and s['time']>5:
   dp=s['p'].get_actor_location()-s['oldp']; dc=s['c'].get_world_location()-s['oldc']
   assert abs(dp.y-150)<1 and abs(dc.y-dp.y)<1, str((dp,dc))
   s['results']['movement_follow']='PASS'
   s['results']['player_delta']=str(dp); s['results']['camera_delta']=str(dc)
   r=s['pc'].get_control_rotation(); r.yaw+=30; s['pc'].set_control_rotation(r)
   s['stage']=3
  elif s['stage']==3 and s['time']>6:
   r=s['c'].get_world_rotation()
   assert abs(r.yaw-s['oldrot'].yaw)>25
   s['results']['rotation_follow']='PASS'
   unreal.log_warning('CAMERA_TEST '+json.dumps(s['results']))
   open('E:/UnrealProject/AcrionGame 5.8/Saved/CameraRepair/verification.json','w').write(json.dumps(s['results'],indent=2))
   unreal.unregister_slate_post_tick_callback(s['handle'])
   unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
  if s['time']>25:raise RuntimeError('PIE test timeout')
 except Exception as e:
  unreal.log_error('CAMERA_TEST FAIL '+str(e))
  unreal.unregister_slate_post_tick_callback(s['handle'])
  unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
s['handle']=unreal.register_slate_post_tick_callback(tick)
