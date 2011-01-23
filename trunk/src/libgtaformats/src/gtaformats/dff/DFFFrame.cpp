/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DFFFrame.h"
#include "DFFException.h"
#include "../util/OutOfBoundsException.h"
#include <cstring>



DFFFrame::DFFFrame(const DFFFrame& other)
		: name(new char[strlen(other.name)+1]), translation(new Vector3(*other.translation)),
		  rotation(new Matrix3(*other.rotation)), flags(other.flags)
{
	strcpy(name, other.name);
}


DFFFrame::~DFFFrame()
{
	if (name != NULL) {
		delete[] name;
	}

	ChildIterator it;

	for (it = getChildBegin() ; it != getChildEnd() ; it++) {
		(*it)->reparent(NULL);
		delete *it;
	}

	delete rotation;
	delete translation;
}


void DFFFrame::mirrorYZ()
{
	translation->mirrorYZ();
}


void DFFFrame::scale(float x, float y, float z)
{
	translation->scale(x, y, z);
}


DFFFrame* DFFFrame::getChild(const char* name)
{
	ChildIterator it;

	for (it = children.begin() ; it != children.end() ; it++) {
		if (strcmp((*it)->getName(), name) == 0) {
			return *it;
		}
	}

	return NULL;
}


const DFFFrame* DFFFrame::getChild(const char* name) const
{
	ConstChildIterator it;

	for (it = children.begin() ; it != children.end() ; it++) {
		if (strcmp((*it)->getName(), name) == 0) {
			return *it;
		}
	}

	return NULL;
}


void DFFFrame::removeChild(DFFFrame* child)
{
	ChildIterator it;

	for (it = children.begin() ; it != children.end() ; it++) {
		if (*it == child) {
			(*it)->reparent(NULL);
			children.erase(it);
		}
	}
}


void DFFFrame::removeChildren()
{
	ChildIterator it;

	for (it = children.begin() ; it != children.end() ; it++) {
		(*it)->reparent(NULL);
	}

	children.clear();
}


int32_t DFFFrame::indexOf(const DFFFrame* child) const
{
	ConstChildIterator it;
	int i = 0;

	for (it = children.begin() ; it != children.end() ; it++, i++) {
		if (*it == child) {
			return i;
		}
	}

	return -1;
}


DFFFrame* DFFFrame::getChild(uint32_t index)
{
	if (index < 0  ||  index >= children.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return children[index];
}


const DFFFrame* DFFFrame::getChild(uint32_t index) const
{
	if (index < 0  ||  index >= children.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return children[index];
}


void DFFFrame::reparent(DFFFrame* frame)
{
	if (frame && parent) {
		throw DFFException("Attempt to reparent a DFFFrame which still has a parent! Remove it from "
						"it's old parent first.", __FILE__, __LINE__);
	}

	parent = frame;
}

