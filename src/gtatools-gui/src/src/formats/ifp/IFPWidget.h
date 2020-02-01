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

#ifndef IFPWIDGET_H_
#define IFPWIDGET_H_

#include <ui_IFPWidget.h>
#include <QWidget>
#include <nxcommon/file/File.h>
#include <gtaformats/ifp/IFPAnimation.h>
#include <QList>



class IFPWidget : public QWidget
{
	Q_OBJECT

public:
	IFPWidget(const File& file);

private slots:
	void currentAnimChanged(int row);
	void currentObjectChanged(int row);
	void frameNumSpinnerChanged(int frame);
	void frameNumSliderChanged(int frame);

private:
	void displayFrame(int fnum);

private:
	Ui_IFPWidget ui;
	QList<IFPAnimation*> anims;
};

#endif /* IFPWIDGET_H_ */
