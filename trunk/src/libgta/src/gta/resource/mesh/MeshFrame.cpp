/*
 * MeshFrame.cpp
 *
 *  Created on: 24.09.2011
 *      Author: alemariusnexus
 */

#include "MeshFrame.h"
#include "../../EngineException.h"
#include <algorithm>

using std::find;



MeshFrame::MeshFrame(const DFFFrame* frame)
		: absModelMatrixValid(false), name(0), parent(NULL)
{
	if (frame->getName()) {
		name = CString(frame->getName()).lower();
	}

	modelMatrix = Matrix4::translation(frame->getTranslation()) * Matrix4(frame->getRotation());

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

	printf("%s\n", name.get());

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

		if (child->name == name)
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
