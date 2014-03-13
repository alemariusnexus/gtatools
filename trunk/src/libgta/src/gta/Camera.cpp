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

#include "Camera.h"
#include <nxcommon/math/Matrix4.h>



void Camera::rotateHorizontal(float angle)
{
	Matrix4 rot = Matrix4::rotationZ(angle);
	Vector3 target = rot * getTarget();
	Vector3 up = rot * getUp();
	target.normalize();
	up.normalize();
	frustum.setDirection(target, up);
}


void Camera::rotateVertical(float angle)
{
	Vector3 side = getUp().cross(getTarget());
	Vector3 target = Matrix4::rotation(angle, side) * getTarget();
	Vector3 up = getTarget().cross(side);
	target.normalize();
	up.normalize();
	frustum.setDirection(target, up);
}


void Camera::move(float length)
{
	setPosition(getPosition() + getTarget() * length);
}


void Camera::moveSideways(float length)
{
	Vector3 side = getUp().cross(getTarget());
	side.normalize();
	setPosition(getPosition() + side * length);
}


void Camera::moveUp(float length)
{
	setPosition(getPosition() + getUp() * length);
}
