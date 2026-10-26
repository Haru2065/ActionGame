import unreal
bp = unreal.load_asset('/Game/Animation/NormalAttackComboSystem/BPC_AttackCombo')
unreal.get_editor_subsystem(unreal.AssetEditorSubsystem).open_editor_for_assets([bp])
