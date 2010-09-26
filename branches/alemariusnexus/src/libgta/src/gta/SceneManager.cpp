/*
 * SceneManager.cpp
 *
 *  Created on: 29.08.2010
 *      Author: alemariusnexus
 */

#include "SceneManager.h"
#include <utility>

using std::pair;



void SceneManager::defineObject(int id, ObjectDefinition* def)
{
	definitions.insert(pair<int, ObjectDefinition*>(id, def));
}


ObjectDefinition* SceneManager::getObject(int id)
{
	DefMap::iterator it = definitions.find(id);

	if (it == definitions.end()) {
		return NULL;
	}

	return it->second;
}
