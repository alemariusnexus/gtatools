/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "HexEditorGotoDialog.h"



HexEditorGotoDialog::HexEditorGotoDialog(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);
}


void HexEditorGotoDialog::exec(HexEditor* editor)
{
	int res = QDialog::exec();

	if (res == Accepted) {
		bool ok;
		int offset = ui.offsetField->text().toInt(&ok, 0);
		bool relative = ui.cposBox->isChecked();
		bool backwards = ui.backwardsBox->isChecked();
		bool expand = ui.expandBox->isChecked();
		int cpos = editor->getCursorPosition();
		int anchor = editor->getCursorAnchor();

		if (ok) {
			int newPos;
			int newAnchor = -1;

			if (backwards)
				offset = -offset;

			if (relative)
				newPos = cpos+offset;
			else
				newPos = offset;

			if (newPos < 0)
				newPos = 0;
			else if (newPos > editor->getDataSize())
				newPos = editor->getDataSize();

			if (expand)
				newAnchor = anchor;

			if (newAnchor == -1)
				newAnchor = newPos;

			editor->setCursor(newPos, newAnchor);
		}
	}
}
