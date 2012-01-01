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

#include "DisplayedFile.h"
#include "formats/FormatHandler.h"
#include "System.h"



DisplayedFile::DisplayedFile(FormatHandler* handler)
		: handler(handler), changes(false)
{
	System* sys = System::getInstance();

	connect(sys, SIGNAL(fileClosed(DisplayedFile*)), this, SLOT(fileClosed(DisplayedFile*)));
	connect(sys, SIGNAL(currentFileChanged(DisplayedFile*, DisplayedFile*)), this,
			SLOT(currentFileChanged(DisplayedFile*, DisplayedFile*)));
}


/*DisplayedFile::~DisplayedFile()
{
	printf("Destroying DisplayedFile\n");
}*/


QString DisplayedFile::getFormatName() const
{
	File file = getFile();
	return handler->getFormatName(&file);
}


void DisplayedFile::fileClosed(DisplayedFile* file)
{
	if (file == this)
		emit closed();
}


void DisplayedFile::currentFileChanged(DisplayedFile* current, DisplayedFile* prev)
{
	if (current == this  &&  prev != this)
		emit madeCurrent();
	else if (prev == this  &&  current != this)
		emit lostCurrent(current);
}
