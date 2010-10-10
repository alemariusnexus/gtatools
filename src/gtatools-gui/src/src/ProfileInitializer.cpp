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

#include "ProfileInitializer.h"
#include "Profile.h"
#include <cstdio>
#include "System.h"
#include <ctime>



ProfileInitializer::ProfileInitializer(Profile* profile)
		: profile(profile), interrupted(false)
{
}


void ProfileInitializer::run()
{
	try {
		Profile::ResourceIterator it;
		ResourceManager* rm = profile->getResourceManager();

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
	ResourceManager* rm = profile->getResourceManager();

	if (interrupted) {
		return;
	}

	if (file.isDirectory()  ||  file.isDirectory()) {
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

