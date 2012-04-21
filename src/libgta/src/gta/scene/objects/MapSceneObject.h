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

#ifndef MAPSCENEOBJECT_H_
#define MAPSCENEOBJECT_H_

#include "../parts/VisualSceneObject.h"
#include "../parts/PVSSceneObject.h"
#include "../../MapItemDefinition.h"


class MapSceneObject : public virtual VisualSceneObject, public virtual PVSSceneObject
{
public:
	MapSceneObject(MapItemDefinition* def);
	MapSceneObject(const MapSceneObject& other);

	virtual SceneObject* clone() const { return new MapSceneObject(*this); }
	virtual bool isEnabled() const
			{ return def->isVisible()  &&  enabled; }
	virtual float getStreamingDistance() const { return def->getDrawDistance(); }
	virtual Vector3 getPosition() const
			{ const float* a = mm.toArray(); return Vector3(a[12], a[13], a[14]); }
	virtual SceneObject* getLODParent() { return lodParent; }
	virtual void setLODParent(VisualSceneObject* parent) { lodParent = parent; }

	virtual void setHasAlphaTransparency(bool alpha) {}
	virtual bool hasAlphaTransparency() const { return def->hasAlphaTransparency(); }
	virtual Matrix4& getModelMatrix() { return mm; }
	virtual const Matrix4& getModelMatrix() const { return mm; }
	virtual void setModelMatrix(const Matrix4& matrix) { mm = matrix; }

	virtual SceneObjectDefinitionInfo* getDefinitionInfo() { return defInfo; }

	virtual typeflags_t getTypeFlags() const { return TypeFlagVisual | TypeFlagPVS; }

	virtual void setEnabled(bool enabled) { this->enabled = enabled; }
	void setDefinitionInfo(SceneObjectDefinitionInfo* info) { defInfo = info; }
	virtual MapItemDefinition* getDefinition() { return def; }

private:
	MapItemDefinition* def;
	bool enabled;
	Matrix4 mm;
	SceneObject* lodParent;
	SceneObjectDefinitionInfo* defInfo;
};

#endif /* MAPSCENEOBJECT_H_ */
