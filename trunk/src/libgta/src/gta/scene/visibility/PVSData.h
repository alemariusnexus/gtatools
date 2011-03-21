/*
 * PVSData.h
 *
 *  Created on: 20.03.2011
 *      Author: alemariusnexus
 */

#ifndef PVSDATA_H_
#define PVSDATA_H_

#include "PVSSection.h"
#include "../DefaultSceneObject.h"
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
	void queryPVS(float x, float y, float z, vector<DefaultSceneObject*>& pvs);
	void serialize(ostream* out);
	void serialize(const File& file);
	bool unserialize(istream* in);
	bool unserialize(const File& file);
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
