/*
 * StaticFile.cpp
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
 */

#include "StaticFile.h"



StaticFile::StaticFile()
		: file(NULL), parent(NULL)
{
}


StaticFile::StaticFile(const File& file)
		: file(new File(file)), parent(NULL)
{
	initChildren();
}


StaticFile::StaticFile(File* file, StaticFile* parent)
		: file(file), parent(parent)
{
	initChildren();
}


void StaticFile::initChildren()
{
	if (file->isDirectory()  ||  file->isArchiveFile()) {
		FileIterator* it = file->getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			children << new StaticFile(child, this);
		}

		delete it;
	}
}


void StaticFile::addRootChild(StaticFile* child)
{
	children << child;
	child->parent = this;
}


