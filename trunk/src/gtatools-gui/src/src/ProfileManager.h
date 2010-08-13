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

#ifndef PROFILEMANAGER_H_
#define PROFILEMANAGER_H_

#include <QtCore/qlist.h>
#include "Profile.h"


class ProfileManager : public QObject {
	Q_OBJECT

public:
	typedef QList<Profile*>::iterator ProfileIterator;

public:
	static ProfileManager* getInstance();
	void loadProfiles();
	ProfileIterator getProfileBegin();
	ProfileIterator getProfileEnd();
	Profile* setCurrentProfile(Profile* profile);
	Profile* getCurrentProfile() { return currentProfile; }
	Profile* getProfile(int idx) { return profiles[idx]; }
	int indexOfProfile(Profile* profile) { return profiles.indexOf(profile); }
	void saveProfiles();

private:
	ProfileManager(QObject* parent = NULL);
	virtual ~ProfileManager();

signals:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);

private slots:
	void currentProfileChangedSlot(Profile* oldProfile, Profile* newProfile);

private:
	QList<Profile*> profiles;
	Profile* currentProfile;
};

#endif /* PROFILEMANAGER_H_ */
