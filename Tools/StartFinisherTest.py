import unreal
import runpy

_startup = {'time': 0.0, 'requested': False}

def _start_tick(dt):
    _startup['time'] += dt
    if not _startup['requested'] and _startup['time'] > 2.0:
        unreal.get_editor_subsystem(unreal.LevelEditorSubsystem).editor_request_begin_play()
        _startup['requested'] = True
    if _startup['time'] > 5.0:
        w = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
        if w and unreal.GameplayStatics.get_player_character(w, 0):
            unreal.unregister_slate_post_tick_callback(_startup['handle'])
            runpy.run_path('E:/UnrealProject/AcrionGame 5.8/Tools/TestFinisherGate.py')
    if _startup['time'] > 30.0:
        unreal.unregister_slate_post_tick_callback(_startup['handle'])

_startup['handle'] = unreal.register_slate_post_tick_callback(_start_tick)
