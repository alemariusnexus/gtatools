/*
 * StaticFile.h
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
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
