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

#ifndef FORMATHANDLER_H_
#define FORMATHANDLER_H_

#include <gtaformats/util/File.h>
#include <QtCore/QString>
#include <QtCore/QObject>
#include "../gui/GUIModule.h"
#include <QtGui/QWidget>
#include <QtCore/QLinkedList>
#include <QtCore/QHash>
#include <QtCore/QVariant>
#include "../FileOpenRequest.h"
#include "../DisplayedFile.h"



class FormatHandler : public QObject {
	Q_OBJECT

public:
	FormatHandler() {}
	virtual ~FormatHandler() {}
	virtual QString getFormatName(const File* file = NULL) const = 0;
	virtual QLinkedList<QString> getFileFormatExtensions() const = 0;
	virtual bool hasFileFormat(const File& file) const = 0;
	virtual DisplayedFile* openFile(const FileOpenRequest& request) { return NULL; }
	virtual int getSuitability(const File& file) const { return 10000; }

	virtual QString buildFileDialogFilter() const;
};

#endif /* FORMATHANDLER_H_ */
