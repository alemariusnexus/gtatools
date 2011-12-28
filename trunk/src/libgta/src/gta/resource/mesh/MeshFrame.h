/*
 * MeshFrame.h
 *
 *  Created on: 24.09.2011
 *      Author: alemariusnexus
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
