/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef GXTFORMATHANDLER_H_
#define GXTFORMATHANDLER_H_

#include "../FormatHandler.h"
#include <gtaformats/gxt/GXTTable.h>
#include <QtCore/QLinkedList>
#include <QtCore/QMap>
#include <QtGui/QWidget>



class GXTFormatHandler : public FormatHandler {
	Q_OBJECT

public:
	static GXTFormatHandler* getInstance();

public:
	virtual QString getFormatName(const File* file = NULL) const { return tr("GXT Text File"); }
	virtual QLinkedList<QString> getFileFormatExtensions() const { return QLinkedList<QString>() << "gxt"; }
	virtual bool canHandle(const EntityOpenRequest& req) const;
	virtual DisplayedEntity* openEntity(const EntityOpenRequest& request);
	void iniExport(const File& file, const QLinkedList<GXTTable*>& tables);
	void stringListMatch(const File& matchFile, const QMap<QString, GXTTable*>& tables, QWidget* parent);

private:
	GXTFormatHandler() {}
};

#endif /* GXTFORMATHANDLER_H_ */
