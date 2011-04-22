/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef STATICFILE_H_
#define STATICFILE_H_

#include <gtaformats/util/File.h>
#include <QtCore/QList>


class StaticFile {
public:
	typedef QList<StaticFile*> ChildList;
	typedef ChildList::iterator ChildIterator;

private:
	enum Type {
		Unknown,
		Node,
		Directory,
		Archive
	};

public:
	StaticFile(const File& file);
	~StaticFile();
	File* getFile() const { return file; }
	StaticFile* getParent() const { return parent; }
	int getChildCount() { ensureChildrenAvailable(); return children.size(); }
	StaticFile* getChild(int idx) { ensureChildrenAvailable(); return children[idx]; }
	ChildIterator getChildBegin() { return children.begin(); }
	ChildIterator getChildEnd() { return children.end(); }
	int indexOf(StaticFile* child) { ensureChildrenAvailable(); return children.indexOf(child); }
	const char* toString() const { return file->getPath()->getFileName(); }

private:
	StaticFile(File* file, StaticFile* parent, Type type = Unknown);
	void loadChildren();
	void ensureChildrenAvailable();

private:
	File* file;
	StaticFile* parent;
	ChildList children;
	bool childrenInited;
	Type type;
};


/*class StaticFile {
private:
	typedef QList<StaticFile*> ChildList;

public:
	StaticFile(const char* name) : name(name), parent(NULL) {}
	StaticFile(const char* name, StaticFile* parent) : name(name), parent(NULL) { parent->addChild(this); }
	StaticFile* getParent() const { return parent; }
	int getChildCount() const { return children.size(); }
	StaticFile* getChild(int idx) const { return children[idx]; }
	int indexOf(StaticFile* child) const { return children.indexOf(child); }
	const char* toString() const { return name; }
	void addChild(StaticFile* child) { children << child; child->parent = this; }

private:
	const char* name;
	StaticFile* parent;
	ChildList children;
};*/

#endif /* STATICFILE_H_ */
