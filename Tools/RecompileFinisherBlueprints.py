import unreal

for path in [
    '/Game/BreakSystem/BP_PlayerBreakSequence',
    '/Game/Animation/NormalAttackComboSystem/BPC_AttackCombo',
    '/Game/ThirdPerson/Blueprints/BP_Player',
    '/Game/UI/QTE/BreakUI',
]:
    bp = unreal.load_asset(path)
    unreal.BlueprintEditorLibrary.compile_blueprint(bp)
    unreal.log_warning('FINISHER_RECOMPILE|%s' % path)
