/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "ItemManager.h"
#include "MapItemDefinition.h"
#include "Engine.h"
#include <gtaformats/gtaide.h>
#include <utility>

using std::pair;



ItemManager::~ItemManager()
{
	for (ItemMap::iterator it = items.begin() ; it != items.end() ; it++) {
		delete it->second;
	}
}


void ItemManager::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_IDE) {
		Engine* engine = Engine::getInstance();

		IDEReader ide(file);
		IDEStatement* stmt;

		while ((stmt = ide.readStatement())  !=  NULL) {
			idetype_t type = stmt->getType();

			if (type == IDETypeStaticObject  ||  type == IDETypeTimedObject) {
				IDEStaticObject* sobj = (IDEStaticObject*) stmt;
				MapItemDefinition* item = new MapItemDefinition(sobj);
				defineItem(sobj->getID(), item);
			} else if (type == IDETypeAnimation) {
				IDEAnimation* anim = (IDEAnimation*) stmt;
				MapItemDefinition* item = new MapItemDefinition(anim);
				defineItem(anim->getID(), item);
			}

			delete stmt;
		}
	}
}


void ItemManager::defineItem(int32_t id, ItemDefinition* item)
{
	items.insert(pair<int32_t, ItemDefinition*>(id, item));
}


ItemDefinition* ItemManager::getItem(int32_t id)
{
	ItemMap::iterator it = items.find(id);

	if (it == items.end()) {
		return NULL;
	}

	return it->second;
}
