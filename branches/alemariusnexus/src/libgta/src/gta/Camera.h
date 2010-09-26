/*
 * Camera.h
 *
 *  Created on: 05.09.2010
 *      Author: alemariusnexus
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <gtaformats/util/math/Vector3.h>



class Camera {
public:
	Camera(	const Vector3& position = Vector3(),
			const Vector3& target = Vector3(0.0f, -1.0f, 0.0f),
			const Vector3& up = Vector3(0.0f, 0.0f, 1.0f)
			/*const Vector3& target = Vector3(0.0f, 0.0f, -1.0f),
			const Vector3& up = Vector3(0.0f, 1.0f, 0.0f)*/
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
