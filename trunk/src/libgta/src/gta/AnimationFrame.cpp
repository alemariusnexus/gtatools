/*
 * AnimationFrame.cpp
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#include "AnimationFrame.h"



AnimationFrame::AnimationFrame(float start, const Quaternion& rot, const Vector3& trans)
		: start(start), rot(rot), trans(trans)
{
}


AnimationFrame::AnimationFrame(const AnimationFrame& other)
		: start(other.start), rot(other.rot), trans(other.trans)
{
}


AnimationFrame::AnimationFrame(const IFPFrame* frame)
		: start(frame->getTime()), rot(frame->getRotation())
{
}


AnimationFrame::AnimationFrame(const IFPRootFrame* frame)
		: start(frame->getTime()), rot(frame->getRotation()), trans(frame->getTranslation())
{
}
