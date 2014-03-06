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

#ifndef CAMERA_H_
#define CAMERA_H_

#include <gta/config.h>
#include <gtaformats/util/math/Vector3.h>
#include <gtaformats/util/math/Frustum.h>
#include "scene/StreamingViewpoint.h"
#include "scene/StreamingManager.h"



class Camera : public StreamingViewpoint {
public:
	/*Camera(	const Vector3& position = Vector3(),
			const Vector3& target = Vector3(0.0f, -1.0f, 0.0f),
			const Vector3& up = Vector3(0.0f, 0.0f, 1.0f)
	) : position(position), target(target), up(up) {}*/
	Camera(const Frustum& frustum = Frustum(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			Vector3::Zero, Vector3::NegativeUnitY, Vector3::UnitZ))
			: frustum(frustum) {}
	const Vector3& getPosition() const { return frustum.getPosition(); }
	const Vector3& getTarget() const { return frustum.getFrontDirection(); }
	const Vector3& getUp() const { return frustum.getUpDirection(); }
	void setPosition(const Vector3& pos) { frustum.setPosition(pos); }
	void setPosition(float x, float y, float z) { setPosition(Vector3(x, y, z)); }
	void lookAt(const Vector3& target, const Vector3& up) { frustum.setDirection(target, up); }
	void rotateHorizontal(float angle);
	void rotateVertical(float angle);
	void move(float length);
	void moveSideways(float length);
	void moveUp(float length);
	const Frustum& getFrustum() const { return frustum; }
	Frustum& getFrustum() { return frustum; }
	void setFrustum(const Frustum& f) { frustum = f; }

	virtual Vector3 getStreamingViewpointPosition() const { return getPosition(); }
	virtual uint32_t getStreamingFlags() const { return FrustumCulling; }
	//virtual uint32_t getStreamingFlags() const { return 0; }
	virtual uint32_t getBuckets() const { return StreamingManager::VisibleBucket | StreamingManager::PhysicsBucket; }
	virtual float getStreamingDistanceMultiplier() const { return 1.0f; }
	virtual Frustum getCullingFrustum() const { return frustum; }

private:
	//Vector3 position, target, up;
	Frustum frustum;
};

#endif /* CAMERA_H_ */
