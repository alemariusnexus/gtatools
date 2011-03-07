/*
 * ItemManager.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef ITEMMANAGER_H_
#define ITEMMANAGER_H_

#include "ItemDefinition.h"
#include "resource/ResourceObserver.h"
#include <map>

using std::map;


class ItemManager : public ResourceObserver {
private:
	typedef map<int32_t, ItemDefinition*> ItemMap;

public:
	static ItemManager* getInstance() { static ItemManager inst; return &inst; }

public:
	virtual void resourceAdded(const File& file);
	void defineItem(int32_t id, ItemDefinition* item);
	ItemDefinition* getItem(int32_t id);

private:
	ItemManager() {}

private:
	ItemMap items;
};

#endif /* ITEMMANAGER_H_ */
