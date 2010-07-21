/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Profile.h"
#include <cstdio>
#include <cstdlib>
#include "ProfileManager.h"



Profile::Profile(const QString& name)
		: name(QString(name)), resourceIndex(NULL)
{
	connect(ProfileManager::getInstance(), SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
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
	printf("Current profile changed");

	if (oldProfile == this  &&  newProfile != this) {
		delete resourceIndex;
	} else if (oldProfile != this  &&  newProfile == this) {
		resourceIndex = new OpenGLResourceManager;
		resourceIndex->setTextureRasterFormat(OpenGLResourceManager::R8G8B8A8);

		ResourceIterator it;

		printf("Adding resources\n");

		for (it = resources.begin() ; it != resources.end() ; it++) {
			File* resource = *it;
			resourceIndex->addResource(*resource);
		}

		printf("End resources\n");

		resourceIndex->bindTexture("jizzy");
		printf("Täxtscher baund\n");
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
