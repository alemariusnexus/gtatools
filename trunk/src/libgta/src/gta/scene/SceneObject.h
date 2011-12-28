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

#ifndef SCENEOBJECT_H_
#define SCENEOBJECT_H_

#include <gtaformats/util/math/Vector3.h>



enum SceneObjectType
{
	SceneObjectInvalid,
	SceneObjectStatic,
	SceneObjectAnimated
};


class SceneObject {
public:
	virtual ~SceneObject() {}
	virtual int getType() const = 0;
	virtual bool isVisible() const { return true; }
	virtual float getDrawDistance() const = 0;
	virtual Vector3 getPosition() const = 0;
	virtual SceneObject* getLODParent() = 0;
	virtual int getLODHierarchyDepth() const
			{ SceneObject* p = getLODParent(); return p ? p->getLODHierarchyDepth()+1 : 0; }
	SceneObject* getLODParent() const { return const_cast<SceneObject*>(this)->getLODParent(); }
};

#endif /* SCENEOBJECT_H_ */
