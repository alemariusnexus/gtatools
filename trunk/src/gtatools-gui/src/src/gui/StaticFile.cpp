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

#include "StaticFile.h"



StaticFile::StaticFile()
		: file(NULL), parent(NULL)
{
}


StaticFile::StaticFile(const File& file)
		: file(new File(file)), parent(NULL)
{
	initChildren();
}


StaticFile::StaticFile(File* file, StaticFile* parent)
		: file(file), parent(parent)
{
	initChildren();
}


void StaticFile::initChildren()
{
	if (file->isDirectory()  ||  file->isArchiveFile()) {
		FileIterator* it = file->getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			children << new StaticFile(child, this);
		}

		delete it;
	}
}


void StaticFile::addRootChild(StaticFile* child)
{
	children << child;
	child->parent = this;
}


