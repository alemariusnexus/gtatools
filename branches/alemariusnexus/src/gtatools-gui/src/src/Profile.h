/*
 * Profile.h
 *
 *  Created on: 03.06.2010
 *      Author: alemariusnexus
 */

#ifndef PROFILE_H_
#define PROFILE_H_

#include <QtCore/qlinkedlist.h>
#include <QtCore/qlist.h>
#include <QtCore/qstring.h>
#include <QtCore/qfile.h>
#include <gtaformats/engine/OpenGLResourceManager.h>


class Profile {
public:
	typedef QLinkedList<File*>::iterator ResourceIterator;

public:
	Profile(const QString& name);

	void addResource(const File& resource);
	ResourceIterator getResourceBegin();
	ResourceIterator getResourceEnd();
	QString getName() const { return name; }

public slots:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);

private:
	QLinkedList<File*> resources;
	QString name;
	OpenGLResourceManager* resourceIndex;
};

#endif /* PROFILE_H_ */
