/*
 * IDEFileFinder.cpp
 *
 *  Created on: 15.10.2010
 *      Author: alemariusnexus
 */

#include "IDEFileFinder.h"
#include <gtaformats/ide/IDEReader.h>
#include <gtaformats/ide/IDEStaticObject.h>
#include <gtaformats/ide/IDETimedObject.h>
#include <gtaformats/ide/IDEAnimation.h>
#include <gtaformats/ide/IDEPedestrian.h>
#include <gtaformats/ide/IDEWeapon.h>



IDEFileFinder::IDEFileFinder(int32_t id)
		: id(id)
{
}


bool IDEFileFinder::matches(const File& file)
{
	if (file.guessContentType() != CONTENT_TYPE_IDE) {
		return false;
	}

	IDEReader ide(file);
	IDEStatement* stmt;

	while ((stmt = ide.readStatement())  !=  NULL) {
		idetype_t type = stmt->getType();

		if (		type == IDE_TYPE_STATIC_OBJECT
				||  type == IDE_TYPE_TIMED_OBJECT
				||  type == IDE_TYPE_ANIMATION
				||  type == IDE_TYPE_PEDESTRIAN
				||  type == IDE_TYPE_WEAPON
		) {
			IDEEntity* entity = (IDEEntity*) stmt;
			int32_t id = entity->getId();

			if (id == this->id) {
				delete stmt;
				fileLines[file] = ide.getLastReadLine();
				return true;
			}
		}

		delete stmt;
	}

	return false;
}
