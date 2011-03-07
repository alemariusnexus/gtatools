/*
 * ItemManager.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "ItemManager.h"
#include <gtaformats/gtaide.h>
#include <utility>

using std::pair;


void ItemManager::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_IDE) {
		IDEReader ide(file);
		IDEStatement* stmt;

		while ((stmt = ide.readStatement())  !=  NULL) {
			switch (stmt->getType()) {
			case IDETypeStaticObject:
				IDEStaticObject* sobj = (IDEStaticObject*) stmt;
				ItemDefinition* item = new ItemDefinition(*sobj);
				defineItem(sobj->getId(), item);
				break;
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
