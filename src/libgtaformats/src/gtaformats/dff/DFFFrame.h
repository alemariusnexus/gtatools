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

#ifndef DFFFRAME_H_
#define DFFFRAME_H_

#include <gtaformats/config.h>
#include <cstdlib>
#include "../util/CString.h"
#include "../util/math/Matrix3.h"
#include "../util/math/Matrix4.h"
#include "../util/math/Vector3.h"
#include "DFFBone.h"
#include <vector>

using std::vector;



/**	\brief A DFFFrame is a class which stores transformation data for DFF geometries.
 *
 *	The frames are a hierarchical structure, child frames transform relative to their parents. Each
 *	transformation consists of a rotation matrix, a translation vector and some unknown flags.
 */
class DFFFrame {
private:
	friend class DFFLoader;

public:
	typedef vector<DFFFrame*>::iterator ChildIterator;
	typedef vector<DFFFrame*>::const_iterator ConstChildIterator;

public:
	/**	\brief Creates a new DFFFrame with given translation and rotation.
	 *
	 * 	@param trans The translation vector.
	 * 	@param rot The rotation vector.
	 */
	DFFFrame(Matrix4* modelMatrix)
			: modelMatrix(modelMatrix), ltm(NULL), parent(NULL), flags(0), bone(NULL) {}

	/**	\brief Creates a new DFFFrame with identity transformation and no name.
	 */
	DFFFrame() : modelMatrix(new Matrix4), ltm(NULL), parent(NULL), flags(0), bone(NULL) {}

	/**	\brief Copy constructor.
	 */
	DFFFrame(const DFFFrame& other);

	/**	\brief Destroys this frame, including all child frames.
	 */
	~DFFFrame();

	const Matrix4& getModelMatrix() const { return *modelMatrix; }

	const Matrix4& getLocalTransformationMatrix() { ensureValidLTM(); return *ltm; }

	/**	\brief Returns the parent frame.
	 *
	 * 	@return The parent frame or NULL, if it's a root frame.
	 */
	DFFFrame* getParent() { return parent; }

	/**	\brief Returns the parent frame.
	 *
	 * 	@return The parent frame or NULL, if it's a root frame.
	 */
	const DFFFrame* getParent() const { return parent; }

	/**	\brief Returns the flags for this frame.
	 *
	 * 	What these flags are is currently unknown. It is believed that these are flags set at matrix creation
	 * 	time but that they are not actually used by GTA.
	 *
	 *	@see <a href="http://www.gtamodding.com/index.php?title=Talk:Frame_List_%28RW_Section%29">
	 *		DFF Frame List section discussion</a> on gtamodding.com
	 *	@return The frame flags.
	 */
	uint32_t getFlags() const { return flags; }

	/**	\brief Returns the name of this frame.
	 *
	 * 	@return The frame name or NULL if it has no name.
	 */
	CString getName() const { return name; }

	/**	\brief Returns the number of child frames.
	 *
	 * 	@return The number of child frames.
	 */
	uint32_t getChildCount() const { return children.size(); }

	/**	\brief Returns an iterator to the begin of the child frame list.
	 *
	 * 	@return The child list begin iterator.
	 */
	ChildIterator getChildBegin() { return children.begin(); }

	/**	\brief Returns an iterator to the begin of the child frame list.
	 *
	 * 	@return The child list begin iterator.
	 */
	ConstChildIterator getChildBegin() const { return children.begin(); }

	/**	\brief Returns an iterator to the end of the child frame list.
	 *
	 * 	@return The child list end iterator.
	 */
	ChildIterator getChildEnd() { return children.end(); }

	/**	\brief Returns an iterator to the end of the child frame list.
	 *
	 * 	@return The child list end iterator.
	 */
	ConstChildIterator getChildEnd() const { return children.end(); }

	/**	\brief Returns a child frame by it's index.
	 *
	 * 	@param index The child frame index.
	 * 	@return The child frame.
	 */
	DFFFrame* getChild(uint32_t index);

	/**	\brief Returns a child frame by it's index.
	 *
	 * 	@param index The child frame index.
	 * 	@return The child frame.
	 */
	const DFFFrame* getChild(uint32_t index) const;

	/**	\brief Returns a child frame by it's name.
	 *
	 * 	@param name The child name.
	 * 	@return The child frame.
	 */
	DFFFrame* getChild(const CString& name);

	/**	\brief Returns a child frame by it's name.
	 *
	 * 	@param name The child name.
	 * 	@return The child frame.
	 */
	const DFFFrame* getChild(const CString& name) const;

	DFFFrame* getChildByBoneID(int32_t id, bool recurse = false);

	/**	\brief Returns the index of the given child frame.
	 *
	 *	@param child The child you want to know the index of.
	 *	@return The child index or -1 if it's not a child of this frame.
	 */
	int32_t indexOf(const DFFFrame* child) const;

	void setModelMatrix(Matrix4* mm) { invalidateLTM(); modelMatrix = mm; }

	/**	\brief Sets the flags of this frame.
	 *
	 * 	@param flags The flags.
	 * 	@see getFlags()
	 */
	void setFlags(uint32_t flags) { this->flags = flags; }

	/**	\brief Sets the name of this frame.
	 *
	 * 	This frame will take ownership of the string pointer parameter and will delete it on destruction.
	 *
	 * 	@param name The name.
	 * 	@see setName(const char*) if you want a copy of the string to be saved instead of the original one.
	 */
	void setName(const CString& name) { this->name = name; }

	/**	\brief Adds a new child frame.
	 *
	 * 	Calling this method with an already parented frame as child will throw an exception, so be sure to
	 * 	<b>remove the child from it's old parent</b> if is has any.
	 *
	 * 	@param child The new child.
	 */
	void addChild(DFFFrame* child) { children.push_back(child); child->reparent(this); }

	/**	\brief Removes the given child.
	 *
	 *	This function does not delete the child!
	 *
	 *	@param child The child to remove.
	 */
	void removeChild(DFFFrame* child);

	/**	\brief Removes the child at the given index.
	 *
	 *	This function does not delete the child!
	 *
	 *	@param index The child index.
	 */
	void removeChild(int32_t index) { removeChild(getChild(index)); }

	/**	\brief Removes the child with the given name.
	 *
	 * 	This function does not delete the child!
	 *
	 * 	@param name The child name.
	 */
	void removeChild(const CString& name) { removeChild(getChild(name)); }

	/**	\brief Removes all child frames.
	 *
	 * 	This function does not delete any children!
	 */
	void removeChildren();

	/**	\brief Returns whether this frame is a root frame (i.e. has no parent).
	 *
	 * 	@return true if it's a root frame, false otherwise.
	 */
	bool isRoot() const { return parent == NULL; }

	DFFBone* getBone() { return bone; }

	const DFFBone* getBone() const { return bone; }

	void setBone(DFFBone* bone) { this->bone = bone; }

private:
	/**	\brief Internal.
	 *
	 *	Assigns the parent frame. Also checks whether this frame already has a parent and throws an exception
	 *	in that case.
	 *
	 *	@param frame The new parent frame. No addChild method will be called on it!
	 */
	void reparent(DFFFrame* frame);

	void ensureValidLTM();
	void invalidateLTM();

private:
	CString name;
	Matrix4* modelMatrix;
	Matrix4* ltm;
	DFFFrame* parent;
	vector<DFFFrame*> children;
	uint32_t flags;
	DFFBone* bone;
};

#endif /* DFFFRAME_H_ */
