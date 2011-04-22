/*
 * PVSData.cpp
 *
 *  Created on: 20.03.2011
 *      Author: alemariusnexus
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
		//: scene(scene), sectSizeX(250.0f), sectSizeY(250.0f), sectSizeZ(2000.0f)
		: scene(scene), sectSizeX(500.0f), sectSizeY(500.0f), sectSizeZ(2000.0f)
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
		DefaultSceneObject* object = *it;
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

		//printf("    Calculating PVS data for section %d/%d...\n", i+1, numSects);

		for (it = scene->getSceneObjectBegin() ; it != scene->getSceneObjectEnd() ; it++) {
			DefaultSceneObject* obj = *it;
			DefaultSceneObject* baseObj = obj;

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

		if (i % (numSects/100) == 0) {
			printf("    %d%% done\n", i / (numSects/100));
			vector<ProgressObserver*>::iterator pit;

			for (pit = progressObservers.begin() ; pit != progressObservers.end() ; pit++) {
				ProgressObserver* obsv = *pit;
				obsv->progressChanged(i / (numSects/100), 100);
			}
		}
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


void PVSData::queryPVS(float x, float y, float z, vector<DefaultSceneObject*>& pvs)
{
	PVSSection* sect = findSection(x, y, z);

	// If sect is NULL, then we are outside the range for which sections exist. As the bounds for this range
	// are the draw distance bounds, we know that outside this range no objects will be visible.
	if (sect) {
		pvs.resize(sect->getPVSObjectCount());
		copy(sect->getPVSObjectBegin(), sect->getPVSObjectEnd(), pvs.begin());
	}
}


void PVSData::serialize(ostream* out)
{
	uint32_t version = PVS_VERSION_DEVELOPMENT;

	char header[] = "GPVS";
	out->write(header, 4);
	out->write((char*) &version, 4);
	out->write((char*) &numSects, 4);

	uint64_t hash = scene->getSceneObjectCount();

	out->write((char*) &hash, 8);

	for (int i = 0 ; i < numSects ; i++) {
		PVSSection* sect = sections[i];

		float x1, y1, z1, x2, y2, z2;
		sect->getFirstCorner(x1, y1, z1);
		sect->getSecondCorner(x2, y2, z2);
		out->write((char*) &x1, 4);
		out->write((char*) &y1, 4);
		out->write((char*) &z1, 4);
		out->write((char*) &x2, 4);
		out->write((char*) &y2, 4);
		out->write((char*) &z2, 4);
		size_type numObjs = sect->getPVSObjectCount();
		out->write((char*) &numObjs, 4);

		PVSSection::ObjectIterator it;

		for (it = sect->getPVSObjectBegin() ; it != sect->getPVSObjectEnd() ; it++) {
			DefaultSceneObject* obj = *it;
			int id = obj->getID();
			out->write((char*) &id, 4);
		}
	}
}


void PVSData::serialize(const File& file)
{
	ofstream out(file.getPath()->toString(), ofstream::binary);
	serialize(&out);
}


bool PVSData::unserialize(istream* in)
{
	char header[4];
	in->read(header, 4);

	if (strncmp(header, "GPVS", 4) != 0) {
		return false;
	}

	uint32_t version;
	in->read((char*) &version, 4);

	if (version != PVS_VERSION_DEVELOPMENT) {
		return false;
	}

	in->read((char*) &numSects, 4);

	uint64_t hash = scene->getSceneObjectCount();

	uint64_t savedHash;
	in->read((char*) &savedHash, 8);

	if (hash != savedHash) {
		return false;
	}

	sections = new PVSSection*[numSects];

	for (int i = 0 ; i < numSects ; i++) {
		float x1, y1, z1, x2, y2, z2;
		in->read((char*) &x1, 4);
		in->read((char*) &y1, 4);
		in->read((char*) &z1, 4);
		in->read((char*) &x2, 4);
		in->read((char*) &y2, 4);
		in->read((char*) &z2, 4);

		PVSSection* sect = new PVSSection(x1, y1, z1, x2, y2, z2);
		sections[i] = sect;

		int numObjs;
		in->read((char*) &numObjs, 4);

		for (int j = 0 ; j < numObjs ; j++) {
			int id;
			in->read((char*) &id, 4);
			DefaultSceneObject* obj = scene->getObjectByID(id);
			sect->addPotentiallyVisibleObject(obj);
		}
	}

	return true;
}


bool PVSData::unserialize(const File& file)
{
	istream* in = file.openInputStream(istream::binary);
	bool retval = unserialize(in);
	delete in;
	return retval;
}


void PVSData::removeProgressObserver(ProgressObserver* obsv)
{
	progressObservers.erase(find(progressObservers.begin(), progressObservers.end(), obsv));
}
