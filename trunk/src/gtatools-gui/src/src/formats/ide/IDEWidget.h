/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#ifndef IDEWIDGET_H_
#define IDEWIDGET_H_

#include <QtGui/QWidget>
#include <ui_IDEWidget.h>
#include <gtaformats/util/File.h>
#include <QtCore/QLinkedList>
#include <QtGui/QBrush>
#include "../../FileOpenRequest.h"



class IDEWidget : public QWidget {
	Q_OBJECT

public:
	IDEWidget(QWidget* parent, const FileOpenRequest& request);

private:
	QTableWidgetItem* createItem(const QString& text, bool link = false);

private slots:
	void staticObjectTableCellDoubleClicked(int row, int col);
	void timedObjectTableCellDoubleClicked(int row, int col);
	void animationTableCellDoubleClicked(int row, int col);
	void pedTableCellDoubleClicked(int row, int col);
	void weaponTableCellDoubleClicked(int row, int col);

private:
	Ui_IDEWidget ui;
	QBrush linkBrush;
};

#endif /* IDEWIDGET_H_ */
