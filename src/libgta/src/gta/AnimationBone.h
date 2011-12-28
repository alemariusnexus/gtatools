/*
 * AnimationBone.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef ANIMATIONBONE_H_
#define ANIMATIONBONE_H_

#include <gta/config.h>
#include "AnimationFrame.h"
#include "Engine.h"
#include <vector>
#include <gtaformats/ifp/IFPObject.h>
#include <gtaformats/util/math/Matrix4.h>

using std::vector;



class AnimationBone {
public:
	enum Flags
	{
		FrameHasTranslation = (1 << 0)
	};

private:
	typedef vector<AnimationFrame*> FrameList;
	typedef FrameList::iterator FrameIterator;
	typedef FrameList::const_iterator ConstFrameIterator;

public:
	AnimationBone(int flags = 0);
	AnimationBone(const AnimationBone& other);
	AnimationBone(const IFPObject* obj);
	~AnimationBone();
	FrameIterator getFrameBegin() { return frames.begin(); }
	ConstFrameIterator getFrameBegin() const { return frames.begin(); }
	FrameIterator getFrameEnd() { return frames.end(); }
	ConstFrameIterator getFrameEnd() const { return frames.end(); }
	size_t getFrameCount() const { return frames.size(); }
	bool getFrames(float time, AnimationFrame*& f1, AnimationFrame*& f2, float& t) const;
	AnimationFrame* getInterpolatedFrame(float time) const;
	Matrix4 getInterpolatedFrameMatrix(float time) const;
	size_t guessSize() const { return frames.size() * 8 * sizeof(float) + sizeof(int) + sizeof(FrameList); }
	const char* getID() const { return id; }

private:
	int flags;
	FrameList frames;
	char* id;
};

#endif /* ANIMATIONBONE_H_ */
