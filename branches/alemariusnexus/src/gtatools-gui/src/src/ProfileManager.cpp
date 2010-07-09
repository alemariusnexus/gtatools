/*
 * ProfileManager.cpp
 *
 *  Created on: 03.06.2010
 *      Author: alemariusnexus
 */

#include "ProfileManager.h"
#include <QtXml/qdom.h>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include "config.h"


ProfileManager::ProfileManager(QObject* parent)
		: QObject(parent), currentProfile(NULL)
{
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


void ProfileManager::saveProfiles()
{
	QDomDocument doc;
	QDomElement profilesElem = doc.createElement("profiles");
	doc.appendChild(profilesElem);

	ProfileIterator it;

	for (it = profiles.begin() ; it != profiles.end() ; it++) {
		Profile* profile = *it;

		QDomElement profileElem = doc.createElement("profile");
		profileElem.setAttribute("name", profile->getName());
		profilesElem.appendChild(profileElem);

		Profile::ResourceIterator rit;
		for (rit = profile->getResourceBegin() ; rit != profile->getResourceEnd() ; rit++) {
			File* resource = *rit;

			QDomElement resourceElem = doc.createElement("resource");
			resourceElem.setNodeValue(resource->getPath()->toString());
			profileElem.appendChild(resourceElem);
		}
	}

	QFile file(QString(CONFIG_DIR).append("/profiles.xml"));
	QTextStream stream(&file);
	doc.save(stream, 4);
}


Profile* ProfileManager::setCurrentProfile(Profile* profile)
{
	Profile* oldProfile = currentProfile;
	currentProfile = profile;
	emit currentProfileChanged(oldProfile, profile);
	return oldProfile;
}


