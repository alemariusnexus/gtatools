#include "HexEditorUndoCommand.h"



HexEditorUndoCommand::HexEditorUndoCommand(HexEditor* editor, int offset, int len, const QByteArray& oldData,
			const QByteArray& newData, bool alreadyDone)
		: editor(editor), offset(offset), len(len), oldData(oldData), newData(newData), alreadyDone(alreadyDone)
{
}


void HexEditorUndoCommand::undo()
{
	QUndoStack* undoStack = editor->getUndoStack();
	editor->setUndoStack(NULL);

	editor->replace(offset, newData.length(), oldData);
	alreadyDone = false;

	editor->setCursor(offset+oldData.length(), offset);
	editor->ensureVisible(offset+oldData.length());

	editor->setUndoStack(undoStack);
}


void HexEditorUndoCommand::redo()
{
	if (!alreadyDone) {
		QUndoStack* undoStack = editor->getUndoStack();
		editor->setUndoStack(NULL);

		editor->replace(offset, len, newData);

		editor->setCursor(offset+len, offset);
		editor->ensureVisible(offset+len);

		editor->setUndoStack(undoStack);
	}
}
