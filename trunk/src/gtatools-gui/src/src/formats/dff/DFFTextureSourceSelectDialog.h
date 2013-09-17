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

#ifndef DFFTEXTURESOURCESELECTDIALOG_H_
#define DFFTEXTURESOURCESELECTDIALOG_H_

#include <QtGui/QDialog>
#include <ui_DFFTextureSourceSelectDialog.h>



class DFFTextureSourceSelectDialog : public QDialog {
	Q_OBJECT

public:
	DFFTextureSourceSelectDialog(QWidget* parent = NULL);
	void setSource(const QString& source, bool path = false);
	bool isPathSource() const { return ui.pathButton->isChecked(); }
	QString getSource() const { return isPathSource() ? ui.pathField->text() : ui.nameField->text(); }

private slots:
	void pathChosen();

private:
	Ui_DFFTextureSourceSelectDialog ui;
};

#endif /* DFFTEXTURESOURCESELECTDIALOG_H_ */
