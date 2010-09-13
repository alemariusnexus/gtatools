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

#ifndef FILESEARCHDIALOG_H_
#define FILESEARCHDIALOG_H_

#include <ui_FileSearchDialog.h>
#include <qdialog.h>
#include <qwidget.h>
#include <qlinkedlist.h>
#include <qlist.h>
#include "../StringMatcher.h"
#include "../formats/FormatHandler.h"
#include <qprogressdialog.h>
#include "../Task.h"



class FileSearchDialog : public QDialog {
	Q_OBJECT

public:
	FileSearchDialog(QWidget* parent = NULL);

private:
	bool collectResults(const File& file, StringMatcher* matcher, bool dirs, QLinkedList<FormatHandler*> handlers,
			QList<File*>& results, int filesMax, int& filesDone, Task* task);

private slots:
	void onCancel(bool checked);
	void onSearch(bool checked);

private:
	Ui_FileSearchDialog ui;
	bool cancelled;
};

#endif /* FILESEARCHDIALOG_H_ */
