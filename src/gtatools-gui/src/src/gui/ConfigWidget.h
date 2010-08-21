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

#ifndef CONFIGWIDGET_H_
#define CONFIGWIDGET_H_

#include <ui_ConfigWidget.h>
#include <qwidget.h>
#include <QList>
#include "../Profile.h"



class ConfigWidget : public QWidget {
	Q_OBJECT

public:
	ConfigWidget(QWidget* parent = NULL);
	~ConfigWidget();

public slots:
	void selectedProfileChanged(int index);
	void onApply(bool checked);
	void onCancel(bool checked);
	void onNewProfile(bool);
	void onRemoveProfile(bool);

public:
	Ui_ConfigWidget ui;
	QList<Profile*> profiles;
};

#endif /* CONFIGWIDGET_H_ */
