/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef PROFILEMANAGER_H_
#define PROFILEMANAGER_H_

#include <QList>
#include "Profile.h"


class ProfileManager : public QObject {
	Q_OBJECT

public:
	typedef QList<Profile*>::iterator ProfileIterator;

public:
	static ProfileManager* getInstance();
	static void destroy();

public:
	void loadProfiles();
	ProfileIterator getProfileBegin();
	ProfileIterator getProfileEnd();
	QList<Profile*> getProfiles() { return profiles; }
	Profile* setCurrentProfile(Profile* profile);
	Profile* getCurrentProfile() { return currentProfile; }
	Profile* getProfile(int idx);
	int getProfileCount() { return profiles.size(); }
	int indexOfProfile(Profile* profile) { return profiles.indexOf(profile); }
	void saveProfiles();
	void addProfile(Profile* profile);
	bool removeProfile(Profile* profile);
	void clearProfiles() { setProfiles(QList<Profile*>()); }
	void setProfiles(const QList<Profile*>& profiles);

private:
	ProfileManager(QObject* parent = NULL);
	virtual ~ProfileManager();

signals:
	void profilesLoaded();
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void profileAdded(Profile* profile);
	void profileRemoved(Profile* profile);

private slots:
	void currentProfileChangedSlot(Profile* oldProfile, Profile* newProfile);
	void eventLoopStarted();

private:
	static ProfileManager* instance;

private:
	QList<Profile*> profiles;
	Profile* currentProfile;
	bool destroying;
};

#endif /* PROFILEMANAGER_H_ */
