#ifndef RAYCASTER_H_
#define RAYCASTER_H_

#include <gta/config.h>
#include <gtaformats/util/math/Vector3.h>
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
