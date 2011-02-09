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

#ifndef FORMATMANAGER_H_
#define FORMATMANAGER_H_

#include "FormatHandler.h"
#include <QtCore/QLinkedList>


class FormatManager {
private:
	typedef QLinkedList<FormatHandler*> HandlerList;

public:
	static FormatManager* getInstance();

public:
	void registerFormatHandler(FormatHandler* provider);
	QLinkedList<FormatHandler*> getHandlers() { return handlers; }
	QLinkedList<FormatHandler*> getHandlers(const File& file);
	FormatHandler* getHandler(const File& file);

private:
	FormatManager();

private:
	HandlerList handlers;
};

#endif /* FORMATMANAGER_H_ */
