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

#ifndef TXDWIDGET_H_
#define TXDWIDGET_H_

#include <ui_TXDWidget.h>
#include <gtaformats/util/File.h>
#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTextureHeader.h>
#include "OpenTXDGUIModule.h"
#include <QtCore/QLinkedList>
#include "../../DisplayedFile.h"



class TXDWidget : public QWidget {
	Q_OBJECT

public:
	TXDWidget(DisplayedFile* dfile, const QString& texture, QWidget* parent);
	~TXDWidget();
	TXDArchive* getArchive() { return txd; }
	QLinkedList<TXDTextureHeader*> getSelectedTextures();

public slots:
	void saveTo(const File& file);

private slots:
	void textureActivated(QListWidgetItem* item, QListWidgetItem* previous);
	void textureListContextMenuRequested(const QPoint& pos);
	void textureExtractionRequested(bool checked);
	void loadConfigUiSettings();
	void sectionChanged(RWSection* sect) { dfile->setHasChanges(true); }

private:
	DisplayedFile* dfile;
	QAction* extractAction;
	OpenTXDGUIModule* openGUIModule;
	QTabWidget* compactTab;
	Ui_TXDWidget ui;
	TXDArchive* txd;
	TXDTextureHeader** textures;
};

#endif /* TXDWIDGET_H_ */
