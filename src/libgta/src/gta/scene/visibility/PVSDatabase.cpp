/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "PVSDatabase.h"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include "../Scene.h"
#include "../SceneObjectDefinitionInfo.h"
#include <algorithm>
#include <set>
#include <gtaformats/util/util.h>
#include <gtaformats/util/stream/StreamReader.h>
#include <gtaformats/util/stream/EndianSwappingStreamReader.h>
#include <gtaformats/util/stream/StreamWriter.h>
#include <gtaformats/util/stream/EndianSwappingStreamWriter.h>

using std::set;
using std::copy;
using std::find;
using std::ofstream;
using std::find;



PVSDatabase::PVSDatabase()
		: sectSizeX(500.0f), sectSizeY(500.0f), sectSizeZ(2000.0f)
{
}


void PVSDatabase::buildSections(Scene::ObjectIterator beg, Scene::ObjectIterator end)
{
	// Calculate the world visibility bounding box
	float bx1 = 0.0f;
	float by1 = 0.0f;
	float bz1 = 0.0f;
	float bx2 = 0.0f;
	float by2 = 0.0f;
	float bz2 = 0.0f;

	Scene::ObjectIterator it;

	for (it = beg ; it != end ; it++) {
		SceneObject* sobj = *it;

		if (sobj->getType() == SceneObjectStatic) {
			StaticSceneObject* object = (StaticSceneObject*) sobj;
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
	}


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
}


void PVSDatabase::calculatePVS(Scene::ObjectIterator beg, Scene::ObjectIterator end, unsigned int numThreads)
{
	Mutex* mutex = new Mutex;
	SectionCalculatorThread* threads = new SectionCalculatorThread[numThreads];

	uint32_t nextSect = 0;

	for (int i = 0 ; i < numThreads ; i++) {
		threads[i].mutex = mutex;
		threads[i].nextSect = &nextSect;
		threads[i].pvs = this;
		threads[i].beg = beg;
		threads[i].end = end;
		threads[i].start();
	}

	while (nextSect < numSects) {
		mutex->lock();

		vector<ProgressObserver*>::iterator pit;
		for (pit = progressObservers.begin() ; pit != progressObservers.end() ; pit++) {
			ProgressObserver* obsv = *pit;
			obsv->progressChanged(nextSect, numSects);
		}

		mutex->unlock();

		SleepMilliseconds(50);
	}

	for (int i = 0 ; i < numThreads ; i++) {
		threads[i].join();
	}

	delete[] threads;
	delete mutex;

	/*// And finally, calculate the PVS data
	for (uint32_t i = 0 ; i < numSects ; i++) {

	}*/
}


void PVSDatabase::SectionCalculatorThread::run()
{
	mutex->lock();

	while (*nextSect < pvs->numSects) {
		//printf("Calc %u/%u\n", *nextSect, pvs->numSects);
		PVSSection* sect = pvs->sections[*nextSect];
		(*nextSect)++;

		mutex->unlock();

		for (Scene::ObjectIterator it = beg ; it != end ; it++) {
			SceneObject* sobj = *it;

			if (sobj->getType() == SceneObjectStatic) {
				StaticSceneObject* obj = (StaticSceneObject*) sobj;
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
						obj = (StaticSceneObject*) obj->getLODParent();
					}
				}
			}
		}

		mutex->lock();
	}

	mutex->unlock();
}


PVSSection* PVSDatabase::findSection(float x, float y, float z)
{
	for (int i = 0 ; i < numSects ; i++) {
		PVSSection* sect = sections[i];

		if (sect->containsPoint(x, y, z)) {
			return sect;
		}
	}

	return NULL;
}


void PVSDatabase::queryPVS(float x, float y, float z, list<SceneObject*>& pvs)
{
	PVSSection* sect = findSection(x, y, z);

	// If sect is NULL, then we are outside the range for which sections exist. As the bounds for this range
	// are the draw distance bounds, we know that outside this range no objects will be visible.
	if (sect) {
		pvs.resize(sect->getPVSObjectCount());
		copy(sect->getPVSObjectBegin(), sect->getPVSObjectEnd(), pvs.begin());
	}
}


void PVSDatabase::removeProgressObserver(ProgressObserver* obsv)
{
	progressObservers.erase(find(progressObservers.begin(), progressObservers.end(), obsv));
}


void PVSDatabase::save(ostream* out, SceneObjectDefinitionDatabase* defDB, int flags)
{
	Engine* engine = Engine::getInstance();

	StreamWriter* writer;

	bool bigEndian;

#ifdef GTAFORMATS_LITTLE_ENDIAN
	bigEndian = (flags & SaveForceBigEndian) != 0;
	writer = bigEndian ? new EndianSwappingStreamWriter(out) : new StreamWriter(out);
#else
	bigEndian = (flags & SaveForceLittleEndian) == 0;
	writer = bigEndian ? new StreamWriter(out) : new EndianSwappingStreamWriter(out);
#endif

	// Magic header GTAT.PVS
	const char magicHeader[] = {'G', 'T', 'A', 'T', '.', 'P', 'V', 'S'};
	out->write(magicHeader, 8);

	uint8_t endianFlag = bigEndian ? 1 : 0;
	out->write((char*) &endianFlag, 1);

	// Version number (>= 10000 are development versions)
	uint32_t version = 10001;
	writer->writeU32(version);

	// The number of sections
	writer->writeU32(numSects);

	// The section data
	for (uint32_t i = 0 ; i < numSects ; i++) {
		PVSSection* sect = sections[i];
		writer->writeArrayCopyFloat((const float*) sect, 6);
	}

	// The number of group dependencies
	uint32_t numDeps = defDB->getGroupDependencyCount();
	writer->writeU32(numDeps);

	// Store the group dependencies...
	SceneObjectDefinitionDatabase::GroupDepIterator dit;
	for (dit = defDB->getGroupDependencyBegin() ; dit != defDB->getGroupDependencyEnd() ; dit++) {
		SceneObjectGroupDependency* dep = dit->second;

		uint32_t pathLen = dep->getRelativePath().length()+1;

		writer->writeU32(pathLen);
		out->write(dep->getRelativePath().get(), pathLen);

		uint64_t mtime = dep->getModifyTime();
		writer->writeU64(mtime);
	}

	// Number of file groups
	uint32_t numGroups = defDB->getFileGroupCount();
	writer->writeU32(numGroups);

	// Store the data for each file group...
	SceneObjectDefinitionDatabase::FileGroupIterator it;
	for (it = defDB->getFileGroupBegin() ; it != defDB->getFileGroupEnd() ; it++) {
		SceneObjectFileGroup* group = it->second;
		CString relPath = group->getRelativePath();
		uint32_t len = relPath.length()+1;

		writer->writeU32(len);
		out->write(relPath.get(), len);

		uint64_t mtime = group->getModifyTime();
		writer->writeU64(mtime);

		numDeps = group->getDependencyCount();
		writer->writeU32(numDeps);

		// Write the group dependency references
		SceneObjectFileGroup::DepIterator gdit;
		for (gdit = group->getDependencyBegin() ; gdit != group->getDependencyEnd() ; gdit++) {
			SceneObjectGroupDependency* dep = *gdit;

			uint32_t idx = defDB->indexOf(dep);
			writer->writeU32(idx);
		}

		uint32_t numGroupSects = 0;

		for (uint32_t i = 0 ; i < numSects ; i++) {
			PVSSection* sect = sections[i];

			PVSSection::ObjectList groupObjs;

			bool sectInGroup = false;

			PVSSection::ObjectIterator oit;
			for (oit = sect->getPVSObjectBegin() ; oit != sect->getPVSObjectEnd() ; oit++) {
				StaticSceneObject* obj = *oit;
				SceneObjectDefinitionInfo* info = obj->getDefinitionInfo();

				if (info->getFileGroup() == group) {
					sectInGroup = true;
					break;
				}
			}

			if (sectInGroup)
				numGroupSects++;
		}

		writer->writeU32(numGroupSects);

		for (uint32_t i = 0 ; i < numSects ; i++) {
			PVSSection* sect = sections[i];

			PVSSection::ObjectList groupObjs;

			PVSSection::ObjectIterator oit;
			for (oit = sect->getPVSObjectBegin() ; oit != sect->getPVSObjectEnd() ; oit++) {
				StaticSceneObject* obj = *oit;
				SceneObjectDefinitionInfo* info = obj->getDefinitionInfo();

				if (info->getFileGroup() == group) {
					groupObjs.push_back(obj);
				}
			}

			if (!groupObjs.empty()) {
				writer->writeU32(i);

				uint32_t numObjs = groupObjs.size();
				writer->writeU32(numObjs);

				for (oit = groupObjs.begin() ; oit != groupObjs.end() ; oit++) {
					StaticSceneObject* obj = *oit;
					SceneObjectDefinitionInfo* info = obj->getDefinitionInfo();

					uint32_t id = info->getID();
					writer->writeU32(id);
				}
			}
		}
	}
}


void PVSDatabase::save(const File& file, SceneObjectDefinitionDatabase* defDB, int flags)
{
	ostream* out = file.openOutputStream(ostream::binary | ostream::out);
	save(out, defDB, flags);
	delete out;
}


PVSDatabase::LoadingResult PVSDatabase::load(istream* stream, SceneObjectDefinitionDatabase* defDB,
		Scene::ObjectList& missingObjs, const File& rootDir, int flags)
{
	bool validateMtime = (flags & LoadValidateByModifyTime) != 0;

	const char expectedMagicHeader[8] = {'G', 'T', 'A', 'T', '.', 'P', 'V', 'S'};

	char magicHeader[8];
	stream->read(magicHeader, 8);

	if (strncmp(magicHeader, expectedMagicHeader, 8) != 0) {
		return ResultInvalidFormat;
	}

	uint8_t endianFlag; // 0 = little endian, 1 = big endian
	stream->read((char*) &endianFlag, 1);

	StreamReader* reader;

#ifdef GTAFORMATS_LITTLE_ENDIAN
	reader = (endianFlag == 0) ? new StreamReader(stream) : new EndianSwappingStreamReader(stream);
#else
	reader = (endianFlag == 1) ? new StreamReader(stream) : new EndianSwappingStreamReader(stream);
#endif

	int32_t expectedVersion = 10001;

	int32_t version = reader->readU32();

	if (version != expectedVersion) {
		return ResultWrongVersion;
	}

	reader->readU32(&numSects);

	sections = new PVSSection*[numSects];

	for (uint32_t i = 0 ; i < numSects ; i++) {
		PVSSection* sect = new PVSSection;
		reader->readArrayFloat((float*) sect, 6);
		sections[i] = sect;
	}

	uint32_t numDeps = reader->readU32();

	bool* depUpToDateTable = new bool[numDeps];

	for (uint32_t i = 0 ; i < numDeps ; i++) {
		uint32_t pathLen = reader->readU32();

		char* relPath = new char[pathLen];
		stream->read(relPath, pathLen);

		uint64_t mtime = reader->readU64();

		SceneObjectGroupDependency* dep = defDB->getGroupDependency(relPath);

		delete[] relPath;

		if (dep)
			depUpToDateTable[i] = (dep->getModifyTime() == mtime);
		else
			depUpToDateTable[i] = false;
	}

	set<CString> resolvedGroups;

	uint32_t numGroups = reader->readU32();

	for (uint32_t i = 0 ; i < numGroups ; i++) {
		uint32_t len = reader->readU32();

		char* relPath = new char[len];
		stream->read(relPath, len);

		SceneObjectFileGroup* group = defDB->getFileGroup(relPath);

		uint64_t mtime = reader->readU64();

		bool upToDate = true;

		reader->readU32(&numDeps);

		if (!group  ||  group->getDependencyCount() != numDeps) {
			// We either have an unknown group or dependencies were added or removed. In both cases, we mark
			// the group as not-up-to-date.
			upToDate = false;
			stream->ignore(numDeps*4);
		} else {
			if (validateMtime) {
				for (uint32_t j = 0 ; j < numDeps ; j++) {
					uint32_t idx = reader->readU32();

					if (!depUpToDateTable[idx]) {
						upToDate = false;
						break;
					}
				}
			} else {
				stream->ignore(numDeps*4);
			}
		}

		uint32_t numGroupSects = reader->readU32();

		File sourceFile(rootDir, relPath);

		if (validateMtime  &&  sourceFile.physicallyExists()) {
			if (sourceFile.getModifyTime() != mtime) {
				upToDate = false;
			}
		}

		if (!upToDate) {
			delete[] relPath;

			for (uint32_t j = 0 ; j < numGroupSects ; j++) {
				stream->ignore(4);
				uint32_t numObjs = reader->readU32();
				stream->ignore(numObjs*4);
			}

			continue;
		}

		resolvedGroups.insert(CString::from(relPath));

		for (uint32_t j = 0 ; j < numGroupSects ; j++) {
			uint32_t sectID = reader->readU32();

			PVSSection* sect = sections[sectID];

			uint32_t numObjs = reader->readU32();

			for (uint32_t k = 0 ; k < numObjs ; k++) {
				uint32_t id = reader->readU32();

				StaticSceneObject* obj = group->getObject(id);
				sect->addPotentiallyVisibleObject(obj);
			}
		}
	}

	delete[] depUpToDateTable;

	SceneObjectDefinitionDatabase::FileGroupIterator it;
	for (it = defDB->getFileGroupBegin() ; it != defDB->getFileGroupEnd() ; it++) {
		SceneObjectFileGroup* group = it->second;

		if (resolvedGroups.find(group->getRelativePath()) == resolvedGroups.end()) {
			SceneObjectFileGroup::ObjectIterator oit;

			for (oit = group->getObjectBegin() ; oit != group->getObjectEnd() ; oit++) {
				StaticSceneObject* obj = oit->second;

				if (obj->getDefinitionInfo()->isLODLeaf())
					missingObjs.push_back(obj);
			}
		}
	}

	return ResultOK;
}


PVSDatabase::LoadingResult PVSDatabase::load(const File& file, SceneObjectDefinitionDatabase* defDB,
		Scene::ObjectList& missingObjs, const File& rootDir, int flags)
{
	istream* stream = file.openInputOutputStream(istream::binary | istream::in);
	LoadingResult res = load(stream, defDB, missingObjs, rootDir, flags);
	delete stream;
	return res;
}
