/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#ifndef FILEVIEWWIDGET_H_
#define FILEVIEWWIDGET_H_

#include <ui_FileViewWidget.h>
#include <QtGui/QWidget>
#include "../FileOpenRequest.h"



class FileViewWidget : public QWidget {
	Q_OBJECT

public:
	FileViewWidget(const FileOpenRequest& request);
	~FileViewWidget();

private:
	Ui_FileViewWidget ui;
	QWidget* displayWidget;
};

#endif /* FILEVIEWWIDGET_H_ */
