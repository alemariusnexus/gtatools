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

#ifndef PVSDATABASE_H_
#define PVSDATABASE_H_

#include "PVSSection.h"
#include "../parts/SceneObject.h"
#include "../SceneObjectDefinitionDatabase.h"
#include "../StreamingViewpoint.h"
#include "../SceneObjectDefinitionInfo.h"
#include "../parts/PVSSceneObject.h"
#include <vector>
#include <nxcommon/util.h>
#include <nxcommon/file/File.h>
#include <nxcommon/ProgressObserver.h>
#include <istream>
#include <ostream>
#include <algorithm>
#include <utility>
#include <list>
#include <thread>
#include <mutex>

using std::istream;
using std::ostream;
using std::vector;
using std::distance;
using std::pair;
using std::list;
using std::thread;
using std::mutex;






class PVSDatabase
{
public:
	enum LoadFlags
	{
		LoadValidateByCRC = (1 << 0)
	};

	enum SaveFlags
	{
		SaveForceLittleEndian = (1 << 0),
		SaveForceBigEndian = (1 << 1)
	};

	enum LoadingResult
	{
		ResultOK,
		ResultInvalidFormat,
		ResultWrongVersion
	};

private:
	struct PVSSceneObjectContainer
	{
		PVSSceneObject* obj;
		bool pvsCalculated;
	};


	class InternalSceneObjectFileGroup
	{
	public:
		typedef map<uint32_t, PVSSceneObjectContainer*> ObjectMap;
		typedef ObjectMap::iterator ObjectIterator;
		typedef ObjectMap::const_iterator ConstObjectIterator;

	public:
		InternalSceneObjectFileGroup(SceneObjectFileGroup* fg) : fg(fg) {}
		SceneObjectFileGroup* getFileGroup() { return fg; }
		void addSceneObject(PVSSceneObjectContainer* obj);
		PVSSceneObjectContainer* getObject(uint32_t id);
		ObjectIterator getObjectBegin() { return objs.begin(); }
		ObjectIterator getObjectEnd() { return objs.end(); }
		ConstObjectIterator getObjectBegin() const { return objs.begin(); }
		ConstObjectIterator getObjectEnd() const { return objs.end(); }
		size_t getObjectCount() const { return objs.size(); }

	private:
		SceneObjectFileGroup* fg;
		ObjectMap objs;
	};


	class SectionCalculatorThread
	{
	public:
		virtual void run();

	public:
		thread thr;
		mutex* mtx;
		uint32_t* nextSect;
		PVSDatabase* pvs;
		list<PVSSceneObjectContainer*>::iterator beg, end;
	};

	typedef map<CString, InternalSceneObjectFileGroup*> FileGroupMap;
	typedef FileGroupMap::iterator FileGroupIterator;
	typedef FileGroupMap::const_iterator ConstFileGroupIterator;

public:
	class PVSSceneObjectIterator
	{
	public:
		PVSSceneObjectIterator() {}
		PVSSceneObjectIterator(vector<PVSSceneObjectContainer*>::iterator it) : it(it) {}
		PVSSceneObjectIterator(const PVSSceneObjectIterator& other) : it(other.it) {}

		PVSSceneObjectIterator& operator++() { it++; return *this; }
		PVSSceneObjectIterator operator++(int) { PVSSceneObjectIterator oldIt = *this; it++; return oldIt; }
		bool operator==(const PVSSceneObjectIterator& other) const { return it == other.it; }
		bool operator!=(const PVSSceneObjectIterator& other) const { return it != other.it; }
		PVSSceneObject* operator*() { return (*it)->obj; }

	private:
		vector<PVSSceneObjectContainer*>::iterator it;
	};

public:
	PVSDatabase();

	void calculatePVS(unsigned int numThreads = 1);

	template <class ItType>
	void addObjects(ItType beg, ItType end);

	void addObject(PVSSceneObject* obj)
			{ addObjects<PVSSceneObject**>(&obj, &obj + 1); }

	void setSections(PVSSection** sects, uint32_t numSects)
			{ sections = sects; this->numSects = numSects; }

	void calculateSections(float sectSizeX, float sectSizeY, float sectSizeZ);

	LoadingResult load(istream* stream, const File& rootDir, int flags = LoadValidateByCRC);

	LoadingResult load(const File& file, const File& rootDir, int flags = LoadValidateByCRC);

	void save(ostream* out, int flags = 0);

	void save(const File& file, int flags = 0);

	/*template <class ContainerType>
	bool queryPVS(ContainerType& pvs, const Vector3& pos, float distMultiplier,
			float* chosenDistMultiplier = NULL);*/

	bool queryPVS(PVSSceneObjectIterator& beg, PVSSceneObjectIterator& end, const Vector3& pos,
			float distMultiplier, float* chosenDistMultiplier = NULL);

	void addProgressObserver(ProgressObserver* obsv) { progressObservers.push_back(obsv); }

	void removeProgressObserver(ProgressObserver* obsv);

	size_t getUncalculatedObjectCount() const { return uncalculatedObjCount; }

private:
	PVSSection* findSection(float x, float y, float z);
	InternalSceneObjectFileGroup* getFileGroup(const CString& relPath);

private:
	list<PVSSceneObjectContainer*> objects;
	FileGroupMap fileGroups;
	vector<ProgressObserver*> progressObservers;
	PVSSection** sections;
	uint32_t numSects;
	SceneObjectDefinitionDatabase defDB;
	size_t uncalculatedObjCount;


	friend class SceneObjectFileGroup;
	friend class PVSVisibilitySet;
	//friend class SectionCalculatorThread;
};










template <class ItType>
void PVSDatabase::addObjects(ItType beg, ItType end)
{
	for (ItType it = beg ; it != end ; it++) {
		PVSSceneObject* obj = dynamic_cast<PVSSceneObject*>(*it);

		if (!obj)
			continue;

		SceneObjectDefinitionInfo* info = obj->getDefinitionInfo();

		PVSSceneObject* baseObj = obj;

		uncalculatedObjCount++;

		PVSSceneObjectContainer* cont = new PVSSceneObjectContainer;
		cont->obj = obj;
		cont->pvsCalculated = false;

		if (obj == baseObj)
			objects.push_back(cont);

		if (info) {
			SceneObjectFileGroup* group = info->getFileGroup();
			InternalSceneObjectFileGroup* igroup = NULL;

			FileGroupIterator it;
			for (it = fileGroups.begin() ; it != fileGroups.end() ; it++) {
				InternalSceneObjectFileGroup* itigroup = it->second;

				if (itigroup->getFileGroup() == group) {
					igroup = itigroup;
					break;
				}
			}

			if (!igroup) {
				igroup = new InternalSceneObjectFileGroup(group);
				fileGroups.insert(pair<CString, InternalSceneObjectFileGroup*>(
						group->getRelativePath(), igroup));
			}

			igroup->addSceneObject(cont);
		}
	}
}

#endif /* PVSDATABASE_H_ */
