/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "config.h"
#include "ProfileManager.h"
#include <QtCore/QSettings>
#include <QtCore/QString>
#include <gtaformats/util/File.h>
#include <gtaformats/util/OutOfBoundsException.h>
#include "System.h"



ProfileManager::ProfileManager(QObject* parent)
		: QObject(parent), currentProfile(NULL)
{
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


void ProfileManager::loadProfiles()
{
	QSettings settings;

	for (int i = 0 ; true ; i++) {
		if (!settings.contains(QString("profile%1/name").arg(i))) {
			break;
		}

		Profile* profile = new Profile(settings.value(QString("profile%1/name").arg(i)).toString());
		profile->setDATRootDirectory(settings.value(QString("profile%1/dat_root").arg(i)).toString());

		for (int j = 0 ; true ; j++) {
			if (!settings.contains(QString("profile%1/resource%2").arg(i).arg(j))) {
				break;
			}

			QString resource = settings.value(QString("profile%1/resource%2").arg(i).arg(j)).toString();
			profile->addResource(File(resource.toLocal8Bit().constData()));
		}

		for (int j = 0 ; true ; j++) {
			if (!settings.contains(QString("profile%1/dat_file%2").arg(i).arg(j))) {
				break;
			}

			QString datFile = settings.value(QString("profile%1/dat_file%2").arg(i).arg(j)).toString();
			profile->addDATFile(File(datFile.toLocal8Bit().constData()));
		}

		addProfile(profile);
	}

	int currentProfileIdx = settings.value("main/current_profile", -1).toInt();

	if (currentProfileIdx != -1) {
		setCurrentProfile(getProfile(currentProfileIdx));
	} else {
		setCurrentProfile(NULL);
	}

	emit profilesLoaded();
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
	sys->closeCurrentFile();

	Profile* oldProfile = currentProfile;
	currentProfile = profile;

	emit currentProfileChanged(oldProfile, profile);

	return oldProfile;
}


void ProfileManager::saveProfiles()
{
	QSettings settings;

	ProfileIterator it;
	int i;

	for (i = 0 ; true ; i++) {
		if (settings.contains(QString("profile%1/name").arg(i))) {
			settings.remove(QString("profile%1").arg(i));
		} else {
			break;
		}
	}

	i = 0;

	for (it = getProfileBegin() ; it != getProfileEnd() ; it++, i++) {
		Profile* profile = *it;
		settings.setValue(QString("profile%1/name").arg(i), profile->getName());
		settings.setValue(QString("profile%1/dat_root").arg(i), profile->getDATRootDirectory());

		Profile::ResourceIterator rit;

		int j;

		for (j = 0 ; true ; j++) {
			if (settings.contains(QString("profile%1/resource%2").arg(i).arg(j))) {
				settings.remove(QString("profile%1/resource%2").arg(i).arg(j));
			} else {
				break;
			}
		}

		for (j = 0 ; true ; j++) {
			if (settings.contains(QString("profile%1/dat_file%2").arg(i).arg(j))) {
				settings.remove(QString("profile%1/dat_file%2").arg(i).arg(j));
			} else {
				break;
			}
		}

		j = 0;

		for (rit = profile->getResourceBegin() ; rit != profile->getResourceEnd() ; rit++, j++) {
			settings.setValue(QString("profile%1/resource%2").arg(i).arg(j), (*rit)->getPath()->toString());
		}

		j = 0;

		for (rit = profile->getDATFilesBegin() ; rit != profile->getDATFilesEnd() ; rit++, j++) {
			settings.setValue(QString("profile%1/dat_file%2").arg(i).arg(j), (*rit)->getPath()->toString());
		}
	}
}


void ProfileManager::currentProfileChangedSlot(Profile* oldProfile, Profile* newProfile)
{
	QSettings settings;

	if (newProfile != NULL) {
		settings.setValue("main/current_profile", indexOfProfile(newProfile));
	} else {
		settings.setValue("main/current_profile", -1);
	}

	settings.sync();
}


void ProfileManager::addProfile(Profile* profile)
{
	profiles << profile;
	connect(profile, SIGNAL(changed()), this, SLOT(profileChangedSlot()));
	emit profileAdded(profile);
}


bool ProfileManager::removeProfile(Profile* profile)
{
	profiles.removeOne(profile);

	if (getCurrentProfile() == profile) {
		if (profiles.size() == 0) {
			setCurrentProfile(NULL);
		} else {
			setCurrentProfile(profiles[0]);
		}
	}

	disconnect(profile, SIGNAL(changed()), this, SLOT(profileChangedSlot()));
	emit profileRemoved(profile);

    return true;
}


void ProfileManager::setProfiles(const QList<Profile*>& profiles)
{
	QList<Profile*> currentProfiles = this->profiles;

	QList<Profile*>::const_iterator it;

	for (it = currentProfiles.begin() ; it != currentProfiles.end() ; it++) {
		Profile* profile = *it;

		if (!profiles.contains(profile)) {
			if (getCurrentProfile() == profile) {
				setCurrentProfile(NULL);
			}

			removeProfile(profile);
			delete profile;
		}
	}

	for (it = profiles.begin() ; it != profiles.end() ; it++) {
		Profile* profile = *it;

		if (!currentProfiles.contains(profile)) {
			addProfile(profile);
		}
	}
}


Profile* ProfileManager::getProfile(int idx)
{
	if (idx < 0  ||  idx >= profiles.size()) {
		throw OutOfBoundsException(idx, __FILE__, __LINE__);
	}

	return profiles[idx];
}


void ProfileManager::profileChangedSlot()
{
	Profile* profile = (Profile*) sender();
	emit profileChanged(profile);
}


