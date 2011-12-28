/*
 * AnimationIndexer.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef ANIMATIONINDEXER_H_
#define ANIMATIONINDEXER_H_

#include "../ResourceObserver.h"
#include <map>
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/CString.h>
#include <gtaformats/util/File.h>

using std::map;



class AnimationIndexer : public ResourceObserver {
private:
	typedef map<CString, File*> AnimMap;

public:
	~AnimationIndexer();
	virtual void resourceAdded(const File& file);
	File* find(CString name);
	File* operator[](CString name) { return find(name); }

private:
	AnimMap index;
};

#endif /* ANIMATIONINDEXER_H_ */
