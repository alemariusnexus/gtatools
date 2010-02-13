/*
 * DFFFrame.cpp
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
 */

#include "DFFFrame.h"


DFFFrame::~DFFFrame()
{
	if (name != NULL) {
		delete[] name;
	}
}


void DFFFrame::mirrorYZ()
{
	float y = translation[1];
	translation[1] = translation[2];
	translation[2] = y;
}
