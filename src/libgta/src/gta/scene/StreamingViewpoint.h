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

#ifndef STREAMINGVIEWPOINT_H_
#define STREAMINGVIEWPOINT_H_

#include <nxcommon/math/Vector3.h>
#include <nxcommon/math/Frustum.h>



class StreamingViewpoint
{
public:
	enum StreamingFlags
	{
		FrustumCulling = 1 << 0
	};

public:
	virtual Vector3 getStreamingViewpointPosition() const = 0;
	virtual uint32_t getBuckets() const = 0;
	virtual uint32_t getStreamingFlags() const = 0;
	virtual float getStreamingDistanceMultiplier() const = 0;
	virtual Frustum getCullingFrustum() const { return Frustum(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f); }
};

#endif /* STREAMINGVIEWPOINT_H_ */
