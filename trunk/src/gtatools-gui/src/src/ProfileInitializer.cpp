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
	try {
		Profile::ResourceIterator it;
		ResourceIndex* rm = profile->getResourceManager();

		System* sys = System::getInstance();
		Task* task = sys->createTask();
		task->start(tr("Generating resource index..."));

		for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
			File* file = *it;
			rm->addResource(File(*file));
		}

		delete task;
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}

