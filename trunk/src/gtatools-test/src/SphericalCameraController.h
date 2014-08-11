/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-test.

	gtatools-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef SPHERICALCAMERACONTROLLER_H_
#define SPHERICALCAMERACONTROLLER_H_

#include <gta/Camera.h>
#include <nxcommon/math/Vector3.h>



class SphericalCameraController
{
public:
	SphericalCameraController();
	void apply();

	void setCenter(const Vector3& center);
	void setRadius(float radius);
	void setAzimuth(float azimuth);
	void setInclination(float inclination);

	void rotateAzimuth(float azimuth);
	void rotateInclination(float inclination);
	void move(float radius);

private:
	Vector3 center;
	float radius;
	float azimuth;
	float inclination;
};

#endif /* SPHERICALCAMERACONTROLLER_H_ */
