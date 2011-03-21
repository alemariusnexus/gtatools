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
#include <istream>
#include <ostream>

using std::istream;
using std::ostream;


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

private:
	PVSSection* findSection(float x, float y, float z);

private:
	Scene* scene;
	PVSSection** sections;
	int numSects;
};

#endif /* PVSDATA_H_ */
