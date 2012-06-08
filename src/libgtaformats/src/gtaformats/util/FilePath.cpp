/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "FilePath.h"
#include "File.h"
#include "strutil.h"
#include "FileException.h"
#include <cstring>
#include <cstdio>

#ifdef _POSIX_VERSION

#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <dirent.h>

#elif defined(_WIN32)

#include <windows.h>

#endif


FilePath::FilePath(const CString& path, int flags)
		: path(normalize(path, flags))
{
}


FilePath::FilePath(const FilePath& other)
		: path(other.path)
{
}


FilePath::FilePath(const FilePath& parent, const CString& child, int flags)
{
	CString tmpPath(parent.path);
	tmpPath.append("/");
	tmpPath.append(child);
	path = normalize(tmpPath, flags);
}


FilePath::~FilePath()
{
}


CString FilePath::getExtension() const
{
	int len = path.length();

	for (int i = len-1 ; i > 0 ; i--) {
		if (path[i] == '.') {
			return path.substr(i+1);
		} else if (path[i] == '/') {
			return CString();
		}
	}

	return CString();
}


CString FilePath::getFileName() const
{
	const char* fname = strrchr(path.get(), '/');

	if (fname == NULL) {
		return path;
	}

	return CString(fname+1);
}


CString FilePath::getPathPart(int partNo)
{
	char* pathCpy = new char[path.length()+1];
	strcpy(pathCpy, path.get());

	char* start = strtok(pathCpy, "/");

	for (int i = 0 ; i < partNo ; i++) {
		start = strtok(NULL, "/");
	}

	char* end = strtok(pathCpy, "/");
	if (end == NULL) {
		end = pathCpy + path.length() + 1;
	}
	end--;

	char* part = new char[end-start + 1];
	strncpy(part, start, end-start);
	part[strlen(part)] = '\0';

	delete[] pathCpy;

	return CString::from(part);
}


FilePath* FilePath::getParentPath() const
{
	const char* parentEnd = strrchr(path.get(), '/');

	if (parentEnd == NULL  ||  parentEnd == path.get()) {
		return NULL;
	}

	char* parentPath = new char[parentEnd-path.get()+1];
	strncpy(parentPath, path.get(), parentEnd-path.get());
	parentPath[parentEnd-path.get()] = '\0';
	FilePath* parent = new FilePath(CString::from(parentPath));
	return parent;
}


FileContentType FilePath::guessContentType() const
{
	CString tmpExt = getExtension();

	if (tmpExt.get() == NULL) {
		return CONTENT_TYPE_UNKNOWN;
	}

	CString ext = tmpExt.lower();

	FileContentType retval;

	if (ext == CString("img")) {
		retval = CONTENT_TYPE_IMG;
	} else if (ext == CString("dir")) {
		retval = CONTENT_TYPE_DIR;
	} else if (ext == CString("ide")) {
		retval = CONTENT_TYPE_IDE;
	} else if (ext == CString("dff")) {
		retval = CONTENT_TYPE_DFF;
	} else if (ext == CString("ipl")) {
		retval = CONTENT_TYPE_IPL;
	} else if (ext == CString("txd")) {
		retval = CONTENT_TYPE_TXD;
	} else if (ext == CString("dat")) {
		retval = CONTENT_TYPE_DAT;
	} else if (ext == CString("col")) {
		retval = CONTENT_TYPE_COL;
	} else if (ext == CString("ifp")) {
		retval = CONTENT_TYPE_IFP;
	} else if (ext == CString("gxt")) {
		retval = CONTENT_TYPE_GXT;
	} else {
		retval = CONTENT_TYPE_UNKNOWN;
	}

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


bool FilePath::isChildOf(const FilePath& other, bool recursive) const
{
	FilePath* parent = getParentPath();

	if (parent == NULL) {
		delete parent;
		return false;
	}

	if (*parent == other) {
		delete parent;
		return true;
	} else if (recursive) {
		bool ret = parent->isChildOf(other, recursive);
		delete parent;
		return ret;
	} else {
		return false;
	}
}


FilePath* FilePath::relativeTo(const FilePath& parent) const
{
	size_t len = parent.toString().length()+1;
	return new FilePath(path.substr(len));
}


bool FilePath::operator==(const FilePath& other) const
{
	return path == other.path;
}


CString FilePath::normalize(const CString& src, int flags)
{
	int srcLen = src.length();
	char* dest = new char[srcLen+1];
	char* lastComponentStart = dest;
	int srcIdx = 0, destIdx = 0;

	//for (int i = 0 ; i <= srcLen ; i++) {
	while (srcIdx <= srcLen) {
		char chr = src[srcIdx];

		if (chr == '\\'  &&  (flags & BackslashAsSeparator) != 0) {
			chr = '/';
		}

		// Remove multiple directory separators
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
					CString fname = child->getPath()->getFileName();
					char* lfname = new char[fname.length() + 1];
					strtolower(lfname, fname.get());
					if (strcmp(lfname, component) == 0) {
						memcpy(lastComponentStart+1, fname.get(), fname.length());
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

	return CString::from(dest);
}


bool FilePath::operator>(const FilePath& other) const
{
	return path > other.path;
}


bool FilePath::operator<(const FilePath& other) const
{
	return path < other.path;
}


FilePath* FilePath::absolute() const
{
#ifdef _POSIX_VERSION
	char absPath[MAXPATHLEN];

	if (!realpath(path.get(), absPath)) {
		throw FileException("Error getting absolute path via realpath()!", __FILE__, __LINE__);
	}

	return new FilePath(absPath);
#elif defined(_WIN32)
	char absPath[MAX_PATH];

	if (GetFullPathName(path.get(), sizeof(absPath), absPath, NULL) == 0) {
		throw FileException("Error getting absolute path via GetFullPathName()!", __FILE__, __LINE__);
	}

	return new FilePath(absPath);
#endif
}

