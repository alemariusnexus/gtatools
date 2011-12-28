/*
 * SceneObjectDefinitionInfo.h
 *
 *  Created on: 22.10.2011
 *      Author: alemariusnexus
 */

#ifndef SCENEOBJECTDEFINITIONINFO_H_
#define SCENEOBJECTDEFINITIONINFO_H_

#include "SceneObjectFileGroup.h"


class SceneObjectDefinitionInfo {
public:
	SceneObjectDefinitionInfo(SceneObjectFileGroup* group, uint32_t id);
	SceneObjectDefinitionInfo(const SceneObjectDefinitionInfo& other)
			: group(other.group), id(other.id), lodLeaf(other.lodLeaf) {}
	SceneObjectFileGroup* getFileGroup() { return group; }
	uint32_t getID() const { return id; }
	void setFileGroup(SceneObjectFileGroup* group) { this->group = group; }
	void setID(uint32_t id) { this->id = id; }
	void setLODLeaf(bool leaf) { lodLeaf = leaf; }
	bool isLODLeaf() const { return lodLeaf; }

private:
	SceneObjectFileGroup* group;
	uint32_t id;
	bool lodLeaf;
};

#endif /* SCENEOBJECTDEFINITIONINFO_H_ */
