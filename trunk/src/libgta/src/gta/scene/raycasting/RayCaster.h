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

#ifndef RAYCASTER_H_
#define RAYCASTER_H_

#include <gta/config.h>
#include <nxcommon/math/Vector3.h>
#include "RayCastingHandler.h"



class RayCaster
{
public:
	enum Flags
	{
		CalculateIntersectionPosition = 1 << 0,
		Sorted = 1 << 1,
		OneHitPerObjectOnly = 1 << 2
	};

private:
	enum CastingResultType
	{
		Box,
		Sphere,
		Mesh
	};

	struct CastingResult
	{
		CastingResultType type;
		uint32_t objID;
		void* objUserPtr;
		void* userPtr;
		float rayR;

		RayCastingHandler::SphereResult sphereRes;
		RayCastingHandler::BoxResult boxRes;
		RayCastingHandler::MeshResult meshRes;
	};

	class CastingResultComparator
	{
	public:
		bool operator()(const CastingResult& r1, const CastingResult& r2) { return r1.rayR < r2.rayR; }
	};

public:
	RayCaster() : handler(NULL) {}

	void setHandler(RayCastingHandler* handler);

	void castRay(const Vector3& start, const Vector3& dir, int flags = 0, uint32_t maxResults = 0);

private:
	RayCastingHandler* handler;
};

#endif /* RAYCASTER_H_ */
