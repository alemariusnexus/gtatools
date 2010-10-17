/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
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
	virtual ~IDEAnimation();

	const char* getModelName() const { return modelName; }
	const char* getTextureName() const { return texName; }
	const char* getAnimationName() const { return animName; }
	float getDrawDist() const { return drawDist; }
	int32_t getFlags() const { return flags; }

	virtual idetype_t getType() const { return IDE_TYPE_ANIMATION; }

private:
	char* modelName;
	char* texName;
	char* animName;
	float drawDist;
	int32_t flags;
};

#endif /* IDEANIMATION_H_ */
