/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

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
