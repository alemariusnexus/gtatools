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

#include "Camera.h"
#include <gtaformats/util/math/Matrix4.h>



void Camera::rotateHorizontal(float angle)
{
	Matrix4 rot = Matrix4::rotationZ(angle);
	//Matrix4 rot = Matrix4::rotationY(angle);
	target = rot * target;
	up = rot * up;
}


void Camera::rotateVertical(float angle)
{
	Vector3 side = up.cross(target);
	target = Matrix4::rotation(angle, side) * target;
	up = target.cross(side);
}


void Camera::move(float length)
{
	target.normalize();
	position += target * length;
}


void Camera::moveSideways(float length)
{
	Vector3 side = up.cross(target);
	target.normalize();
	position += side * length;
}


void Camera::moveUp(float length)
{
	target.normalize();
	position += up * length;
}
