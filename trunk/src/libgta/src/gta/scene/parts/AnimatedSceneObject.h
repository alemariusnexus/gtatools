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

#ifndef ANIMATEDSCENEOBJECT_H_
#define ANIMATEDSCENEOBJECT_H_

#include "VisualSceneObject.h"
#include "../../AnimatedMapItemDefinition.h"
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/util/CString.h>


class AnimatedSceneObject : public virtual VisualSceneObject {
public:
	virtual void setAnimationTime(float t) = 0;
	virtual float getAnimationTime() const = 0;
	virtual void setCurrentAnimation(const CString& name) = 0;
	virtual CString getCurrentAnimation() const = 0;
	virtual void increaseAnimationTime(float t) { setAnimationTime(getAnimationTime() + t); }
	virtual typeflags_t getTypeFlags() const { return TypeFlagAnimated | TypeFlagVisual; }
};

#endif /* ANIMATEDSCENEOBJECT_H_ */
