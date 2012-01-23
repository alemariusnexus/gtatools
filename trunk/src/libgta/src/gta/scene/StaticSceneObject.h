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

#ifndef STATICSCENEOBJECT_H_
#define STATICSCENEOBJECT_H_

#include "VisualSceneObject.h"
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/gtaipl.h>
#include "../MapItemDefinition.h"
#include <btBulletDynamicsCommon.h>


class SceneObjectDefinitionInfo;



class StaticSceneObject : public VisualSceneObject {
public:
	StaticSceneObject(MapItemDefinition* def, const Matrix4& modelMatrix = Matrix4(),
			StaticSceneObject* lodParent = NULL);
	StaticSceneObject(const StaticSceneObject& other);
	SceneObject* clone() const { return new StaticSceneObject(*this); }
	MapItemDefinition* getDefinition() { return def; }
	const MapItemDefinition* getDefinition() const { return def; }
	void setHasAlphaTransparency(bool alpha) { this->alpha = alpha; }
	void setDefinitionInfo(SceneObjectDefinitionInfo* info) { defInfo = info; }
	SceneObjectDefinitionInfo* getDefinitionInfo() { return defInfo; }
	virtual bool isVisible() const { return def->isVisible(); }
	virtual int getType() const { return SceneObjectStatic; }
	virtual float getDrawDistance() const { return def->getDrawDistance(); }
	virtual Vector3 getPosition() const
			{ const float* m = modelMatrix.toArray(); return Vector3(m[12], m[13], m[14]); }
	virtual bool hasAlphaTransparency() const { return alpha; }

private:
	int flags;
	bool alpha;
	MapItemDefinition* def;
	SceneObjectDefinitionInfo* defInfo;
};

#endif /* STATICSCENEOBJECT_H_ */
