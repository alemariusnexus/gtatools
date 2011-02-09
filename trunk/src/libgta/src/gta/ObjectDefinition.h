/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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
 */

#ifndef OBJECTDEFINITION_H_
#define OBJECTDEFINITION_H_

#include "config.h"

class ObjectDefinition {
public:
	virtual void draw() = 0;
	void setDrawDistance(float dd) { drawDist = dd; }
	float getDrawDistance() { return drawDist; }
	void setBounds(float x, float y, float z, float r) { bounds[0] = x; bounds[1] = y; bounds[2] = z; bounds[3] = r; }
	float* getBounds() { return bounds; }

private:
	float drawDist;
	float bounds[4];
};

#endif /* OBJECTDEFINITION_H_ */
