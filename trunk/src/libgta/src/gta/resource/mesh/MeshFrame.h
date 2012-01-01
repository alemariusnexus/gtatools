/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#ifndef MESHFRAME_H_
#define MESHFRAME_H_

#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/util/CString.h>
#include <gtaformats/dff/DFFFrame.h>
#include <vector>
#include "../../Engine.h"

using std::vector;



class MeshFrame {
public:
	typedef vector<MeshFrame*> ChildList;
	typedef ChildList::iterator ChildIterator;
	typedef ChildList::const_iterator ConstChildIterator;

public:
	MeshFrame() : absModelMatrixValid(false), parent(NULL) {}
	MeshFrame(const DFFFrame* frame);
	~MeshFrame();
	const Matrix4 getModelMatrix() const { return modelMatrix; }
	const Matrix4 getAbsoluteModelMatrix();
	void setModelMatrix(const Matrix4& mm) { modelMatrix = mm; invalidateAbsModelMatrix(); }
	void addChild(MeshFrame* node);
	bool removeChild(MeshFrame* node);
	size_t getChildCount() const { return children.size(); }
	ChildIterator getChildBegin() { return children.begin(); }
	ChildIterator getChildEnd() { return children.end(); }
	ConstChildIterator getChildBegin() const { return children.begin(); }
	ConstChildIterator getChildEnd() const { return children.end(); }
	MeshFrame* getParent() { return parent; }
	const MeshFrame* getParent() const { return parent; }
	CString getName() const { return name; }
	void setName(const CString& name) { this->name = name; }
	MeshFrame* getChildByName(const CString& name, bool recursive = false);
	void dump(int level = 0);

private:
	void reparent(MeshFrame* parent);
	void invalidateAbsModelMatrix();

private:
	Matrix4 modelMatrix;
	Matrix4 absModelMatrix;
	bool absModelMatrixValid;

	CString name;

	MeshFrame* parent;
	ChildList children;
};

#endif /* MESHFRAME_H_ */
