/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "FormatManager.h"
#include "unrecog/UnrecognizedFormatHandler.h"
#include "img/IMGFormatHandler.h"
#include "txd/TXDFormatHandler.h"
#include "ipl/IPLFormatHandler.h"
#include "ide/IDEFormatHandler.h"
#include "dff/DFFFormatHandler.h"
#include "gxt/GXTFormatHandler.h"
#include "col/COLFormatHandler.h"



FormatManager::FormatManager()
{
	registerFormatHandler(new UnrecognizedFormatHandler());
	registerFormatHandler(TXDFormatHandler::getInstance());
	registerFormatHandler(new IMGFormatHandler);
	registerFormatHandler(new IPLFormatHandler);
	registerFormatHandler(new IDEFormatHandler);
	registerFormatHandler(DFFFormatHandler::getInstance());
	registerFormatHandler(GXTFormatHandler::getInstance());
	registerFormatHandler(new COLFormatHandler);
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

	int bestSuitability = 0;
	FormatHandler* bestHandler = NULL;

	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		FormatHandler* handler = *it;
		if (handler->hasFileFormat(file)) {
			int suitability = handler->getSuitability(file);

			if (suitability > bestSuitability) {
				bestHandler = handler;
				bestSuitability = suitability;
			}
		}
	}

	return bestHandler;
}
