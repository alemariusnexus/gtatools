/*
 * SceneManager.h
 *
 *  Created on: 29.08.2010
 *      Author: alemariusnexus
 */

#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include <map>
#include "ObjectDefinition.h"
#include <gtaformats/engine/ResourceIndex.h>

using std::map;



class SceneManager {
private:
	typedef map<int, ObjectDefinition*> DefMap;

public:
	void defineObject(int id, ObjectDefinition* def);
	ObjectDefinition* getObject(int id);

private:
	DefMap definitions;
};

#endif /* SCENEMANAGER_H_ */
