/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "MeshFrame.h"
#include "../../EngineException.h"
#include <algorithm>

using std::find;



MeshFrame::MeshFrame(const DFFFrame* frame)
		: absModelMatrixValid(false),
		  ibm(frame->getBone() ? frame->getBone()->getInverseBoneMatrix() : Matrix4::Identity), name(0),
		  boneID(frame->getBone() ? frame->getBone()->getIndex() : -1),
		  boneNum(frame->getBone() ? frame->getBone()->getNumber() : -1), parent(NULL)
{
	if (frame->getName().get()) {
		name = frame->getName().lower();
	}

	//modelMatrix = Matrix4::translation(frame->getTranslation()) * Matrix4(frame->getRotation());
	modelMatrix = frame->getModelMatrix();

	for (DFFFrame::ConstChildIterator it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
		const DFFFrame* cframe = *it;

		MeshFrame* child = new MeshFrame(cframe);
		addChild(child);
	}
}


MeshFrame::~MeshFrame()
{
	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		delete *it;
	}
}


void MeshFrame::dump(int level)
{
	for (int i = 0 ; i < level ; i++) {
		printf("  ");
	}

	if (name.get())
		printf("%s\n", name.get());
	else
		printf("[UNNAMED]\n");

	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		MeshFrame* child = *it;
		child->dump(level+1);
	}
}


void MeshFrame::addChild(MeshFrame* node)
{
	node->reparent(this);
	children.push_back(node);
}


bool MeshFrame::removeChild(MeshFrame* node)
{
	ChildIterator it = find(children.begin(), children.end(), node);

	if (it == children.end()) {
		return false;
	}

	children.erase(it);
	node->reparent(NULL);
	return true;
}


MeshFrame* MeshFrame::getChildByName(const CString& name, bool recursive)
{
	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		MeshFrame* child = *it;

		if (child->name.get()  &&  child->name.lower().trim() == name)
			return child;

		if (recursive) {
			MeshFrame* subChild = child->getChildByName(name, true);

			if (subChild)
				return subChild;
		}
	}

	return NULL;
}


const Matrix4 MeshFrame::getAbsoluteModelMatrix()
{
	if (!absModelMatrixValid) {
		if (parent)
			absModelMatrix = parent->getAbsoluteModelMatrix() * modelMatrix;
		else
			absModelMatrix = modelMatrix;

		absModelMatrixValid = true;
	}

	return absModelMatrix;
}


void MeshFrame::reparent(MeshFrame* parent)
{
	if (this->parent  &&  parent) {
		throw EngineException("Attempt to reparent a MeshFrame which still has a parent. Remove it from its "
				"old parent first", __FILE__, __LINE__);
	}

	this->parent = parent;
	invalidateAbsModelMatrix();
}


void MeshFrame::invalidateAbsModelMatrix()
{
	absModelMatrixValid = false;

	for (ChildIterator it = children.begin() ; it != children.end() ; it++) {
		MeshFrame* child = *it;
		child->invalidateAbsModelMatrix();
	}
}
