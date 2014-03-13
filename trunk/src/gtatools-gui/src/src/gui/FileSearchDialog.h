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

#ifndef FILESEARCHDIALOG_H_
#define FILESEARCHDIALOG_H_

#include <ui_FileSearchDialog.h>
#include <QtGui/QDialog>
#include <QtGui/QWidget>
#include <QtCore/QLinkedList>
#include <QtCore/QList>
#include <QtGui/QProgressDialog>
#include "../Task.h"
#include <nxcommon/file/FileFinder.h>



class FileSearchDialog : public QDialog {
	Q_OBJECT

public:
	FileSearchDialog(QWidget* parent = NULL);

private slots:
	void onCancel(bool checked);
	void onSearch(bool checked);

private:
	Ui_FileSearchDialog ui;
	FileFinder* finder;
};

#endif /* FILESEARCHDIALOG_H_ */
