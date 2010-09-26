/*
 * Camera.cpp
 *
 *  Created on: 05.09.2010
 *      Author: alemariusnexus
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
