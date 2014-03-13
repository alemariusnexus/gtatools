/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include <nxcommon/math/Vector3.h>
#include <nxcommon/math/Matrix4.h>


class SceneObject {
public:
	typedef uint64_t typeflags_t;

	enum TypeFlags {
		TypeFlagVisual = (1 << 0),
		TypeFlagAnimated = (1 << 1),
		TypeFlagPVS = (1 << 2),
		TypeFlagLight = (1 << 3),
		TypeFlagRigidBody = (1 << 4)
	};

public:
	SceneObject() : visibleBuckets(0), lastVisibleBuckets(0) {}
	virtual ~SceneObject() {}
	virtual SceneObject* clone() const = 0;
	virtual typeflags_t getTypeFlags() const = 0;
	virtual bool isEnabled() const { return true; }
	virtual float getStreamingDistance() const = 0;
	virtual uint32_t getStreamingBuckets() const = 0;
	virtual Matrix4& getModelMatrix() = 0;
	virtual const Matrix4& getModelMatrix() const = 0;
	virtual void setModelMatrix(const Matrix4& matrix) = 0;

	virtual Vector3 getPosition() const
	{
		const float* a = getModelMatrix().toArray();
		return Vector3(a[12], a[13], a[14]);
	}

private:
	uint32_t lastVisibleBuckets;
	uint32_t visibleBuckets;

private:
	friend class Scene;
	friend class StreamingManager;
};

#endif /* SCENEOBJECT_H_ */
