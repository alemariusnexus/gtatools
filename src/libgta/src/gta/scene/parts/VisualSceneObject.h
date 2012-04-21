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

#ifndef VISUALSCENEOBJECT_H_
#define VISUALSCENEOBJECT_H_

#include "SceneObject.h"
#include <gtaformats/util/math/Matrix4.h>


class VisualSceneObject : public virtual SceneObject {
public:
	virtual void setHasAlphaTransparency(bool alpha) = 0;
	virtual bool hasAlphaTransparency() const = 0;
	virtual Matrix4& getModelMatrix() = 0;
	virtual const Matrix4& getModelMatrix() const = 0;
	virtual void setModelMatrix(const Matrix4& matrix) = 0;
	virtual void setLODParent(VisualSceneObject* parent) = 0;
	virtual SceneObject* getLODParent() = 0;
	virtual typeflags_t getTypeFlags() const { return TypeFlagVisual; }
};

#endif /* VISUALSCENEOBJECT_H_ */
