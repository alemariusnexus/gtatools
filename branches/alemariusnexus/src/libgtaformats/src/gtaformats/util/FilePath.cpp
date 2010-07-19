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

#include "FilePath.h"
#include "File.h"
#include "util.h"
#include <cstring>
#include <cstdio>


FilePath::FilePath(const char* path)
		: path(normalize(path))
{
}


FilePath::FilePath(const FilePath& other)
		: path(new char[strlen(other.path)+1])
{
	strcpy(path, other.path);
}


FilePath::FilePath(const FilePath& parent, const char* child)
		: path(new char[strlen(parent.path) + strlen(child) + 2])
{
	char* normChild = normalize(child);
	strcpy(path, parent.path);
	strcat(path, "/");
	strcat(path, normChild);
	delete[] normChild;
}


const char* FilePath::getExtension() const
{
	int len = strlen(path);

	for (int i = len-1 ; i > 0 ; i--) {
		if (path[i] == '.') {
			return path+i+1;
		} else if (path[i] == '/') {
			return NULL;
		}
	}

	return NULL;
}


const char* FilePath::getFileName() const
{
	const char* fname = strrchr(path, '/');

	if (fname == NULL) {
		return NULL;
	}

	return fname+1;
}


char* FilePath::getPathPart(int partNo)
{
	char* start = strtok(path, "/");

	for (int i = 0 ; i < partNo ; i++) {
		start = strtok(NULL, "/");
	}

	char* end = strtok(path, "/");
	if (end == NULL) {
		end = path + strlen(path) + 1;
	}
	end--;

	char* part = new char[end-start + 1];
	strncpy(part, start, end-start);
	part[strlen(part)] = '\0';

	return part;
}


FilePath* FilePath::getParentPath() const
{
	char* parentEnd = strrchr(path, '/');

	if (parentEnd == NULL  ||  parentEnd == path) {
		return NULL;
	}

	char* parentPath = new char[parentEnd-path+1];
	strncpy(parentPath, path, parentEnd-path);
	parentPath[parentEnd-path] = '\0';
	FilePath* parent = new FilePath(parentPath);
	delete[] parentPath;
	return parent;
}


FileContentType FilePath::guessContentType() const
{
	const char* tmpExt = getExtension();

	if (tmpExt == NULL) {
		return CONTENT_TYPE_UNKNOWN;
	}

	char* ext = new char[strlen(tmpExt)+1];
	strtolower(ext, tmpExt);

	if (strcmp(ext, "img") == 0) {
		return CONTENT_TYPE_IMG;
	} else if (strcmp(ext, "dir") == 0) {
		return CONTENT_TYPE_DIR;
	} else if (strcmp(ext, "ide") == 0) {
		return CONTENT_TYPE_IDE;
	} else if (strcmp(ext, "dff") == 0) {
		return CONTENT_TYPE_DFF;
	} else if (strcmp(ext, "ipl") == 0) {
		return CONTENT_TYPE_IPL;
	} else if (strcmp(ext, "txd") == 0) {
		return CONTENT_TYPE_TXD;
	} else {
		return CONTENT_TYPE_UNKNOWN;
	}
}


bool FilePath::isIMGPath() const
{
	FilePath* parent = getParentPath();
	bool imgPath = false;

	if (File(parent, false).isRegularFile()) {
		FileContentType type = parent->guessContentType();

		if (type == CONTENT_TYPE_DIR  ||  type == CONTENT_TYPE_IMG) {
			imgPath = true;
		}
	}

	delete parent;
	return imgPath;
}


bool FilePath::isChildOf(const FilePath& other) const
{
	FilePath* parent = getParentPath();

	if (parent == NULL) {
		return false;
	}

	if (*parent == other) {
		return true;
	} else {
		return parent->isChildOf(other);
	}
}


bool FilePath::operator==(const FilePath& other) const
{
	return strcmp(path, other.path) == 0;
}


char* FilePath::normalize(const char* src)
{
	int srcLen = strlen(src);
	char* dest = new char[srcLen+1];

	for (int i = 0 ; i < srcLen ; i++) {
		char chr = src[i];

		if (chr == '\\') {
			chr = '/';
		}

		dest[i] = chr;
	}

	dest[srcLen] = '\0';
	rtrim(dest, '/');

	return dest;
}


bool FilePath::operator>(const FilePath& other) const
{
	return strcmp(path, other.path) > 0;
}


bool FilePath::operator<(const FilePath& other) const
{
	return strcmp(path, other.path) < 0;
}

