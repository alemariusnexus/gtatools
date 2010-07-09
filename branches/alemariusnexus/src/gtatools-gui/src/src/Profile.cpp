/*
 * Profile.cpp
 *
 *  Created on: 03.06.2010
 *      Author: alemariusnexus
 */

#include "Profile.h"



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
