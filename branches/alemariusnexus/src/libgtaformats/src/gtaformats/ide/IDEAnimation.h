/*
 * IDEAnimation.h
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#ifndef IDEANIMATION_H_
#define IDEANIMATION_H_

#include "../gf_config.h"
#include "IDEEntity.h"

#define IDE_TYPE_ANIMATION 2



class IDEAnimation : public IDEEntity {
public:
	IDEAnimation(int32_t id, const char* modelName, const char* texName, const char* animName,
			float drawDist, int32_t flags);
	~IDEAnimation();

	const char* getModelName() { return modelName; }
	const char* getTextureName() { return texName; }
	const char* getAnimationName() { return animName; }
	float getDrawDist() { return drawDist; }
	int32_t getFlags() { return flags; }

	virtual idetype_t getType() const { return IDE_TYPE_ANIMATION; }

private:
	char* modelName;
	char* texName;
	char* animName;
	float drawDist;
	int32_t flags;
};

#endif /* IDEANIMATION_H_ */
