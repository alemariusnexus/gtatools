/*
 * AnimationFrame.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef ANIMATIONFRAME_H_
#define ANIMATIONFRAME_H_

#include <gta/config.h>
#include <gtaformats/util/math/Vector3.h>
#include <gtaformats/util/math/Quaternion.h>
#include <gtaformats/ifp/IFPFrame.h>
#include <gtaformats/ifp/IFPRootFrame.h>


class AnimationFrame {
public:
	AnimationFrame(float start, const Quaternion& rot, const Vector3& trans = Vector3());
	AnimationFrame(const AnimationFrame& other);
	AnimationFrame(const IFPFrame* frame);
	AnimationFrame(const IFPRootFrame* frame);
	float getStart() const { return start; }
	Quaternion getRotation() const { return rot; }
	Vector3 getTranslation() const { return trans; }

private:
	float start;
	Quaternion rot;
	Vector3 trans;
};

#endif /* ANIMATIONFRAME_H_ */
