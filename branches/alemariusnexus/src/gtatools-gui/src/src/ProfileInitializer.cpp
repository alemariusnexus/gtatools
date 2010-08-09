/*
 * ProfileInitializer.cpp
 *
 *  Created on: 01.08.2010
 *      Author: alemariusnexus
 */

#include "ProfileInitializer.h"
#include "Profile.h"
#include <cstdio>
#include "System.h"



ProfileInitializer::ProfileInitializer(Profile* profile)
		: profile(profile)
{
}


void ProfileInitializer::run()
{
	Profile::ResourceIterator it;
	ResourceIndex* rm = profile->getResourceManager();

	int numRes = 0;
	for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++, numRes++) {
		rm->addResource(**it);
		//sys->updateTaskValue(numRes);
	}
}

