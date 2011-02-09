/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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
 */

#ifndef EXTENDEDRESOURCEMANAGER_H_
#define EXTENDEDRESOURCEMANAGER_H_

#include <gta/ResourceManager.h>
#include <map>
#include <gta/Engine.h>

using std::multimap;



class ExtendedResourceManager : public ResourceManager {
	typedef multimap<hash_t, char*> MeshTexMap;

public:
	virtual void addResource(const File& file);
	int getTexturesForMesh(hash_t meshHash, char**& texNames);
	int getTexturesForMesh(const char* meshName, char**& texNames)
			{ return getTexturesForMesh(Hash(meshName), texNames); }

private:
	MeshTexMap meshTexIndex;
};

#endif /* EXTENDEDRESOURCEMANAGER_H_ */
