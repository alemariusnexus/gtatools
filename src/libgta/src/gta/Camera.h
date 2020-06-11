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

#ifndef GTA_CAMERA_H_
#define GTA_CAMERA_H_

#include <gta/config.h>
#include <nxcommon/math/Vector3.h>
#include <nxcommon/math/Frustum.h>
#include <nxcommon/gl/Camera.h>
#include "scene/StreamingViewpoint.h"
#include "scene/StreamingManager.h"



namespace gta
{


class Camera : public nxcommon::Camera, public StreamingViewpoint {
public:
	Camera(const Frustum& frustum = Frustum(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			Vector3::Zero, Vector3::NegativeUnitY, Vector3::UnitZ))
			: nxcommon::Camera(frustum) {}

	virtual Vector3 getStreamingViewpointPosition() const { return getPosition(); }
	virtual uint32_t getStreamingFlags() const { return FrustumCulling; }
	//virtual uint32_t getStreamingFlags() const { return 0; }
	virtual uint32_t getBuckets() const { return StreamingManager::VisibleBucket | StreamingManager::PhysicsBucket; }
	virtual float getStreamingDistanceMultiplier() const { return 1.0f; }
	virtual Frustum getCullingFrustum() const { return frustum; }
};

}

#endif /* GTA_CAMERA_H_ */
