/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef MAPSCENEOBJECTLODINSTANCE_H_
#define MAPSCENEOBJECTLODINSTANCE_H_

#include "../../MapItemDefinition.h"


class MapSceneObject;


class MapSceneObjectLODInstance
{
public:
	MapSceneObjectLODInstance(MapItemDefinition* def) : def(def), obj(NULL) {}
	MapSceneObjectLODInstance(const MapSceneObjectLODInstance& other)
			: def(other.def), obj(NULL), mm(other.mm) {}
	float getStreamingDistance() const { return def->getDrawDistance(); }
	MapItemDefinition* getDefinition() { return def; }
	const MapItemDefinition* getDefinition() const { return def; }
	virtual bool hasAlphaTransparency() const { return def->hasAlphaTransparency(); }
	MapSceneObject* getSceneObject() { return obj; }
	void setRelativeModelMatrix(const Matrix4& mat);
	Matrix4& getRelativeModelMatrix() { return relativeMM; }
	const Matrix4& getRelativeModelMatrix() const { return relativeMM; }
	Matrix4& getModelMatrix() { updateModelMatrix(); return mm; }
	//const Matrix4& getModelMatrix() const { return mm; }

private:
	void updateModelMatrix();

private:
	MapItemDefinition* def;
	MapSceneObject* obj;
	Matrix4 relativeMM;
	Matrix4 mm;

	friend class MapSceneObject;

public:
	int32_t debugID;
};

#endif /* MAPSCENEOBJECTLODINSTANCE_H_ */
