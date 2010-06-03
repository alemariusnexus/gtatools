/*
 * File.cpp
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#include "File.h"
#include <cstdio>
#include <cstring>




File::File(const char* path)
		: path(NULL)
#ifdef linux
		  , dir(NULL)
#endif

{
	const char* end = path + strlen(path);

	while (*end == '/'  ||  *end == '\\') {
		end--;
	}

	this->path = new char[end-path+1];
	strncpy(this->path, path, end-path+1);
}


File::~File()
{
	delete[] path;

#ifdef linux
	if (dir != NULL) {
		closedir(dir);
	}
#endif
}


bool File::exists() const
{
#ifdef linux
	struct stat fileInfo;
	return stat(path, &fileInfo) == 0;
#else
	#error "File::exists() not supported on this platform!"
#endif
}


File::FileType File::getType() const
{
#ifdef linux
	struct stat fileInfo;

	if (stat(path, &fileInfo) != 0) {
		return TYPE_ERROR;
	}

	if (S_ISDIR(fileInfo.st_mode)) {
		return TYPE_DIRECTORY;
	} else if (S_ISREG(fileInfo.st_mode)) {
		return TYPE_FILE;
	}
#else
	#error "File::getType() not supported on this platform!"
#endif
}


File* File::getChild(const char* name) const
{
	char* newPath = new char[strlen(path) + strlen(name) + 2];
	sprintf(newPath, "%s/%s", path, name);
	File* child = new File(newPath);
	delete[] newPath;
	return child;
}


File* File::getFirstChild()
{
#ifdef linux
	if (dir != NULL) {
		closedir(dir);
	}

	dir = opendir(path);
#endif

	return getNextChild();
}


File* File::getNextChild()
{
#ifdef linux
	dirent* entry = readdir(dir);
	File* child = getChild(entry->d_name);
	delete entry;
	return child;
#endif
}


File* File::getParent() const
{
	char* parentEnd = strrchr(path, '/');
	char* parentPath = new char[parentEnd-path + 1];
	strncpy(parentPath, path, parentEnd-path);
	File* parent = new File(parentPath);
	delete[] parentPath;
	return parent;
}


