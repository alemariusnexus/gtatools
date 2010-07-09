/*
 * ProfileManager.h
 *
 *  Created on: 03.06.2010
 *      Author: alemariusnexus
 */

#ifndef PROFILEMANAGER_H_
#define PROFILEMANAGER_H_

#include <QtCore/qlinkedlist.h>
#include "Profile.h"


class ProfileManager : public QObject {
	Q_OBJECT

public:
	typedef QLinkedList<Profile*>::iterator ProfileIterator;

public:
	static ProfileManager* getInstance();
	ProfileIterator getProfileBegin();
	ProfileIterator getProfileEnd();
	void saveProfiles();
	Profile* setCurrentProfile(Profile* profile);

private:
	ProfileManager(QObject* parent = NULL);
	virtual ~ProfileManager();

signals:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);

private:
	QLinkedList<Profile*> profiles;
	Profile* currentProfile;
};

#endif /* PROFILEMANAGER_H_ */
