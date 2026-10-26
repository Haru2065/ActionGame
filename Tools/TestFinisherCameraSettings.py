import unreal,json
s={'t':0.,'stage':0,'checks':[]}
def tick(dt):
 s['t']+=dt
 try:
  if s['stage']==0 and s['t']>1:
   unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play();s['stage']=1
  w=unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
  if s['stage']==1 and s['t']>4 and w:
   p=unreal.GameplayStatics.get_player_character(w,0)
   if not p:return
   c=p.get_component_by_class(unreal.CameraComponent)
   s.update(p=p,c=c,old=c.get_world_location())
   p.call_method('Finisher_StartCmeraAction')
   assert (c.get_world_location()-s['old']).length()<0.1
   s['checks'].append('Entry preserves world position')
   s['stage']=2
  elif s['stage']==2 and s['t']>6:
   c=s['c'];assert c.get_attach_parent().get_name()=='FinisherArm'
   assert str(c.get_attach_socket_name())=='SpringEndpoint'
   assert c.get_editor_property('relative_location').length()<0.1
   s['checks'].append('FinisherArm endpoint reached')
   unreal.log_warning('CINE_PREVIEW_READY '+str(c.get_world_location()))
   s['stage']=3
  elif s['stage']==3 and s['t']>22:
   old=s['c'].get_world_location();s['p'].call_method('Finisher_EndCameraAction')
   assert (s['c'].get_world_location()-old).length()<0.1
   s['checks'].append('Exit preserves world position');s['stage']=4
  elif s['stage']==4 and s['t']>24:
   c=s['c'];assert c.get_attach_parent().get_name()=='CameraBoom'
   assert str(c.get_attach_socket_name())=='SpringEndpoint'
   assert c.get_editor_property('relative_location').length()<0.1
   s['checks'].append('Normal camera endpoint restored')
   unreal.log_warning('CINE_TEST PASS '+str(s['checks']))
   open('E:/UnrealProject/AcrionGame 5.8/Saved/FinisherCameraTuning/verification.json','w').write(json.dumps(s['checks'],indent=2))
   unreal.unregister_slate_post_tick_callback(s['h'])
   unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
  if s['t']>35:raise RuntimeError('timeout')
 except Exception as e:
  unreal.log_error('CINE_TEST FAIL '+str(e));unreal.unregister_slate_post_tick_callback(s['h']);unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_end_play()
s['h']=unreal.register_slate_post_tick_callback(tick)
