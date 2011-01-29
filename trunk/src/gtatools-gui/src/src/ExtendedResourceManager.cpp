/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#include "ExtendedResourceManager.h"
#include <gtaformats/ide/IDEReader.h>
#include <gtaformats/ide/IDEStatement.h>
#include <gtaformats/ide/IDEStaticObject.h>
#include <gtaformats/ide/IDETimedObject.h>
#include <gtaformats/util/strutil.h>
#include <utility>
#include <iterator>

using std::pair;



void ExtendedResourceManager::addResource(const File& file)
{
	if (!file.isDirectory()) {
		FileContentType type = file.guessContentType();

		if (type == CONTENT_TYPE_IDE) {
			IDEReader ide(file);
			IDEStatement* stmt;

			while ((stmt = ide.readStatement())  !=  NULL) {
				if (stmt->getType() == IDE_TYPE_STATIC_OBJECT  ||  stmt->getType() == IDE_TYPE_TIMED_OBJECT) {
					IDEStaticObject* sobj = (IDEStaticObject*) stmt;
					char* meshName = new char[strlen(sobj->getModelName())+1];
					strtolower(meshName, sobj->getModelName());
					char* texName = new char[strlen(sobj->getTextureName())+1];
					strtolower(texName, sobj->getTextureName());

					if (strcmp(meshName, "lanitewin4_lan") == 0) {
						printf("FOUND: %s\n", texName);
					}

					meshTexIndex.insert(pair<hash_t, char*>(Hash(meshName), texName));
					delete[] meshName;
				}

				delete stmt;
			}

			return;
		}
	}

	ResourceManager::addResource(file);
}


int ExtendedResourceManager::getTexturesForMesh(hash_t meshHash, char**& texNames)
{
	pair<MeshTexMap::iterator, MeshTexMap::iterator> range = meshTexIndex.equal_range(meshHash);

	int numElements = distance(range.first, range.second);

	texNames = new char*[numElements];

	MeshTexMap::iterator it;
	int i = 0;
	for (it = range.first ; it != range.second ; it++, i++) {
		texNames[i] = it->second;
	}

	return numElements;
}
