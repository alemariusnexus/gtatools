/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "DFFFrame.h"
#include "DFFException.h"
#include "../util/OutOfBoundsException.h"
#include <cstring>



DFFFrame::DFFFrame(const DFFFrame& other)
		: name(other.name), modelMatrix(new Matrix4(*other.modelMatrix)),
		  ltm(other.ltm ? new Matrix4(*other.ltm) : NULL), flags(other.flags), bone(new DFFBone(*other.bone))
{
}


DFFFrame::~DFFFrame()
{
	ChildIterator it;

	for (it = getChildBegin() ; it != getChildEnd() ; it++) {
		(*it)->reparent(NULL);
		delete *it;
	}

	delete modelMatrix;

	if (ltm)
		delete ltm;
}


DFFFrame* DFFFrame::getChild(const CString& name)
{
	ChildIterator it;

	for (it = children.begin() ; it != children.end() ; it++) {
		if ((*it)->getName() == name) {
			return *it;
		}
	}

	return NULL;
}


const DFFFrame* DFFFrame::getChild(const CString& name) const
{
	ConstChildIterator it;

	for (it = children.begin() ; it != children.end() ; it++) {
		if ((*it)->getName() == name) {
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
	if (index >= children.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return children[index];
}


const DFFFrame* DFFFrame::getChild(uint32_t index) const
{
	if (index >= children.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return children[index];
}


DFFFrame* DFFFrame::getChildByBoneID(int32_t id, bool recurse)
{
	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		DFFFrame* child = *it;

		if (child->getBone()  &&  child->getBone()->getIndex() == id)
			return child;

		if (recurse) {
			child = child->getChildByBoneID(id, true);

			if (child)
				return child;
		}
	}

	return NULL;
}


void DFFFrame::reparent(DFFFrame* frame)
{
	if (frame && parent) {
		throw DFFException("Attempt to reparent a DFFFrame which still has a parent! Remove it from "
						"it's old parent first.", __FILE__, __LINE__);
	}

	parent = frame;
}


void DFFFrame::ensureValidLTM()
{
	if (!ltm) {
		if (parent) {
			ltm = new Matrix4(*modelMatrix * parent->getLocalTransformationMatrix());
		} else {
			ltm = new Matrix4(*modelMatrix);
		}
	}
}


void DFFFrame::invalidateLTM()
{
	if (ltm) {
		delete ltm;
		ltm = NULL;
	}

	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		DFFFrame* child = *it;
		child->invalidateLTM();
	}
}

