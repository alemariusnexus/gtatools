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

#ifndef CONFIGWIDGET_H_
#define CONFIGWIDGET_H_

#include <ui_ConfigWidget.h>
#include <QtGui/QWidget>
#include <QtCore/QList>
#include "../Profile.h"



class ConfigWidget : public QWidget {
	Q_OBJECT

public:
	ConfigWidget(QWidget* parent = NULL);
	~ConfigWidget();

private slots:
	void selectedProfileChanged(int index);
	void buttonClicked(QAbstractButton* button);
	void onNewProfile(bool);
	void onRemoveProfile(bool);

private:
	void apply();
	void cancel();

public:
	Ui_ConfigWidget ui;
	QList<Profile*> profiles;
};

#endif /* CONFIGWIDGET_H_ */
