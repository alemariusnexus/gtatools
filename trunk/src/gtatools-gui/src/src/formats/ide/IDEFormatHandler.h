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

#ifndef IDEFORMATHANDLER_H_
#define IDEFORMATHANDLER_H_

#include "../EntityHandler.h"
#include "../../System.h"

class IDEFormatHandler: public EntityHandler {
	Q_OBJECT

public:
	IDEFormatHandler();
	virtual QString getFileFormatName(const File& file) const { return tr("Item Definition File (IDE)"); }
	virtual QLinkedList<QString> getFileFormatExtensions(const File& file) const { return QLinkedList<QString>() << "ide"; }
	virtual bool canHandle(const EntityOpenRequest& req) const;
	virtual DisplayedEntity* openEntity(const EntityOpenRequest& request);

private slots:
	void systemQuerySent(const SystemQuery& query, QList<SystemQueryResult>& results);
};

#endif /* IDEFORMATHANDLER_H_ */
