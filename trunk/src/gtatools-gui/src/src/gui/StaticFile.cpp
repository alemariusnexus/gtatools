/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "StaticFile.h"
#include <cstdio>
#include <cstring>



StaticFile::StaticFile(const File& file)
		: file(file), parent(NULL), childrenInited(false)
{
	if (file.isDirectory()) {
		type = Directory;
	} else if (file.isArchiveDirectory()) {
		type = Archive;
	} else {
		type = Node;
	}
}


StaticFile::StaticFile(const File& file, StaticFile* parent, Type type)
		: file(file), parent(parent), childrenInited(false)
{
	if (type != Unknown) {
		this->type = type;
	} else {
		if (file.isDirectory()) {
			this->type = Directory;
		} else if (file.isArchiveDirectory()) {
			this->type = Archive;
		} else {
			this->type = Node;
		}
	}
}


StaticFile::~StaticFile()
{
	ChildList::iterator it;

	for (it = children.begin() ; it != children.end() ; it++) {
		delete *it;
	}
}


void StaticFile::loadChildren()
{
	if (type != Node) {
		if (type != Archive) {
			for (File child : file.getChildren()) {
				children << new StaticFile(child, this);
			}
		} else {
			for (File child : file.getChildren()) {
				children << new StaticFile(child, this, Node);
			}
		}
	}

	childrenInited = true;
}


void StaticFile::ensureChildrenAvailable()
{
	if (!childrenInited) {
		loadChildren();
	}
}


