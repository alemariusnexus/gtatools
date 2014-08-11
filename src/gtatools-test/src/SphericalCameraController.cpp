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

#include "SphericalCameraController.h"
#include <gta/Engine.h>
#include <nxcommon/math/Matrix4.h>
#include <cmath>
#include <algorithm>

using std::min;
using std::max;




SphericalCameraController::SphericalCameraController()
		: center(Vector3::Zero), radius(10.0f), azimuth(0.0f), inclination(M_PI_2)
{
}


void SphericalCameraController::apply()
{
	Camera* cam = Engine::getInstance()->getCamera();

	Vector3 pos (
			center.getX() + radius * cos(azimuth) * sin(inclination),
			center.getY() + radius * sin(azimuth) * sin(inclination),
			center.getZ() + radius * cos(inclination)
			);

	Vector3 target = center - pos;
	target.normalize();

	Vector3 targetProjOnXY = target;
	targetProjOnXY.setZ(0.0f);

	Vector3 up;

	if (fabs(target.getZ()) > 0.9999f) {
		// This is nearly the +Z (or -Z) vector. In this case, we must approximate the up vector from
		// the azimuth.

		if (target.getZ() < 0.0f) {
			up = Vector3(-cos(azimuth), -sin(azimuth), 0.0f);
		} else {
			up = Vector3(cos(azimuth), sin(azimuth), 0.0f);
		}
	} else {
		Vector3 side = target.cross(Vector3::UnitZ);
		side.normalize();
		up = Matrix4::rotation(M_PI_2, side) * target;
	}

	cam->setPosition(pos);
	cam->lookAt(target, up);
}


void SphericalCameraController::setCenter(const Vector3& center)
{
	this->center = center;
}


void SphericalCameraController::setRadius(float radius)
{
	this->radius = radius;
}


void SphericalCameraController::setAzimuth(float azimuth)
{
	this->azimuth = fmodf(azimuth, 2.0f*M_PI);
}


void SphericalCameraController::setInclination(float inclination)
{
	//this->inclination = fmodf(inclination, M_PI);
	this->inclination = min(max(0.0f, inclination), (float) M_PI);
}


void SphericalCameraController::rotateAzimuth(float azimuth)
{
	setAzimuth(this->azimuth + azimuth);
}


void SphericalCameraController::rotateInclination(float inclination)
{
	setInclination(this->inclination + inclination);
}


void SphericalCameraController::move(float radius)
{
	this->radius = max(0.0f, this->radius + radius);
}
