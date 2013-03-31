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

#ifndef ANIMATEDMAPSCENEOBJECT_H_
#define ANIMATEDMAPSCENEOBJECT_H_

#include "../parts/AnimatedSceneObject.h"
#include "MapSceneObject.h"
#include "../../AnimatedMapItemDefinition.h"


class AnimatedMapSceneObject : public virtual MapSceneObject, public virtual AnimatedSceneObject
{
public:
	AnimatedMapSceneObject();
	AnimatedMapSceneObject(const AnimatedMapSceneObject& other);
	virtual void setAnimationTime(float t) { time = t; }
	virtual float getAnimationTime() const { return time; }
	virtual void setCurrentAnimation(const CString& name) { curAnim = name; }
	virtual CString getCurrentAnimation() const;
	virtual bool isAutoAnimationEnabled() const { return autoAnim; }
	void setAutoAnimationEnabled(bool aa) { autoAnim = aa; }
	void setAutoPickDefaultAnimation(bool ap) { autoPickDefaultAnim = ap; }
	bool isAutoPickDefaultAnimation() const { return autoPickDefaultAnim; }

	virtual typeflags_t getTypeFlags() const
			{ return TypeFlagVisual | TypeFlagPVS | TypeFlagRigidBody | TypeFlagAnimated; }

private:
	CString curAnim;
	float time;
	bool autoAnim;
	bool autoPickDefaultAnim;
};

#endif /* ANIMATEDMAPSCENEOBJECT_H_ */
