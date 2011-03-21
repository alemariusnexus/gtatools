#include "intersection.h"


bool IntersectAABoxSphere(float sx, float sy, float sz, float sr,
		float bx1, float by1, float bz1,
		float bx2, float by2, float bz2)
{
	float distSq = 0;

	if (sx < bx1) {
		distSq += (bx1-sx) * (bx1-sx);
	} else if (sx > bx2) {
		distSq += (sx-bx2) * (sx-bx2);
	}

	if (sy < by1) {
		distSq += (by1-sy) * (by1-sy);
	} else if (sy > by2) {
		distSq += (sy-by2) * (sy-by2);
	}

	if (sz < bz1) {
		distSq += (bz1-sz) * (bz1-sz);
	} else if (sz > bz2) {
		distSq += (sz-bz2) * (sz-bz2);
	}

	return distSq <= sr*sr;
}
