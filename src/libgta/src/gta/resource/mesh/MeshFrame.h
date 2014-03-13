/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include <nxcommon/math/Matrix4.h>
#include <nxcommon/CString.h>
#include <gtaformats/dff/DFFFrame.h>
#include <vector>
#include <algorithm>

using std::vector;
using std::find;



class MeshFrame {
public:
	typedef vector<MeshFrame*> ChildList;
	typedef ChildList::iterator ChildIterator;
	typedef ChildList::const_iterator ConstChildIterator;

public:
	MeshFrame() : absModelMatrixValid(false), boneID(-1), parent(NULL) {}
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
	Matrix4 getInverseBoneMatrix() const { return ibm; }
	int32_t getBoneID() const { return boneID; }
	void setBoneID(int32_t id) { boneID = id; }
	int32_t getBoneNumber() const { return boneNum; }
	void setBoneNumber(int32_t num) { boneNum = num; }
	size_t indexOf(MeshFrame* child) const
			{ return find(children.begin(), children.end(), child) - children.begin(); }
	MeshFrame* getChild(size_t idx) { return children[idx]; }

private:
	void reparent(MeshFrame* parent);
	void invalidateAbsModelMatrix();

private:
	Matrix4 modelMatrix;
	Matrix4 absModelMatrix;
	bool absModelMatrixValid;
	Matrix4 ibm;

	CString name;
	int32_t boneID;
	int32_t boneNum;

	MeshFrame* parent;
	ChildList children;
};

#endif /* MESHFRAME_H_ */
