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
#include <qmetatype.h>
#include <gtaformats/util/File.h>


class Profile : public QObject {
	Q_OBJECT

public:
	typedef QLinkedList<File*>::iterator ResourceIterator;

public:
	Profile(const QString& name);

	void addResource(const File& resource);
	ResourceIterator getResourceBegin();
	ResourceIterator getResourceEnd();
	QString getName() const { return name; }
	ResourceIterator removeResource(ResourceIterator it);
	void clearResources();
	void setName(const QString& name) { this->name = name; }
	void synchronize();
	bool containsFile(const File& file);

public slots:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);

signals:
	void changed();

private:
	QLinkedList<File*> resources;
	QString name;
	OpenGLResourceManager* resourceIndex;




public:
	/**	\brief Just here for compatibility with Qt's meta type system and QVariant.
	 *
	 * 	NEVER EVER CALL THIS CONSTRUCTOR!
	 */
	Profile();

	/**	\brief Just here for compatibility with Qt's meta type system and QVariant.
	 *
	 * 	NEVER EVER CALL THIS CONSTRUCTOR!
	 */
	Profile(const Profile& other);
};

//Q_DECLARE_METATYPE(Profile)

#endif /* PROFILE_H_ */
