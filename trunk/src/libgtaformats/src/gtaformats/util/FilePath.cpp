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
 */

#include "FilePath.h"
#include "File.h"
#include "strutil.h"
#include <cstring>
#include <cstdio>

#ifdef _POSIX_VERSION
#include <sys/types.h>
#include <dirent.h>
#endif


FilePath::FilePath(const char* path, int flags)
		: path(normalize(path, flags))
{
}


FilePath::FilePath(const FilePath& other)
		: path(new char[strlen(other.path)+1])
{
	strcpy(path, other.path);
}


FilePath::FilePath(const FilePath& parent, const char* child, int flags)
{
	char* tmpPath = new char[strlen(parent.path) + strlen(child) + 2];
	strcpy(tmpPath, parent.path);
	strcat(tmpPath, "/");
	strcat(tmpPath, child);
	path = normalize(tmpPath, flags);
	delete[] tmpPath;
}


FilePath::~FilePath()
{
	delete[] path;
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
		return path;
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

	FileContentType retval;

	if (strcmp(ext, "img") == 0) {
		retval = CONTENT_TYPE_IMG;
	} else if (strcmp(ext, "dir") == 0) {
		retval = CONTENT_TYPE_DIR;
	} else if (strcmp(ext, "ide") == 0) {
		retval = CONTENT_TYPE_IDE;
	} else if (strcmp(ext, "dff") == 0) {
		retval = CONTENT_TYPE_DFF;
	} else if (strcmp(ext, "ipl") == 0) {
		retval = CONTENT_TYPE_IPL;
	} else if (strcmp(ext, "txd") == 0) {
		retval = CONTENT_TYPE_TXD;
	} else if (strcmp(ext, "dat") == 0) {
		retval = CONTENT_TYPE_DAT;
	} else if (strcmp(ext, "col") == 0) {
		retval = CONTENT_TYPE_COL;
	} else {
		retval = CONTENT_TYPE_UNKNOWN;
	}

	delete[] ext;

	return retval;
}


bool FilePath::isIMGPath() const
{
	FilePath* parent = getParentPath();
	bool imgPath = false;

	if (!parent) {
		return false;
	}

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
		delete parent;
		return false;
	}

	if (*parent == other) {
		delete parent;
		return true;
	} else {
		bool ret = parent->isChildOf(other);
		delete parent;
		return ret;
	}
}


bool FilePath::operator==(const FilePath& other) const
{
	return strcmp(path, other.path) == 0;
}


char* FilePath::normalize(const char* src, int flags)
{
	int srcLen = strlen(src);
	char* dest = new char[srcLen+1];
	char* lastComponentStart = dest;
	int srcIdx = 0, destIdx = 0;

	//for (int i = 0 ; i <= srcLen ; i++) {
	while (srcIdx <= srcLen) {
		char chr = src[srcIdx];

		if (chr == '\\'  &&  (flags & BackslashAsSeparator) != 0) {
			chr = '/';
		}

		if (chr == '/'  &&  destIdx > 0  &&  dest[destIdx-1] == '/') {
			srcIdx++;
			continue;
		}

		dest[destIdx] = chr;

		if ((chr == '/'  ||  chr == '\0')  &&  srcIdx != 0  &&  (flags & CorrectCase) != 0) {
			dest[destIdx] = '\0';

			char* component = new char[destIdx - (lastComponentStart-dest) + 1];
			strtolower(component, lastComponentStart+1);
			if (chr == '/') {
				dest[destIdx] = '/';
				dest[destIdx+1] = '\0';
			}

			File* compParent = File(dest).getParent();

			if (compParent) {
				FileIterator* it = compParent->getIterator();
				File* child;

				while ((child = it->next())  !=  NULL) {
					const char* fname = child->getPath()->getFileName();
					char* lfname = new char[strlen(fname)+1];
					strtolower(lfname, fname);
					if (strcmp(lfname, component) == 0) {
						memcpy(lastComponentStart+1, fname, strlen(fname));
						delete child;
						delete[] lfname;
						break;
					}
					delete child;
					delete[] lfname;
				}

				delete it;
				delete compParent;
			}

			delete[] component;

			lastComponentStart = dest+destIdx;
		}

		srcIdx++;
		destIdx++;
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

