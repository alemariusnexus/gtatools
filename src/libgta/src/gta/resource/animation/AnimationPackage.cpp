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

#include "AnimationPackage.h"
#include <utility>

using std::pair;



AnimationPackage::~AnimationPackage()
{
	for (AnimMap::iterator it = anims.begin() ; it != anims.end() ; it++) {
		delete it->second;
	}
}


void AnimationPackage::addAnimation(const CString& name, Animation* anim)
{
	size += anim->guessSize();
	anims.insert(pair<CString, Animation*>(name, anim));
}


Animation* AnimationPackage::find(const CString& name)
{
	AnimMap::iterator it = anims.find(name);

	if (it == anims.end())
		return NULL;

	return it->second;
}
