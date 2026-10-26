import unreal

w = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem).get_game_world()
for a in unreal.GameplayStatics.get_all_actors_of_class(w, unreal.Actor):
    for c in a.get_components_by_class(unreal.PlayerBreakSequenceComponent):
        unreal.log_warning('BREAK_OWNER|actor=%s|controller=%s|component=%s|active=%s|waiting=%s|can=%s|wait=%s' % (
            a.get_path_name(), a.get_controller() if isinstance(a, unreal.Pawn) else None,
            c.get_path_name(), c.is_break_sequence_active(), c.is_waiting_for_finisher(), c.can_start_finisher(), c.get_editor_property('BreakWaitTime')))
for comp in unreal.ObjectIterator(unreal.PlayerBreakSequenceComponent):
    if comp.get_world():
        unreal.log_warning('ALL_BREAK_OBJECT|%s|world=%s|active=%s|waiting=%s' % (
            comp.get_path_name(), comp.get_world(), comp.is_break_sequence_active(), comp.is_waiting_for_finisher()))
for widget in unreal.ObjectIterator(unreal.UserWidget):
    if widget.get_class().get_name() == 'BreakUI_C':
        unreal.log_warning('BREAK_WIDGET_OWNER|%s|component=%s' % (widget.get_path_name(), widget.get_editor_property('BreakComponent')))
