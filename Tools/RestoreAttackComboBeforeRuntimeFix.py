import unreal
import shutil
from pathlib import Path
root = Path('E:/UnrealProject/AcrionGame 5.8')
asset_path = '/Game/Animation/NormalAttackComboSystem/BPC_AttackCombo'
bp = unreal.load_asset(asset_path)
assert bp
package = bp.get_outermost()
unreal.EditorLoadingAndSavingUtils.fully_load_packages([package])
sub = unreal.get_editor_subsystem(unreal.AssetEditorSubsystem)
sub.close_all_editors_for_asset(bp)
source = root / 'Saved/Backups/AttackComboRuntime_20261026/BPC_AttackCombo.uasset'
target = root / 'Content/Animation/NormalAttackComboSystem/BPC_AttackCombo.uasset'
assert source.is_file()
shutil.copy2(target, root / 'Saved/Backups/AttackComboRuntime_20261026/BPC_AttackCombo_after_fix.uasset')
shutil.copyfile(source, target)
result = unreal.EditorLoadingAndSavingUtils.reload_packages([package], unreal.ReloadPackagesInteractionMode.ASSUME_POSITIVE)
unreal.log_warning('ATTACK_COMBO_RESTORE|reload=%s' % str(result))
restored = unreal.load_asset(asset_path)
sub.open_editor_for_assets([restored])
assert source.read_bytes() == target.read_bytes(), 'Restored file differs from backup'
unreal.log_warning('ATTACK_COMBO_RESTORE|restored_from_backup')
