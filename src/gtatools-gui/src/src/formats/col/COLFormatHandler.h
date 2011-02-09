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

#ifndef COLFORMATHANDLER_H_
#define COLFORMATHANDLER_H_

#include "../FormatHandler.h"

class COLFormatHandler: public FormatHandler
{
public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("Collision File (COL)"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "col"; }
	virtual bool hasFileFormat(const File& file) const { return file.guessContentType() == CONTENT_TYPE_COL; }
	virtual QWidget* createWidgetForFile(const FileOpenRequest& request, QWidget* parent);
};

#endif /* COLFORMATHANDLER_H_ */
