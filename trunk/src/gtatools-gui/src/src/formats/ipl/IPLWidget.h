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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef IPLWIDGET_H_
#define IPLWIDGET_H_

#include <QtGui/QWidget>
#include <ui_IPLWidget.h>
#include <gtaformats/util/File.h>
#include <QtGui/QBrush>



class IPLWidget : public QWidget {
	Q_OBJECT

public:
	IPLWidget(QWidget* parent, const File& file);

private:
	QTableWidgetItem* createItem(const QString& text, bool link = false);

private slots:
	void instanceTableCellDoubleClicked(int row, int col);

private:
	Ui_IPLWidget ui;
	QBrush linkBrush;
};

#endif /* IPLWIDGET_H_ */
