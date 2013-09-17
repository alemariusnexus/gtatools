/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef ANIMATIONPACKAGE_H_
#define ANIMATIONPACKAGE_H_

#include "../../Animation.h"
#include "../ResourceCache.h"
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/CString.h>
#include <map>

using std::map;



class AnimationPackage {
private:
	typedef map<CString, Animation*> AnimMap;

public:
	typedef AnimMap::iterator AnimIterator;
	typedef AnimMap::const_iterator ConstAnimIterator;

public:
	AnimationPackage() : size(sizeof(AnimMap) + sizeof(cachesize_t)) {}
	~AnimationPackage();
	cachesize_t getSize() const { return size; }
	void addAnimation(const CString& name, Animation* anim);
	Animation* find(const CString& name);
	Animation* operator[](const CString& name) { return find(name); }
	AnimIterator getAnimationBegin() { return anims.begin(); }
	AnimIterator getAnimationEnd() { return anims.end(); }
	ConstAnimIterator getAnimationBegin() const { return anims.begin(); }
	ConstAnimIterator getAnimationEnd() const { return anims.end(); }
	size_t getAnimationCount() const { return anims.size(); }

private:
	AnimMap anims;
	cachesize_t size;
};

#endif /* ANIMATIONPACKAGE_H_ */
