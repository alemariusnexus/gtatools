/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
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


void DFFFrame::scale(float x, float y, float z)
{
	translation[0] *= x;
	translation[1] *= y;
	translation[2] *= z;
}


