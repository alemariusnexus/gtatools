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

#ifndef TEXTURESEARCHDIALOG_H_
#define TEXTURESEARCHDIALOG_H_

#include <ui_TextureSearchDialog.h>
#include <QtGui/QDialog>
#include <QtCore/QLinkedList>
#include <gtaformats/util/File.h>
#include <QtGui/QProgressDialog>
#include "../../Task.h"
#include "TextureFileFinder.h"




class TextureSearchDialog : public QDialog {
	Q_OBJECT

private:
	struct TextureMatch {
		File* txdFile;
		QString textureName;
	};

public:
	TextureSearchDialog(QWidget* parent, const QLinkedList<File*>& rootFiles);

private slots:
	void onCancel(bool checked);
	void onSearch(bool checked);

private:
	Ui_TextureSearchDialog ui;
	QLinkedList<File*> rootFiles;
	TextureFileFinder* finder;
};

#endif /* TEXTURESEARCHDIALOG_H_ */
