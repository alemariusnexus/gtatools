/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#include "FormatManager.h"
#include "img/IMGFormatHandler.h"
#include "txd/TXDFormatHandler.h"
#include "ipl/IPLFormatHandler.h"
#include "ide/IDEFormatHandler.h"
#include "dff/DFFFormatHandler.h"
#include "gxt/GXTFormatHandler.h"



FormatManager::FormatManager()
{
	registerFormatHandler(TXDFormatHandler::getInstance());
	registerFormatHandler(new IMGFormatHandler);
	registerFormatHandler(new IPLFormatHandler);
	registerFormatHandler(new IDEFormatHandler);
	registerFormatHandler(DFFFormatHandler::getInstance());
	registerFormatHandler(GXTFormatHandler::getInstance());
}


FormatManager* FormatManager::getInstance()
{
	static FormatManager* manager = new FormatManager;
	return manager;
}


void FormatManager::registerFormatHandler(FormatHandler* handler)
{
	handlers << handler;
}


QLinkedList<FormatHandler*> FormatManager::getHandlers(const File& file)
{
	QLinkedList<FormatHandler*> list;

	HandlerList::iterator it;
	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		FormatHandler* handler = *it;
		if (handler->hasFileFormat(file)) {
			list << handler;
		}
	}

	return list;
}


FormatHandler* FormatManager::getHandler(const File& file)
{
	HandlerList::iterator it;

	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		FormatHandler* handler = *it;
		if (handler->hasFileFormat(file)) {
			return handler;
		}
	}

	return NULL;
}
