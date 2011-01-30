/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef DFFXMLDUMPDIALOG_H_
#define DFFXMLDUMPDIALOG_H_


#include <ui_DFFXMLDumpDialog.h>
#include <QtGui/QDialog>


class DFFXMLDumpDialog : public QDialog
{
public:
	DFFXMLDumpDialog(QWidget* parent = NULL);
	bool isFrameDumpChecked() { return ui.frameBox->isChecked(); }
	bool isGeometryDumpChecked() { return ui.geometryBox->isChecked(); }
	bool isPartDumpChecked() { return ui.partBox->isChecked(); }
	bool isVertexDataDumpChecked() { return ui.vertexDataBox->isChecked(); }
	bool isMaterialDumpChecked() { return ui.materialBox->isChecked(); }
	bool isIndexDumpChecked() { return ui.indexBox->isChecked(); }

private:
	Ui_DFFXMLDumpDialog ui;
};

#endif /* DFFXMLDUMPDIALOG_H_ */
