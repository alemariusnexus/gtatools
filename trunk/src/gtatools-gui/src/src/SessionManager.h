/*
 * SessionManager.h
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#ifndef SESSIONMANAGER_H_
#define SESSIONMANAGER_H_

#include "Profile.h"
#include <gtaformats/engine.h>
#include <vector>

using std::vector;


class SessionManager {
public:
	static SessionManager* getInstance()
	{
		static SessionManager* inst = new SessionManager;
		return inst;
	}
	static SessionManager* init() { return getInstance(); }

	Profile* getCurrentProfile() { return profile; }
	void setCurrentProfile(Profile* profile);
	ResourceIndex* getResourceIndex() { return resourceIndex; }
	void startProfileIteration();
	Profile* getNextProfile();

private:
	SessionManager();

private:
	Profile* profile;
	ResourceIndex* resourceIndex;
	vector<Profile*> profiles;
	vector<Profile*>::iterator currentIt;
};

#endif /* SESSIONMANAGER_H_ */
