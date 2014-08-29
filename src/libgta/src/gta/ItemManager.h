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

#ifndef ITEMMANAGER_H_
#define ITEMMANAGER_H_

#include <gta/config.h>
#include "ItemDefinition.h"
#include "resource/ResourceObserver.h"
#include <unordered_map>

using std::unordered_map;



class ItemManager : public ResourceObserver {
	friend class Engine;

private:
	typedef unordered_map<int32_t, ItemDefinition*> ItemMap;

public:
	~ItemManager();
	virtual void resourceAdded(const File& file);
	void defineItem(int32_t id, ItemDefinition* item);
	ItemDefinition* getItem(int32_t id);

private:
	ItemManager() {}

private:
	ItemMap items;
};

#endif /* ITEMMANAGER_H_ */
