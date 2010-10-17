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
