/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#ifndef CAMERA_H_
#define CAMERA_H_

#include <gtaformats/util/math/Vector3.h>



class Camera {
public:
	Camera(	const Vector3& position = Vector3(),
			const Vector3& target = Vector3(0.0f, -1.0f, 0.0f),
			const Vector3& up = Vector3(0.0f, 0.0f, 1.0f)
	) : position(position), target(target), up(up) {}
	const Vector3& getPosition() const { return position; }
	Vector3& getPosition() { return position; }
	const Vector3& getTarget() const { return target; }
	Vector3& getTarget() { return target; }
	const Vector3& getUp() const { return up; }
	Vector3& getUp() { return up; }
	void setPosition(const Vector3& pos) { position = pos; }
	void lookAt(const Vector3& target, const Vector3& up) { this->target = target; this->up = up; }
	void rotateHorizontal(float angle);
	void rotateVertical(float angle);
	void move(float length);
	void moveSideways(float length);
	void moveUp(float length);

private:
	Vector3 position, target, up;
};

#endif /* CAMERA_H_ */
