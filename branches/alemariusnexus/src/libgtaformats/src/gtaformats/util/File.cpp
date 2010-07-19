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

#include "File.h"
#include <cstdio>
#include <cstring>
#include "util.h"
#include "FileException.h"
#include "stream/FileInputStream.h"
#include "../img/IMGArchive.h"

#ifdef linux
#include <errno.h>
#endif




File::File(const char* path)
		: path(new FilePath(path)), autoDeletePath(true)
#ifdef linux
		  , dir(NULL)
#endif

{
}


File::File(FilePath* path, bool autoDeletePath)
		: path(path), autoDeletePath(autoDeletePath)
#ifdef linux
		  , dir(NULL)
#endif
{
}


File::File(const File& other)
		: path(new FilePath(*other.path)), autoDeletePath(true)
#ifdef linux
		  , dir(NULL)
#endif
{
}


File::File(const File& parent, const char* child)
		: path(NULL), autoDeletePath(true)
#ifdef linux
		  , dir(NULL)
#endif
{
	if (!parent.isDirectory()) {
		FileContentType type = parent.guessContentType();

		if (type != CONTENT_TYPE_DIR  &&  type != CONTENT_TYPE_IMG) {
			char* errmsg = new char[strlen(parent.getPath()->toString()) + 128];
			sprintf(errmsg, "Attempt to call File::File(const File& parent, const char* child) with non-directory '%s' as parent",
					parent.getPath()->toString());
			FileException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
	}

	path = new FilePath(*parent.path, child);
}


File::~File()
{
	if (autoDeletePath) {
		delete path;
	}

#ifdef linux
	if (dir != NULL) {
		closedir(dir);
	}
#endif
}


bool File::physicallyExists() const
{
#ifdef linux
	struct stat fileInfo;
	return stat(path->toString(), &fileInfo) == 0;
#else
	DWORD attribs = GetFileAttributes(path->toString());
	return attribs != 0xFFFFFFFF;
#endif
}


bool File::exists() const
{
	if (physicallyExists()) {
		return true;
	} else {
		if (path->isIMGPath()) {
			try {
				File* parent = getParent();
				IMGArchive archive(*parent);
				delete parent;

				if (archive.getEntryByName(path->getFileName()) != NULL) {
					return true;
				} else {
					return false;
				}
			} catch (Exception& ex) {
				char* errMsg = new char[strlen(path->toString()) + 64];
				sprintf(errMsg, "Exception thrown during existence check of IMG entry %s.", path->toString());
				FileException fex(errMsg, __FILE__, __LINE__, &ex);
				delete[] errMsg;
				throw fex;
			}
		} else {
			return false;
		}
	}
}


FileType File::getType() const
{
	if (!physicallyExists()) {
		if (path->isIMGPath()) {
			return TYPE_FILE;
		}
	}

#ifdef linux
	struct stat fileInfo;

	if (stat(path->toString(), &fileInfo) != 0) {
		return TYPE_ERROR;
	}

	if (S_ISDIR(fileInfo.st_mode)) {
		return TYPE_DIRECTORY;
	} else if (S_ISREG(fileInfo.st_mode)) {
		return TYPE_FILE;
	} else if (S_ISLNK(fileInfo.st_mode)) {
		return TYPE_LINK;
	} else {
		return TYPE_OTHER;
	}
#else
	//#error "File::getType() not supported on this platform!"
	DWORD attribs = GetFileAttributes(path->toString());

	if (attribs == INVALID_FILE_ATTRIBUTES) {
		return TYPE_ERROR;
	}

	if ((attribs & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		return TYPE_DIRECTORY;
	} else {
		return TYPE_FILE;
	}
#endif
}


FileIterator* File::getIterator() const
{
	return new FileIterator(this);
}


File* File::getParent() const
{
	FilePath* parent = path->getParentPath();

	if (parent == NULL) {
		return NULL;
	}

	return new File(parent);
}


FileContentType File::guessContentType() const
{
	return path->guessContentType();
}


InputStream* File::openStream(int flags) const
{
	if (physicallyExists()) {
		return new FileInputStream(*this, flags);
	} else {
		if (path->isIMGPath()) {
			try {
				File* parent = getParent();
				InputStream* stream = parent->openStream(STREAM_BINARY);
				IMGArchive archive(stream);
				delete parent;
				InputStream* rstream = archive.gotoEntry(path->getFileName(), true);

				if (rstream != NULL) {
					return rstream;
				} else {
					delete rstream;
					delete stream;
					return NULL;
				}
			} catch (Exception& ex) {
				char* errMsg = new char[strlen(path->toString()) + 64];
				sprintf(errMsg, "Exception thrown during stream opening of IMG entry %s.", path->toString());
				FileException fex(errMsg, __FILE__, __LINE__, &ex);
				delete[] errMsg;
				throw fex;
			}
		} else {
			return NULL;
		}
	}
}


File* File::getChild(int childIdx) const
{
	if (physicallyExists()) {
		if (isRegularFile()) {
			FileContentType type = guessContentType();

			if (type == CONTENT_TYPE_DIR  ||  type == CONTENT_TYPE_IMG) {
				try {
					IMGArchive archive(*this);

					if (childIdx >= archive.getEntryCount()) {
						return NULL;
					}

					return new File(*this, archive.getEntries()[childIdx]->name);
				} catch (Exception& ex) {
					char* errMsg = new char[strlen(path->toString()) + 64];
					sprintf(errMsg, "Exception thrown during child count of IMG archive %s.", path->toString());
					FileException fex(errMsg, __FILE__, __LINE__, &ex);
					delete[] errMsg;
					throw fex;
				}
			} else {
				char* errmsg = new char[strlen(path->toString())+128];
				sprintf(errmsg, "Called getChild(int) with a regular non-archive file: '%s'", path->toString());
				FileException ex(errmsg, __FILE__, __LINE__);
				delete[] errmsg;
				throw ex;
			}
		} else {
			FileIterator* it = getIterator();
			File* nextFile = NULL;

			for (int i = 0 ; i <= childIdx ; i++) {
				if (nextFile != NULL) {
					delete nextFile;
				}

				nextFile = it->next();

				if (nextFile == NULL) {
					break;
				}
			}

			delete it;

			return nextFile;
		}
	} else {
		char* errmsg = new char[strlen(path->toString())+128];
		sprintf(errmsg, "Called getChild(int) with a non-existant file: '%s'", path->toString());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
}


int File::getChildCount(bool recursive) const
{
	if (isRegularFile()) {
		FileContentType type = guessContentType();

		if (type != CONTENT_TYPE_DIR  &&  type != CONTENT_TYPE_IMG) {
			return 0;
		}
	}

	FileIterator* it = getIterator();

	int i = 0;
	File* child;

	while ((child = it->next())  !=  NULL) {
		if (recursive) {
			i += child->getChildCount(true);
		}

		delete child;
		i++;
	}

	delete it;

	return i;
}


int File::indexOf(const File& other) const
{
	FileIterator* it = getIterator();

	int i = 0;
	File* child;

	while ((child = it->next())  !=  NULL) {
		if (strcmp(child->getPath()->toString(), other.getPath()->toString()) == 0) {
			delete child;
			delete it;
			return i;
		}

		i++;
		delete child;
	}

	delete it;

	return -1;
}


int File::getDirectoryIndex() const
{
	File* parent = getParent();

	if (parent == NULL) {
		return -1;
	}

	int index = parent->indexOf(*this);
	delete parent;
	return index;
}


bool File::operator==(const File& other) const
{
	return *path == *other.path;
}


bool File::operator>(const File& other) const
{
	return *path > *other.path;
}


bool File::operator<(const File& other) const
{
	return *path < *other.path;
}


bool File::isArchiveFile() const
{
	if (!isRegularFile()) {
		return false;
	}

	FileContentType type = guessContentType();
	return type == CONTENT_TYPE_IMG  ||  type == CONTENT_TYPE_DIR;
}


File::filesize File::getSize() const
{
	if (!physicallyExists()) {
		if (path->isIMGPath()) {
			File* imgFile = getParent();

			try {
				IMGArchive* img = new IMGArchive(*imgFile);
				IMGEntry* entry = img->getEntryByName(path->getFileName());
				filesize size = entry->size * IMG_BLOCK_SIZE;
				delete img;
				delete imgFile;
				return size;
			} catch (Exception& ex) {
				char* errMsg = new char[strlen(path->toString()) + 64];
				sprintf(errMsg, "Exception thrown during size retrieval of IMG entry %s.", path->toString());
				FileException fex(errMsg, __FILE__, __LINE__, &ex);
				delete[] errMsg;
				throw fex;
			}
		} else {
			char* errMsg = new char[strlen(path->toString()) + 64];
			sprintf(errMsg, "Attempt to get size of non-existent file %s", path->toString());
			FileException ex(errMsg);
			delete[] errMsg;
			throw ex;
		}
	}

#ifdef linux
	struct stat fileInfo;

	if (stat(path->toString(), &fileInfo) != 0) {
		char* errStr = strerror(errno);
		char* errMsg = new char[strlen(errStr) + strlen(path->toString()) + 64];
		sprintf(errMsg, "Internal error receiving size of file %s: %s", path->toString(), errStr);
		FileException ex(errMsg, __FILE__, __LINE__);
		delete[] errMsg;
		throw ex;
	}

	return fileInfo.st_size;
#else
	InputStream* stream = openStream(STREAM_BINARY);
	stream->seek(0, InputStream::STREAM_SEEK_END);
	InputStream::streampos size = stream->tell();
	delete stream;
	return size;
#endif
}


bool File::mkdir() const
{
#ifdef linux
	return ::mkdir(path->toString(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#else
	return CreateDirectory(path->toString()) != 0;
#endif
}


bool File::isChildOf(const File& other) const
{
	return path->isChildOf(*other.path);
}

