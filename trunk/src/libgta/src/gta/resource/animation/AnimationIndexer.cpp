/*
 * AnimationIndexer.cpp
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#include "AnimationIndexer.h"
#include <utility>

using std::pair;




AnimationIndexer::~AnimationIndexer()
{
	for (AnimMap::iterator it = index.begin() ; it != index.end() ; it++) {
		delete it->second;
	}
}


void AnimationIndexer::resourceAdded(const File& file)
{
	if (file.guessContentType() == CONTENT_TYPE_IFP) {
		const char* fname = file.getPath()->getFileName();
		size_t len = strlen(fname);
		CString lname(fname, len-4);
		lname.lower();
		index.insert(pair<CString, File*>(lname, new File(file)));
	}
}


File* AnimationIndexer::find(CString name)
{
	AnimMap::iterator it = index.find(name);

	if (it == index.end())
		return NULL;

	return it->second;
}
