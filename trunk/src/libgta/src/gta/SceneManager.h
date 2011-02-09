/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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
 */

#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include "config.h"
#include <map>
#include "ObjectDefinition.h"

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
