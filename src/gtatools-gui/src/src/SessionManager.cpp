/*
 * SessionManager.cpp
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#include "SessionManager.h"
#include <wx/confbase.h>
#include <wx/string.h>
#include <wx/msgdlg.h>


SessionManager::SessionManager()
{
	wxConfigBase* config = wxConfigBase::Get();

	for (int i = 0 ; ; i++) {
		wxString psn = wxString::Format(wxT("/Profile%d"), i);

		if (!config->Exists(psn)) {
			break;
		}

		wxString profileName = config->Read(wxString::Format(wxT("%s/Name")));

		Profile* profile = new Profile(profileName);

		for (int j = 0 ; ; j++) {
			wxString resEntryName = wxString::Format(wxT("%s/Resource%d"), psn, j);

			if (!config->Exists(resEntryName)) {
				break;
			}

			wxString resource = config->Read(resEntryName);
			profile->addResource(resource);
		}

		profiles->push_back(profile);
	}

	long defaultProfile;
	config->Read(wxT("/Main/DefaultProfile"), &defaultProfile, 0);

	if (defaultProfile >= profiles.size()) {
		wxMessageBox(wxT("Index out of bounds for configuration value /Main/DefaultProfile. Loading profile 0"),
				wxT("Configuration error"), wxOK | wxICON_ERROR);
		defaultProfile = 0;
	}

	setCurrentProfile(profiles[defaultProfile]);
}


void SessionManager::setCurrentProfile(Profile* profile)
{
	this->profile = profile;

	if (resourceIndex != NULL) {
		delete resourceIndex;
	}

	resourceIndex = profile->createResourceIndex();
}


void SessionManager::startProfileIteration()
{
	currentIt = profiles.begin();
}


Profile* SessionManager::getNextProfile()
{
	if (currentIt == profiles.end()) {
		return NULL;
	}

	return *currentIt++;
}
