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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "PVSData.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include "../Scene.h"
#include <algorithm>

using std::copy;
using std::find;
using std::ofstream;
using std::find;



PVSData::PVSData(Scene* scene)
		: scene(scene), sectSizeX(250.0f), sectSizeY(250.0f), sectSizeZ(2000.0f)
{
}


void PVSData::build()
{
	printf("Generating PVS data structures...\n");

	printf("  Calculating the world visibility bounding box...\n");

	// Calculate the world visibility bounding box
	float bx1 = 0.0f;
	float by1 = 0.0f;
	float bz1 = 0.0f;
	float bx2 = 0.0f;
	float by2 = 0.0f;
	float bz2 = 0.0f;

	Scene::ObjectIterator it;

	for (it = scene->getSceneObjectBegin() ; it != scene->getSceneObjectEnd() ; it++) {
		StaticSceneObject* object = *it;
		const float* mat = object->getModelMatrix().toArray();

		float ox = mat[12];
		float oy = mat[13];
		float oz = mat[14];
		float dd = object->getDefinition()->getDrawDistance();

		if (ox+dd > bx2) {
			bx2 = ox+dd;
		}
		if (ox-dd < bx1) {
			bx1 = ox-dd;
		}

		if (oy+dd > by2) {
			by2 = oy+dd;
		}
		if (oy-dd < by1) {
			by1 = oy-dd;
		}

		if (oz+dd > bz2) {
			bz2 = oz+dd;
		}
		if (oz-dd < bz1) {
			bz1 = oz-dd;
		}
	}

	printf("  Building the sections...\n");

	// Now build the sections
	int numSectsX = ceil((bx2-bx1) / sectSizeX);
	int numSectsY = ceil((by2-by1) / sectSizeY);
	int numSectsZ = ceil((bz2-bz1) / sectSizeZ);
	numSects = numSectsX * numSectsY * numSectsZ;

	sections = new PVSSection*[numSects];

	int i = 0;
	for (float xOffs = bx1 ; xOffs < bx2 ; xOffs += sectSizeX) {
		for (float yOffs = by1 ; yOffs < by2 ; yOffs += sectSizeY) {
			for (float zOffs = bz1 ; zOffs < bz2 ; zOffs += sectSizeZ) {
				PVSSection* section = new PVSSection(xOffs, yOffs, zOffs,
						xOffs+sectSizeX, yOffs+sectSizeY, zOffs+sectSizeZ);
				sections[i++] = section;
			}
		}
	}

	printf("  Calculating actual PVS data...\n");

	// And finally, calculate the PVS data
	for (i = 0 ; i < numSects ; i++) {
		PVSSection* sect = sections[i];

		for (it = scene->getSceneObjectBegin() ; it != scene->getSceneObjectEnd() ; it++) {
			StaticSceneObject* obj = *it;
			StaticSceneObject* baseObj = obj;

			while (obj) {
				const float* mat = obj->getModelMatrix().toArray();

				float ox = mat[12];
				float oy = mat[13];
				float oz = mat[14];
				float dd = obj->getDefinition()->getDrawDistance();

				if (sect->intersectsSphere(ox, oy, oz, dd)) {
					sect->addPotentiallyVisibleObject(baseObj);
					break;
				} else {
					obj = obj->getLODParent();
				}
			}
		}

		/*if (i % (numSects/100) == 0) {
			printf("    %d%% done\n", i / (numSects/100));
			vector<ProgressObserver*>::iterator pit;

			for (pit = progressObservers.begin() ; pit != progressObservers.end() ; pit++) {
				ProgressObserver* obsv = *pit;
				obsv->progressChanged(i / (numSects/100), 100);
			}
		}*/
	}

	printf("PVS DATA SUCCESSFULLY BUILT!\n");
}


PVSSection* PVSData::findSection(float x, float y, float z)
{
	for (int i = 0 ; i < numSects ; i++) {
		PVSSection* sect = sections[i];

		if (sect->containsPoint(x, y, z)) {
			return sect;
		}
	}

	return NULL;
}


void PVSData::queryPVS(float x, float y, float z, list<StaticSceneObject*>& pvs)
{
	PVSSection* sect = findSection(x, y, z);

	// If sect is NULL, then we are outside the range for which sections exist. As the bounds for this range
	// are the draw distance bounds, we know that outside this range no objects will be visible.
	if (sect) {
		pvs.resize(sect->getPVSObjectCount());
		copy(sect->getPVSObjectBegin(), sect->getPVSObjectEnd(), pvs.begin());
	}
}


void PVSData::removeProgressObserver(ProgressObserver* obsv)
{
	progressObservers.erase(find(progressObservers.begin(), progressObservers.end(), obsv));
}
