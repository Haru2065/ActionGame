import unreal
bp=unreal.load_asset('/Game/ThirdPerson/Blueprints/BP_Player')
p=unreal.get_default_object(bp.generated_class())
b=p.get_component_by_class(unreal.SpringArmComponent)
r=p.get_component_by_class(unreal.CapsuleComponent)
unreal.log_warning('CAM_REPAIR before='+str(b.get_attach_parent()))
with unreal.ScopedEditorTransaction('Restore CameraBoom attachment to player capsule'):
 b.modify()
 assert b.attach_to_component(r, 'None', unreal.AttachmentRule.KEEP_RELATIVE, unreal.AttachmentRule.KEEP_RELATIVE, unreal.AttachmentRule.KEEP_RELATIVE, False)
 unreal.BlueprintEditorLibrary.compile_blueprint(bp)
p=unreal.get_default_object(bp.generated_class())
b=p.get_component_by_class(unreal.SpringArmComponent)
assert b.get_attach_parent()==p.get_component_by_class(unreal.CapsuleComponent), 'CameraBoom attachment not restored'
unreal.EditorAssetLibrary.save_loaded_asset(bp)
unreal.log_warning('CAM_REPAIR saved parent='+str(b.get_attach_parent()))


