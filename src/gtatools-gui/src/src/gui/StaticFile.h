/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef STATICFILE_H_
#define STATICFILE_H_

#include <gtaformats/util/File.h>
#include <QtCore/qlist.h>


class StaticFile {
private:
	typedef QList<StaticFile*> ChildList;

public:
	StaticFile();
	StaticFile(const File& file);
	~StaticFile();
	File* getFile() const { return file; }
	StaticFile* getParent() const { return parent; }
	int getChildCount() const { return children.size(); }
	StaticFile* getChild(int idx) const { return children[idx]; }
	int indexOf(StaticFile* child) const { return children.indexOf(child); }
	const char* toString() const { return file->getPath()->getFileName(); }
	void addRootChild(StaticFile* child);

private:
	StaticFile(File* file, StaticFile* parent);
	void initChildren();

private:
	File* file;
	StaticFile* parent;
	ChildList children;
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
