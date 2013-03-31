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

#ifndef SCENEOBJECTDEFINITIONINFO_H_
#define SCENEOBJECTDEFINITIONINFO_H_

#include "SceneObjectFileGroup.h"


class SceneObjectDefinitionInfo {
public:
	SceneObjectDefinitionInfo(SceneObjectFileGroup* group, uint32_t id);
	SceneObjectDefinitionInfo(const SceneObjectDefinitionInfo& other)
			: group(other.group), id(other.id), fixed(other.fixed) {}
	SceneObjectFileGroup* getFileGroup() { return group; }
	uint32_t getID() const { return id; }
	void setFileGroup(SceneObjectFileGroup* group) { this->group = group; }
	void setID(uint32_t id) { this->id = id; }
	void markAsFixed() { fixed = true; }
	bool isFixed() const { return fixed; }

private:
	SceneObjectFileGroup* group;
	uint32_t id;
	bool fixed;
};

#endif /* SCENEOBJECTDEFINITIONINFO_H_ */
