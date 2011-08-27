/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#ifndef IFPOBJECT_H_
#define IFPOBJECT_H_

#include "IFPFrame.h"
#include "IFPRootFrame.h"
#include <vector>

using std::vector;



class IFPObject {
public:
	enum FrameType
	{
		ChildFrame = 3,
		RootFrame = 4
	};

public:
	typedef vector<IFPFrame*> FrameList;
	typedef FrameList::iterator FrameIterator;
	typedef FrameList::const_iterator ConstFrameIterator;

public:
	void setName(const char* n) { if (name) delete[] name; name = new char[strlen(n)+1]; strcpy(name, n); }
	void setFrameType(FrameType type) { frameType = type; }
	void setBoneID(int32_t id) { boneID = id; }
	char* getName() { return name; }
	const char* getName() const { return name; }
	FrameType getFrameType() const { return frameType; }
	int32_t getBoneID() const { return boneID; }
	FrameIterator getFrameBegin() { return frames.begin(); }
	FrameIterator getFrameEnd() { return frames.end(); }
	ConstFrameIterator getFrameBegin() const { return frames.begin(); }
	ConstFrameIterator getFrameEnd() const { return frames.end(); }
	int getFrameCount() const { return frames.size(); }
	void addFrame(IFPFrame* frame) { frames.push_back(frame); }

public:
	IFPObject() {}

private:
	char* name;
	FrameType frameType;
	FrameList frames;
	int32_t boneID;

private:
	friend class IFPLoader;
};

#endif /* IFPOBJECT_H_ */
