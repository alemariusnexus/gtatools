/*
 * ItemManager.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef ITEMMANAGER_H_
#define ITEMMANAGER_H_

#include "config.h"
#include "ItemDefinition.h"
#include "resource/ResourceObserver.h"

#ifdef CXX0X_AVAILABLE
#include <unordered_map>
using std::unordered_map;
#else
#include <map>
using std::map;
#endif



class ItemManager : public ResourceObserver {
private:
#ifdef CXX0X_AVAILABLE
	typedef unordered_map<int32_t, ItemDefinition*> ItemMap;
#else
	typedef map<int32_t, ItemDefinition*> ItemMap;
#endif

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
