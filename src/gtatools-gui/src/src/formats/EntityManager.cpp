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

#include "EntityManager.h"
#include "unrecog/UnrecognizedFormatHandler.h"
#include "txd/TXDFormatHandler.h"
#include "gxt/GXTFormatHandler.h"
#include "img/IMGFormatHandler.h"
#include "ipl/IPLFormatHandler.h"
#include "ide/IDEFormatHandler.h"
#include "dff/DFFFormatHandler.h"
#include "col/COLFormatHandler.h"
#include "ifp/IFPFormatHandler.h"



EntityManager::EntityManager()
{
	registerEntityHandler(new UnrecognizedFormatHandler());
	registerEntityHandler(TXDFormatHandler::getInstance());
	registerEntityHandler(GXTFormatHandler::getInstance());
	registerEntityHandler(new IMGFormatHandler);
	registerEntityHandler(new IPLFormatHandler);
	registerEntityHandler(new IDEFormatHandler);
	registerEntityHandler(DFFFormatHandler::getInstance());
	registerEntityHandler(new COLFormatHandler);
	registerEntityHandler(new IFPFormatHandler);
}


EntityManager* EntityManager::getInstance()
{
	static EntityManager* manager = new EntityManager;
	return manager;
}


void EntityManager::registerEntityHandler(EntityHandler* handler)
{
	handlers << handler;
}


QLinkedList<EntityHandler*> EntityManager::getEntityHandlers(const EntityOpenRequest& req)
{
	QLinkedList<EntityHandler*> list;

	HandlerList::iterator it;
	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		EntityHandler* handler = *it;
		if (handler->canHandle(req)) {
			list << handler;
		}
	}

	return list;
}


EntityHandler* EntityManager::getEntityHandler(const EntityOpenRequest& req)
{
	HandlerList::iterator it;

	int bestSuitability = 0;
	EntityHandler* bestHandler = NULL;

	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		EntityHandler* handler = *it;
		if (handler->canHandle(req)) {
			int suitability = handler->getSuitability(req);

			if (suitability > bestSuitability) {
				bestHandler = handler;
				bestSuitability = suitability;
			}
		}
	}

	return bestHandler;
}
