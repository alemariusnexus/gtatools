/*
 * Profile.cpp
 *
 *  Created on: 03.06.2010
 *      Author: alemariusnexus
 */

#include "Profile.h"
#include <cstdio>
#include <cstdlib>



Profile::Profile(const QString& name)
		: name(QString(name))
{
}


void Profile::addResource(const File& resource)
{
	resources << new File(resource);
}


Profile::ResourceIterator Profile::getResourceBegin()
{
	return resources.begin();
}


Profile::ResourceIterator Profile::getResourceEnd()
{
	return resources.end();
}


void Profile::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	if (oldProfile == this  &&  newProfile != this) {
		delete resourceIndex;
	} else if (oldProfile != this  &&  newProfile == this) {
		resourceIndex = new OpenGLResourceManager;

		ResourceIterator it;

		for (it = resources.begin() ; it != resources.end() ; it++) {
			File* resource = *it;
			resourceIndex->addResource(*resource);
		}
	}
}


Profile::ResourceIterator Profile::removeResource(ResourceIterator it)
{
	File* file = *it;
	ResourceIterator next = resources.erase(it);
	delete file;
	return next;
}


void Profile::clearResources()
{
	ResourceIterator it;

	for (it = getResourceBegin() ; it != getResourceEnd() ;) {
		it = removeResource(it);
	}
}


void Profile::synchronize()
{
	emit changed();
}


bool Profile::containsFile(const File& file)
{
	ResourceIterator it;
	for (it = getResourceBegin() ; it != getResourceEnd() ; it++) {
		File* res = *it;

		if (file.isChildOf(*res)) {
			return true;
		}
	}

	return false;
}

















Profile::Profile()
{
	fprintf(stderr, "UUUUUUUAAAAAARRRGGGHHH! Profile::Profile() should NEVER EVER be called. It's just here "
			"for compatibility with QVariant and Qt's meta type system! Program will exit now!");
	exit(1);
}


Profile::Profile(const Profile& other)
{
	fprintf(stderr, "UUUUUUUAAAAAARRRGGGHHH! Profile::Profile(const Profile&) should NEVER EVER be called. It's "
				"just here for compatibility with QVariant and Qt's meta type system! Program will exit now!");
	exit(1);
}
