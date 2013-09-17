/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "DisplayedEntity.h"
#include "formats/FormatHandler.h"
#include "System.h"



DisplayedEntity::DisplayedEntity()
		: changes(false)
{
	System* sys = System::getInstance();

	connect(sys, SIGNAL(entityClosed(DisplayedEntity*)), this, SLOT(entityClosed(DisplayedEntity*)));
	connect(sys, SIGNAL(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)), this,
			SLOT(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)));
}


bool DisplayedEntity::save(bool useLast)
{
	if (doSave(useLast)) {
		emit saved();
		setHasChanges(false);
		return true;
	}

	return false;
}


void DisplayedEntity::entityClosed(DisplayedEntity* ent)
{
	if (ent == this)
		emit closed();
}


void DisplayedEntity::currentEntityChanged(DisplayedEntity* current, DisplayedEntity* prev)
{
	if (current == this  &&  prev != this)
		emit madeCurrent();
	else if (prev == this  &&  current != this)
		emit lostCurrent(current);
}
