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

#ifndef PVSDATA_H_
#define PVSDATA_H_

#include "PVSSection.h"
#include "../StaticSceneObject.h"
#include <vector>
#include <gtaformats/util/File.h>
#include <gtaformats/util/ProgressObserver.h>
#include <istream>
#include <ostream>

using std::istream;
using std::ostream;
using std::vector;


class Scene;


const uint32_t PVS_VERSION_DEVELOPMENT = 0x0;


class PVSData
{
public:
	PVSData(Scene* scene);
	void build();
	void queryPVS(float x, float y, float z, list<StaticSceneObject*>& pvs);
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
	int numSects;
	float sectSizeX, sectSizeY, sectSizeZ;
};

#endif /* PVSDATA_H_ */
