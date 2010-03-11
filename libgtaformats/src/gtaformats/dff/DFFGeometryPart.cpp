/*
 * DFFGeometryPart.cpp
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
 */

#include "DFFGeometryPart.h"


DFFGeometryPart::~DFFGeometryPart()
{
	delete[] indices;
	// We don't delete the material because it's part of the whole geometry. Possibly more geometry parts can
	// share one material.
}
