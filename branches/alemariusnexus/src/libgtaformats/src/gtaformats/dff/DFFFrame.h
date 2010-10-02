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

#ifndef DFFFRAME_H_
#define DFFFRAME_H_

#include "../gf_config.h"
#include <cstdlib>
#include "../util/math/Matrix3.h"
#include "../util/math/Vector3.h"
#include <vector>

using std::vector;


class DFFFrame {
private:
	friend class DFFLoader;

public:
	typedef vector<DFFFrame*>::iterator ChildIterator;
	typedef vector<DFFFrame*>::const_iterator ConstChildIterator;

public:
	DFFFrame(Vector3* trans, Matrix3* rot)
			: name(NULL), translation(trans), rotation(rot), parent(NULL), flags(0) {}
	DFFFrame() : name(NULL), translation(new Vector3), rotation(new Matrix3), parent(NULL), flags(0) {}
	DFFFrame(const DFFFrame& other);
	~DFFFrame();
	Matrix3& getRotation() { return *rotation; }
	const Matrix3& getRotation() const { return *rotation; }
	Vector3& getTranslation() { return *translation; }
	const Vector3& getTranslation() const { return *translation; }
	DFFFrame* getParent() { return parent; }
	const DFFFrame* getParent() const { return parent; }
	int32_t getFlags() const { return flags; }
	const char* getName() const { return name; }
	int32_t getChildCount() { return children.size(); }
	ChildIterator getChildBegin() { return children.begin(); }
	ConstChildIterator getChildBegin() const { return children.begin(); }
	ChildIterator getChildEnd() { return children.end(); }
	ConstChildIterator getChildEnd() const { return children.end(); }
	DFFFrame* getChild(int32_t index);
	const DFFFrame* getChild(int32_t index) const;
	DFFFrame* getChild(const char* name);
	const DFFFrame* getChild(const char* name) const;
	int32_t indexOf(const DFFFrame* child) const;
	void setRotation(const Matrix3& rot) { rotation = new Matrix3(rot); }
	void setRotation(Matrix3* rot) { rotation = rot; }
	void setTranslation(const Vector3& trans) { translation = new Vector3(trans); }
	void setTranslation(Vector3* trans) { translation = trans; }
	void setFlags(int32_t flags) { this->flags = flags; }
	void setName(const char* name) { this->name = new char[strlen(name)+1]; strcpy(this->name, name); }
	void setName(char* name) { this->name = name; }
	void addChild(DFFFrame* child) { children.push_back(child); child->parent = this; }
	void removeChild(DFFFrame* child);
	void removeChild(int32_t index) { removeChild(getChild(index)); }
	void removeChild(const char* name) { removeChild(getChild(name)); }
	void removeChildren();
	bool isRoot() const { return parent == NULL; }

	void mirrorYZ();
	void scale(float x, float y, float z);

private:
	Matrix3* rotation;
	Vector3* translation;
	DFFFrame* parent;
	vector<DFFFrame*> children;
	int32_t flags;
	char* name;
};

#endif /* DFFFRAME_H_ */
