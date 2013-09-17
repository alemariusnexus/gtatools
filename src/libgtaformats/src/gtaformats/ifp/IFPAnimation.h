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

#ifndef IFPANIMATION_H_
#define IFPANIMATION_H_

#include "IFPObject.h"
#include "../util/CString.h"
#include <vector>

using std::vector;



class IFPAnimation {
public:
	typedef vector<IFPObject*> ObjectList;
	typedef ObjectList::iterator ObjectIterator;
	typedef ObjectList::const_iterator ConstObjectIterator;

public:
	void setName(const CString& n) { name = n; }
	CString getName() const { return name; }
	ObjectIterator getObjectBegin() { return objs.begin(); }
	ObjectIterator getObjectEnd() { return objs.end(); }
	ConstObjectIterator getObjectBegin() const { return objs.begin(); }
	ConstObjectIterator getObjectEnd() const { return objs.end(); }
	int getObjectCount() const { return objs.size(); }
	void addObject(IFPObject* obj) { objs.push_back(obj); }

public:
	IFPAnimation() {}

private:
	CString name;
	ObjectList objs;

private:
	friend class IFPLoader;
};

#endif /* IFPANIMATION_H_ */
