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

#ifndef PROFILECONFIGWIDGET_H_
#define PROFILECONFIGWIDGET_H_

#include "ui_ProfileConfigWidget.h"
#include <QtGui/QWidget>
#include <QtCore/QLinkedList>
#include "../Profile.h"



class ProfileConfigWidget : public QWidget {
	Q_OBJECT

public:
	ProfileConfigWidget(Profile* profile, QWidget* parent = NULL);
	~ProfileConfigWidget();
	QString getProfileName() { return ui.nameField->text(); }
	QString getRootDirectory() { return ui.rootDirField->text(); }
	void getResourceFiles(QLinkedList<QString>& dest);
	void getSearchResourceFiles(QLinkedList<QString>& dest);
	void getDATFiles(QLinkedList<QString>& dest);
	GameInfo::VersionMode getVersionMode() const
			{ return (GameInfo::VersionMode) ui.versionBox->itemData(ui.versionBox->currentIndex()).toInt(); }

private slots:
	void rootDirButtonClicked(bool checked);
	void versionBoxChanged(int index);

	void engineFileAddButtonClicked(bool checked = false);
	void engineDirAddButtonClicked(bool checked = false);
	void engineFileEditButtonClicked(bool checked = false);
	void engineFileRemoveButtonClicked(bool checked = false);
	void engineResourceSelectionChanged();

	void searchFileAddButtonClicked(bool checked = false);
	void searchDirAddButtonClicked(bool checked = false);
	void searchFileEditButtonClicked(bool checked = false);
	void searchFileRemoveButtonClicked(bool checked = false);
	void searchResourceSelectionChanged();

	void datFileAddButtonClicked(bool checked = false);
	void datFileEditButtonClicked(bool checked = false);
	void datFileRemoveButtonClicked(bool checked = false);
	void datSelectionChanged();

private:
	void clearFiles();
	void tryAutocomplete();

private:
	Ui_ProfileConfigWidget ui;
	Profile* displayedProfile;
};

#endif /* PROFILECONFIGWIDGET_H_ */
