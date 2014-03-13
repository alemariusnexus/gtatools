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

#ifndef IPLFORMATHANDLER_H_
#define IPLFORMATHANDLER_H_

#include "../EntityHandler.h"

class IPLFormatHandler: public EntityHandler {
	Q_OBJECT

public:
	virtual QString getFileFormatName(const File& file) const;
	virtual QLinkedList<QString> getFileFormatExtensions(const File&) const { return QLinkedList<QString>() << "ipl"; }
	virtual bool canHandle(const EntityOpenRequest& req) const;
	virtual DisplayedEntity* openEntity(const EntityOpenRequest& request);
};

#endif /* IPLFORMATHANDLER_H_ */