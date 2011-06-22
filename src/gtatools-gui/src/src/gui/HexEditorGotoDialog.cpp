/*
 * HexEditorGotoDialog.cpp
 *
 *  Created on: 21.06.2011
 *      Author: alemariusnexus
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
		int cpos = editor->getCursorPosition();

		if (ok) {
			int newPos;

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

			editor->setCursorPosition(newPos);
		}
	}
}
