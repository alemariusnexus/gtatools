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

#ifndef PVSDATABASE_H_
#define PVSDATA_HBASE_

#include "PVSSection.h"
#include "../Scene.h"
#include "../StaticSceneObject.h"
#include "../SceneObjectDefinitionDatabase.h"
#include <vector>
#include <gtaformats/util/File.h>
#include <gtaformats/util/ProgressObserver.h>
#include <gtaformats/util/thread/Thread.h>
#include <gtaformats/util/thread/Mutex.h>
#include <istream>
#include <ostream>

using std::istream;
using std::ostream;
using std::vector;






class PVSDatabase
{
public:
	enum LoadFlags
	{
		LoadValidateByModifyTime = (1 << 0)
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
	class SectionCalculatorThread : public Thread
	{
	public:
		virtual void run();

	public:
		Mutex* mutex;
		uint32_t* nextSect;
		PVSDatabase* pvs;
		Scene::ObjectIterator beg, end;
	};

public:
	PVSDatabase();
	void buildSections(Scene::ObjectIterator beg, Scene::ObjectIterator end);
	void calculatePVS(Scene::ObjectIterator beg, Scene::ObjectIterator end, unsigned int numThreads = 1);
	LoadingResult load(istream* stream, SceneObjectDefinitionDatabase* defDB, Scene::ObjectList& missingObjs,
			const File& rootDir, int flags = LoadValidateByModifyTime);
	LoadingResult load(const File& file, SceneObjectDefinitionDatabase* defDB,
			Scene::ObjectList& missingObjs, const File& rootDir, int flags = LoadValidateByModifyTime);
	void save(ostream* out, SceneObjectDefinitionDatabase* defDB, int flags = 0);
	void save(const File& file, SceneObjectDefinitionDatabase* defDB, int flags = 0);
	void queryPVS(float x, float y, float z, list<SceneObject*>& pvs);
	void setSectionSize(float x, float y, float z) { sectSizeX = x; sectSizeY = y; sectSizeZ = z; }
	void getSectionSize(float& x, float& y, float& z) const { x = sectSizeX; y = sectSizeY; z = sectSizeZ; }
	void addProgressObserver(ProgressObserver* obsv) { progressObservers.push_back(obsv); }
	void removeProgressObserver(ProgressObserver* obsv);

private:
	PVSSection* findSection(float x, float y, float z);

private:
	vector<ProgressObserver*> progressObservers;
	Scene* scene;
	PVSSection** sections;
	uint32_t numSects;
	float sectSizeX, sectSizeY, sectSizeZ;


	friend class SectionCalculatorThread;
};

#endif /* PVSDATABASE_H_ */
