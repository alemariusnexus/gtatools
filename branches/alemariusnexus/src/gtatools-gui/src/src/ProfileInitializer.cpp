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
		: profile(profile), interrupted(false)
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
			addResource(*file);

			if (interrupted) {
				break;
			}
		}

		delete task;
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void ProfileInitializer::addResource(const File& file)
{
	ResourceIndex* rm = profile->getResourceManager();

	if (interrupted) {
		return;
	}

	if (file.isDirectory()) {
		FileIterator* it = file.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			addResource(*child);
			delete child;

			if (interrupted) {
				return;
			}
		}

		delete it;
	} else {
		rm->addResource(file);
	}
}

