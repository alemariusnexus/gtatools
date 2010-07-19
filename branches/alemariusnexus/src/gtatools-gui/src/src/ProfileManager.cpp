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

#include "ProfileManager.h"
#include "config.h"
#include <qsettings.h>
#include <qstring.h>
#include <gtaformats/util/File.h>
#include "System.h"



ProfileManager::ProfileManager(QObject* parent)
		: QObject(parent), currentProfile(NULL)
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	for (int i = 0 ; true ; i++) {
		if (!settings.contains(QString("profile%1/name").arg(i))) {
			break;
		}

		Profile* profile = new Profile(settings.value(QString("profile%1/name").arg(i)).toString());

		for (int j = 0 ; true ; j++) {
			if (!settings.contains(QString("profile%1/resource%2").arg(i).arg(j))) {
				break;
			}

			QString resource = settings.value(QString("profile%1/resource%2").arg(i).arg(j)).toString();
			profile->addResource(File(resource.toLocal8Bit().constData()));
		}

		profiles << profile;
	}

	setCurrentProfile(getProfile(settings.value("main/current_profile").toInt()));

	connect(this, SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChangedSlot(Profile*, Profile*)));
}


ProfileManager::~ProfileManager()
{
}


ProfileManager* ProfileManager::getInstance()
{
	static ProfileManager* inst = new ProfileManager;
	return inst;
}


ProfileManager::ProfileIterator ProfileManager::getProfileBegin()
{
	return profiles.begin();
}


ProfileManager::ProfileIterator ProfileManager::getProfileEnd()
{
	return profiles.end();
}


Profile* ProfileManager::setCurrentProfile(Profile* profile)
{
	System* sys = System::getInstance();
	sys->startTask(0, 1, "Opening profile...");

	Profile* oldProfile = currentProfile;
	currentProfile = profile;
	emit currentProfileChanged(oldProfile, profile);

	sys->updateTaskValue(1);
	sys->endTask();

	return oldProfile;
}


void ProfileManager::saveProfiles()
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	ProfileIterator it;
	int i = 0;

	for (it = getProfileBegin() ; it != getProfileEnd() ; it++, i++) {
		Profile* profile = *it;
		settings.setValue(QString("profile%1/name").arg(i), profile->getName());

		Profile::ResourceIterator rit;

		int j;

		for (j = 0 ; true ; j++) {
			if (settings.contains(QString("profile%1/resource%2").arg(i).arg(j))) {
				settings.remove(QString("profile%1/resource%2").arg(i).arg(j));
			} else {
				break;
			}
		}

		j = 0;

		for (rit = profile->getResourceBegin() ; rit != profile->getResourceEnd() ; rit++, j++) {
			settings.setValue(QString("profile%1/resource%2").arg(i).arg(j), (*rit)->getPath()->toString());
		}
	}
}


void ProfileManager::currentProfileChangedSlot(Profile* oldProfile, Profile* newProfile)
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);
	settings.setValue("main/current_profile", indexOfProfile(newProfile));
	settings.sync();
}



