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

#ifndef ENTITYMANAGER_H_
#define ENTITYMANAGER_H_

#include "EntityHandler.h"
#include "../EntityOpenRequest.h"
#include <QtCore/QLinkedList>


class EntityManager {
private:
	typedef QLinkedList<EntityHandler*> HandlerList;

public:
	static EntityManager* getInstance();

public:
	void registerEntityHandler(EntityHandler* handler);
	QLinkedList<EntityHandler*> getEntityHandlers() { return handlers; }
	QLinkedList<EntityHandler*> getEntityHandlers(const EntityOpenRequest& req);
	EntityHandler* getEntityHandler(const EntityOpenRequest& req);

private:
	EntityManager();

private:
	HandlerList handlers;
};

#endif /* ENTITYMANAGER_H_ */
