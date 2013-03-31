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
#include "PVSVisibilitySet.h"
#include <algorithm>
#include <set>
#include <gtaformats/util/stream/StreamReader.h>
#include <gtaformats/util/stream/EndianSwappingStreamReader.h>
#include <gtaformats/util/stream/StreamWriter.h>
#include <gtaformats/util/stream/EndianSwappingStreamWriter.h>
#include <gtaformats/util/CRC32.h>
#include "../../Engine.h"
#include "../../EngineException.h"

using std::set;
using std::copy;
using std::find;
using std::ofstream;
using std::find;



struct IPVSSection
{
	PVSSection* sect;
	uint32_t idx;
};







PVSDatabase::PVSDatabase()
		: sections(NULL), numSects(0), uncalculatedObjCount(0)
{
}


PVSSection* PVSDatabase::findSection(float x, float y, float z)
{
	for (uint32_t i = 0 ; i < numSects ; i++) {
		PVSSection* sect = sections[i];

		if (sect->containsPoint(x, y, z)) {
			return sect;
		}
	}

	return NULL;
}


bool PVSDatabase::queryPVS(list<PVSSceneObject*>& pvs, const Vector3& pos, float distMultiplier,
		float* chosenDistMultiplier)
{

	PVSSection* sect = findSection(pos.getX(), pos.getY(), pos.getZ());

	if (sect) {
		PVSVisibilitySet* set = sect->getVisibilitySet(distMultiplier);

		if (set) {
			if (chosenDistMultiplier)
				*chosenDistMultiplier = set->getDistanceMultiplier();

			pvs.resize(pvs.size() + set->getPVSObjectCount());
			PVSVisibilitySet::ObjectIterator beg = set->getPVSObjectBegin();
			PVSVisibilitySet::ObjectIterator end = set->getPVSObjectEnd();
			list<PVSSceneObject*>::iterator pvsIt = pvs.begin();
			while (beg != end) {
				*pvsIt++ = (*beg)->obj;
				beg++;
			}
		} else {
			return false;
		}
	}

	return true;
}


/*bool PVSDatabase::queryPVS(float x, float y, float z, float distMultiplier, list<PVSSceneObject*>& pvs)
{
	PVSSection* sect = findSection(x, y, z);

	// If sect is NULL, then we are outside the range for which sections exist. As the bounds for this range
	// are the draw distance bounds, we know that outside this range no objects will be visible.
	if (sect) {
		PVSVisibilitySet* set = sect->getVisibilitySet(distMultiplier);

		if (!set)
			return false;

		list<PVSSceneObject*>::iterator endIt = pvs.end();
		endIt--;
		pvs.resize(pvs.size() + set->getPVSObjectCount());
		endIt++;
		copy(set->getPVSObjectBegin(), set->getPVSObjectEnd(), endIt);
	}

	return true;
}*/


void PVSDatabase::removeProgressObserver(ProgressObserver* obsv)
{
	progressObservers.erase(find(progressObservers.begin(), progressObservers.end(), obsv));
}


void PVSDatabase::save(ostream* out, int flags)
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
	uint32_t version = 10003;
	writer->writeU32(version);

	// The number of sections
	writer->writeU32(numSects);

	// The section data
	for (uint32_t i = 0 ; i < numSects ; i++) {
		PVSSection* sect = sections[i];
		writer->writeArrayCopyFloat((const float*) sect, 6);

		writer->writeU32(sect->getVisibilitySetCount());

		PVSSection::SetIterator it;
		for (it = sect->getVisibilitySetBegin() ; it != sect->getVisibilitySetEnd() ; it++) {
			PVSVisibilitySet* set = *it;
			writer->writeFloat(set->getDistanceMultiplier());
		}
	}

	// The number of group dependencies
	uint32_t numDeps = defDB.getGroupDependencyCount();
	writer->writeU32(numDeps);

	// Store the group dependencies...
	SceneObjectDefinitionDatabase::GroupDepIterator dit;
	for (dit = defDB.getGroupDependencyBegin() ; dit != defDB.getGroupDependencyEnd() ; dit++) {
		SceneObjectGroupDependency* dep = dit->second;

		uint32_t pathLen = dep->getRelativePath().length()+1;

		writer->writeU32(pathLen);
		out->write(dep->getRelativePath().get(), pathLen);

		uint32_t checksum = dep->getChecksum();
		writer->writeU32(checksum);
	}

	// Number of file groups
	uint32_t numGroups = fileGroups.size();
	writer->writeU32(numGroups);

	// Store the data for each file group...
	for (FileGroupIterator it = fileGroups.begin() ; it != fileGroups.end() ; it++) {
		InternalSceneObjectFileGroup* igroup = it->second;
		SceneObjectFileGroup* group = igroup->getFileGroup();

		CString relPath = group->getRelativePath();
		uint32_t len = relPath.length()+1;

		writer->writeU32(len);
		out->write(relPath.get(), len);

		uint32_t checksum = group->getChecksum();
		writer->writeU32(checksum);

		numDeps = group->getDependencyCount();
		writer->writeU32(numDeps);

		// Write the group dependency references
		SceneObjectFileGroup::DepIterator gdit;
		for (gdit = group->getDependencyBegin() ; gdit != group->getDependencyEnd() ; gdit++) {
			SceneObjectGroupDependency* dep = *gdit;

			uint32_t idx = defDB.indexOf(dep);
			writer->writeU32(idx);
		}

		writer->writeU32(igroup->getObjectCount());

		// Write the IDs of all objects
		InternalSceneObjectFileGroup::ObjectIterator ooit;
		for (ooit = igroup->getObjectBegin() ; ooit != igroup->getObjectEnd() ; ooit++) {
			PVSSceneObject* obj = ooit->second->obj;

			uint32_t id = ooit->first;
			writer->writeU32(id);
		}

		list<IPVSSection> groupSects;

		for (uint32_t i = 0 ; i < numSects ; i++) {
			PVSSection* sect = sections[i];

			bool sectInGroup = false;

			PVSSection::SetIterator vsit;
			for (vsit = sect->getVisibilitySetBegin() ; vsit != sect->getVisibilitySetEnd() ; vsit++) {
				PVSVisibilitySet* set = *vsit;

				PVSVisibilitySet::ObjectIterator oit;
				for (oit = set->getPVSObjectBegin() ; oit != set->getPVSObjectEnd() ; oit++) {
					PVSSceneObject* obj = (*oit)->obj;
					SceneObjectDefinitionInfo* info = obj->getDefinitionInfo();

					if (info) {
						if (info->getFileGroup() == group) {
							sectInGroup = true;
							break;
						}
					}
				}

				if (sectInGroup)
					break;
			}

			if (sectInGroup) {
				IPVSSection isect;
				isect.sect = sect;
				isect.idx = i;
				groupSects.push_back(isect);
			}
		}

		writer->writeU32(groupSects.size());

		for (list<IPVSSection>::iterator sit = groupSects.begin() ; sit != groupSects.end() ; sit++) {
			IPVSSection isect = *sit;
			PVSSection* sect = isect.sect;

			writer->writeU32(isect.idx);

			PVSSection::SetIterator vsit;
			for (vsit = sect->getVisibilitySetBegin() ; vsit != sect->getVisibilitySetEnd() ; vsit++) {
				PVSVisibilitySet* set = *vsit;

				PVSVisibilitySet::ObjectList groupObjs;

				PVSVisibilitySet::ObjectIterator oit;
				for (oit = set->getPVSObjectBegin() ; oit != set->getPVSObjectEnd() ; oit++) {
					PVSSceneObjectContainer* objc = *oit;
					PVSSceneObject* obj = objc->obj;
					SceneObjectDefinitionInfo* info = obj->getDefinitionInfo();

					if (info) {
						if (info->getFileGroup() == group) {
							groupObjs.push_back(objc);
						}
					}
				}

				uint32_t numObjs = groupObjs.size();
				writer->writeU32(numObjs);

				for (oit = groupObjs.begin() ; oit != groupObjs.end() ; oit++) {
					PVSSceneObjectContainer* objc = *oit;
					PVSSceneObject* obj = objc->obj;
					SceneObjectDefinitionInfo* info = obj->getDefinitionInfo();

					uint32_t id = info->getID();
					writer->writeU32(id);
				}
			}
		}
	}
}


void PVSDatabase::save(const File& file, int flags)
{
	ostream* out = file.openOutputStream(ostream::binary | ostream::out);
	save(out, flags);
	delete out;
}


PVSDatabase::LoadingResult PVSDatabase::load(istream* stream, const File& rootDir, int flags)
{
	bool validateCRC = (flags & LoadValidateByCRC) != 0;

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

	int32_t expectedVersion = 10003;

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

		uint32_t numSets = reader->readU32();

		for (uint32_t j = 0 ; j < numSets ; j++) {
			float dmul = reader->readFloat();
			PVSVisibilitySet* set = new PVSVisibilitySet(dmul);
			sect->addVisibilitySet(set);
		}
	}

	uint32_t numDeps = reader->readU32();

	bool* depUpToDateTable = new bool[numDeps];

	for (uint32_t i = 0 ; i < numDeps ; i++) {
		uint32_t pathLen = reader->readU32();

		char* relPath = new char[pathLen];
		stream->read(relPath, pathLen);

		uint32_t checksum = reader->readU32();

		SceneObjectGroupDependency* dep = defDB.getGroupDependency(relPath);

		delete[] relPath;

		if (dep)
			depUpToDateTable[i] = (dep->getChecksum() == checksum);
		else
			depUpToDateTable[i] = false;
	}

	set<CString> resolvedGroups;

	uint32_t numGroups = reader->readU32();

	for (uint32_t i = 0 ; i < numGroups ; i++) {
		uint32_t len = reader->readU32();

		char* relPath = new char[len];
		stream->read(relPath, len);

		InternalSceneObjectFileGroup* igroup = getFileGroup(relPath);
		SceneObjectFileGroup* group = igroup->getFileGroup();

		uint32_t checksum = reader->readU32();

		bool upToDate = true;

		reader->readU32(&numDeps);

		if (!group  ||  group->getDependencyCount() != numDeps) {
			// We either have an unknown group or dependencies were added or removed. In both cases, we mark
			// the group as not-up-to-date.
			upToDate = false;
			stream->ignore(numDeps*4);
		} else {
			if (validateCRC) {
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

		uint32_t objCount = reader->readU32();

		File sourceFile(rootDir, relPath);

		if (validateCRC  &&  sourceFile.physicallyExists()) {
			//printf("Checking %s: %X vs. %X\n", sourceFile.getPath()->getFileName().get(), sourceFile.crc32(), checksum);
			if (sourceFile.crc32() != checksum) {
				upToDate = false;
			}
		}

		if (!upToDate) {
			delete[] relPath;

			stream->ignore(objCount*4);

			uint32_t numGroupSects = reader->readU32();

			for (uint32_t j = 0 ; j < numGroupSects ; j++) {
				stream->ignore(4);
				uint32_t numObjs = reader->readU32();
				stream->ignore(numObjs*4);
			}

			continue;
		}

		for (uint32_t i = 0 ; i < objCount ; i++) {
			uint32_t id = reader->readU32();
			PVSSceneObjectContainer* objc = igroup->getObject(id);

			if (!objc->pvsCalculated) {
				uncalculatedObjCount--;
				objc->pvsCalculated = true;
			}
		}

		uint32_t numGroupSects = reader->readU32();

		resolvedGroups.insert(CString::from(relPath));

		for (uint32_t j = 0 ; j < numGroupSects ; j++) {
			uint32_t sectID = reader->readU32();

			PVSSection* sect = sections[sectID];

			PVSSection::SetIterator sit;
			for (sit = sect->getVisibilitySetBegin() ; sit != sect->getVisibilitySetEnd() ; sit++) {
				PVSVisibilitySet* set = *sit;

				uint32_t numObjs = reader->readU32();

				for (uint32_t l = 0 ; l < numObjs ; l++) {
					uint32_t id = reader->readU32();

					PVSSceneObjectContainer* objc = igroup->getObject(id);
					set->addPotentiallyVisibleObject(objc);
				}
			}
		}
	}

	delete[] depUpToDateTable;

	return ResultOK;
}


PVSDatabase::LoadingResult PVSDatabase::load(const File& file, const File& rootDir, int flags)
{
	istream* stream = file.openInputOutputStream(istream::binary | istream::in);
	LoadingResult res = load(stream, rootDir, flags);
	delete stream;
	return res;
}


void PVSDatabase::calculatePVS(unsigned int numThreads)
{
	if (!sections)
		calculateSections(500.0f, 500.0f, 2000.0f);

	if (uncalculatedObjCount == 0)
		return;

	Mutex* mutex = new Mutex;
	SectionCalculatorThread* threads = new SectionCalculatorThread[numThreads];

	list<PVSSceneObjectContainer*> uncalculatedObjs;

	for (list<PVSSceneObjectContainer*>::iterator it = objects.begin() ; it != objects.end() ; it++) {
		PVSSceneObjectContainer* objc = *it;

		if (!objc->pvsCalculated) {
			uncalculatedObjs.push_back(objc);
		}
	}

	uint32_t nextSect = 0;

	for (unsigned int i = 0 ; i < numThreads ; i++) {
		threads[i].mutex = mutex;
		threads[i].nextSect = &nextSect;
		threads[i].pvs = this;
		threads[i].beg = uncalculatedObjs.begin();
		threads[i].end = uncalculatedObjs.end();
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

	for (unsigned int i = 0 ; i < numThreads ; i++) {
		threads[i].join();
	}

	list<PVSSceneObjectContainer*>::iterator it;
	for (it = uncalculatedObjs.begin() ; it != uncalculatedObjs.end() ; it++) {
		PVSSceneObjectContainer* objc = *it;
		objc->pvsCalculated = true;
	}

	uncalculatedObjCount = 0;

	delete[] threads;
	delete mutex;
}


void PVSDatabase::SectionCalculatorThread::run()
{
	mutex->lock();

	while (*nextSect < pvs->numSects) {
		PVSSection* sect = pvs->sections[*nextSect];
		(*nextSect)++;

		mutex->unlock();

		unsigned int i = 0;

		for (list<PVSSceneObjectContainer*>::iterator it = beg ; it != end ; it++, i++) {
			PVSSceneObjectContainer* objc = *it;

			if (objc->pvsCalculated)
				continue;

			PVSSceneObject* obj = objc->obj;
			PVSSceneObject* baseObj = obj;

			PVSSection::SetIterator sit;
			for (sit = sect->getVisibilitySetBegin() ; sit != sect->getVisibilitySetEnd() ; sit++) {
				PVSVisibilitySet* set = *sit;

				Vector3 pos = obj->getPosition();

				float ox = pos.getX();
				float oy = pos.getY();
				float oz = pos.getZ();
				float dd = obj->getStreamingDistance() * set->getDistanceMultiplier();

				if (dd == 0.0f  ||  sect->intersectsSphere(ox, oy, oz, dd)) {
					set->addPotentiallyVisibleObject(objc);
				}
			}
		}

		mutex->lock();
	}

	mutex->unlock();
}


void PVSDatabase::calculateSections(float sectSizeX, float sectSizeY, float sectSizeZ)
{
	// Calculate the world visibility bounding box
	float bx1 = 0.0f;
	float by1 = 0.0f;
	float bz1 = 0.0f;
	float bx2 = 0.0f;
	float by2 = 0.0f;
	float bz2 = 0.0f;

	list<PVSSceneObjectContainer*>::iterator it;

	for (it = objects.begin() ; it != objects.end() ; it++) {
		PVSSceneObject* object = (*it)->obj;

		Vector3 pos = object->getPosition();
		float ox = pos.getX();
		float oy = pos.getY();
		float oz = pos.getZ();
		float dd = object->getStreamingDistance();

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

				PVSVisibilitySet* set1 = new PVSVisibilitySet(1.0f);
				section->addVisibilitySet(set1);

				PVSVisibilitySet* set2 = new PVSVisibilitySet(2.0f);
				section->addVisibilitySet(set2);

				//PVSVisibilitySet* set5 = new PVSVisibilitySet(5.0f);
				//section->addVisibilitySet(set5);

				sections[i++] = section;
			}
		}
	}
}


void PVSDatabase::InternalSceneObjectFileGroup::addSceneObject(PVSSceneObjectContainer* objc)
{
	SceneObjectDefinitionInfo* info = objc->obj->getDefinitionInfo();

	if (info->getFileGroup() != fg) {
		throw EngineException("SceneObjectFileGroup::addSceneObject() called with object of different "
				"file group!", __FILE__, __LINE__);
	}

	if (!objs.insert(pair<uint32_t, PVSSceneObjectContainer*>(info->getID(), objc)).second) {
		throw EngineException("ID collision in SceneObjectFileGroup::addSceneObject()!", __FILE__, __LINE__);
	}

	objc->obj->getDefinitionInfo()->markAsFixed();
}


PVSDatabase::PVSSceneObjectContainer* PVSDatabase::InternalSceneObjectFileGroup::getObject(uint32_t id)
{
	ObjectIterator it = objs.find(id);

	if (it == objs.end()) {
		return NULL;
	}

	return it->second;
}


PVSDatabase::InternalSceneObjectFileGroup* PVSDatabase::getFileGroup(const CString& relPath)
{
	FileGroupIterator it = fileGroups.find(relPath);

	if (it == fileGroups.end()) {
		return NULL;
	}

	return it->second;
}

