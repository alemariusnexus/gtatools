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

#ifndef FORMATMANAGER_H_
#define FORMATMANAGER_H_

#include "FormatHandler.h"
#include "../EntityOpenRequest.h"
#include <QtCore/QLinkedList>


class FormatManager {
private:
	typedef QLinkedList<FormatHandler*> HandlerList;

public:
	static FormatManager* getInstance();

public:
	void registerFormatHandler(FormatHandler* provider);
	QLinkedList<FormatHandler*> getHandlers() { return handlers; }
	QLinkedList<FormatHandler*> getHandlers(const EntityOpenRequest& req);
	FormatHandler* getHandler(const EntityOpenRequest& req);

private:
	FormatManager();

private:
	HandlerList handlers;
};

#endif /* FORMATMANAGER_H_ */
