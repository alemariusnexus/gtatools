/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "IDEFileFinder.h"
#include <gtaformats/ide/IDEReader.h>
#include <gtaformats/ide/IDEStaticObject.h>
#include <gtaformats/ide/IDETimedObject.h>
#include <gtaformats/ide/IDEAnimation.h>
#include <gtaformats/ide/IDEPedestrian.h>
#include <gtaformats/ide/IDEWeapon.h>



IDEFileFinder::IDEFileFinder(int32_t id, idetype_t types, FileFinder* meshFinder, FileFinder* txdFinder)
		: id(id), types(types), meshFinder(meshFinder), txdFinder(txdFinder)
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

		if ((type & types) == 0) {
			delete stmt;
			continue;
		}

		if (id != -1) {
			if ((type & IDETypeGroupEntity)  !=  0) {
				IDEEntity* entity = (IDEEntity*) stmt;
				int32_t id = entity->getID();

				if (id != this->id) {
					delete stmt;
					continue;
				}
			} else {
				delete stmt;
				continue;
			}
		}

		if (meshFinder != NULL) {
			if (type == IDETypeStaticObject  ||  type == IDETypeTimedObject) {
				IDEStaticObject* sobj = (IDEStaticObject*) stmt;

				if (!meshFinder->matches(File(sobj->getModelName()))) {
					delete stmt;
					continue;
				}
			} else {
				delete stmt;
				continue;
			}
		}

		if (txdFinder != NULL) {
			if (type == IDETypeStaticObject  ||  type == IDETypeTimedObject) {
				IDEStaticObject* sobj = (IDEStaticObject*) stmt;

				if (!txdFinder->matches(File(sobj->getTXDArchiveName()))) {
					delete stmt;
					continue;
				}
			} else {
				delete stmt;
				continue;
			}
		}

		fileLines[file] = ide.getLastReadLine();

		delete stmt;
		return true;
	}

	return false;
}
