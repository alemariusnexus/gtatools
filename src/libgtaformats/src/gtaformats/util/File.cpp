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

#include "File.h"
#include <cstdio>
#include <cstring>
#include "util.h"
#include "FileException.h"
#include "../img/IMGArchive.h"
#include "FileFinder.h"
#include "FileIMGWrapperStream.h"

#ifdef _POSIX_VERSION
#include <errno.h>
#endif




File::File(const CString& path)
		: path(new FilePath(path)), autoDeletePath(true),
		  archivePtr(shared_ptr< shared_ptr<IMGArchive> > (new shared_ptr<IMGArchive>))
{
}


File::File(FilePath* path, bool autoDeletePath)
		: path(path), autoDeletePath(autoDeletePath),
		  archivePtr(shared_ptr< shared_ptr<IMGArchive> > (new shared_ptr<IMGArchive>))
{
}


File::File(const File& other)
		: path(new FilePath(*other.path)), autoDeletePath(true), archivePtr(other.archivePtr)
{
}


File::File(const File& parent, const CString& child)
		: path(NULL), autoDeletePath(true), archivePtr(parent.archivePtr)
{
	if (parent.exists()  &&  !parent.isDirectory()) {
		FileContentType type = parent.guessContentType();

		if (type != CONTENT_TYPE_DIR  &&  type != CONTENT_TYPE_IMG) {
			char* errmsg = new char[parent.getPath()->toString().length() + 128];
			sprintf(errmsg, "Attempt to call File::File(const File& parent, const char* child) with non-directory '%s' as parent",
					parent.getPath()->toString().get());
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
}


bool File::physicallyExists() const
{
#ifdef _POSIX_VERSION
	struct stat fileInfo;
	return stat(path->toString().get(), &fileInfo) == 0;
#elif defined(_WIN32)
	DWORD attribs = GetFileAttributes(path->toString().get());
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
				//IMGArchive archive(*parent);
				shared_ptr<IMGArchive> archive = getIMGArchive();
				delete parent;

				if (archive->getEntryByName(path->getFileName().get()) != archive->getEntryEnd()) {
					return true;
				} else {
					return false;
				}
			} catch (Exception& ex) {
				char* errMsg = new char[path->toString().length() + 64];
				sprintf(errMsg, "Exception thrown during existence check of IMG entry %s.",
						path->toString().get());
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

#ifdef _POSIX_VERSION
	struct stat fileInfo;

	if (stat(path->toString().get(), &fileInfo) != 0) {
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
#elif defined(_WIN32)
	DWORD attribs = GetFileAttributes(path->toString().get());

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

	File* parentFile = new File(parent);
	parentFile->archivePtr = archivePtr;
	return parentFile;
}


FileContentType File::guessContentType() const
{
	return path->guessContentType();
}


istream* File::openInputStream(ifstream::openmode mode) const
{
	if (physicallyExists()) {
		if (isRegularFile()) {
			return new ifstream(path->toString().get(), mode | ifstream::in);
		} else {
			char* errMsg = new char[path->toString().length() + 64];
			sprintf(errMsg, "Attempt to open stream on non-regular file %s.", path->toString().get());
			FileException fex(errMsg, __FILE__, __LINE__);
			delete[] errMsg;
			throw fex;
		}
	} else {
		if (path->isIMGPath()) {
			// TODO Reimplement
			try {
				shared_ptr<IMGArchive> archive = getIMGArchive();
				istream* rstream = archive->gotoEntry(path->getFileName().get(), false);

				if (rstream != NULL) {
					FileIMGWrapperStream<istream>* wrapper
							= new FileIMGWrapperStream<istream>(rstream, archive);
					return wrapper;
				} else {
					return NULL;
				}
			} catch (Exception& ex) {
				char* errMsg = new char[path->toString().length() + 64];
				sprintf(errMsg, "Exception thrown during stream opening of IMG entry %s.",
						path->toString().get());
				FileException fex(errMsg, __FILE__, __LINE__, &ex);
				delete[] errMsg;
				throw fex;
			}
			return NULL;
		} else {
			char* errMsg = new char[path->toString().length() + 64];
			sprintf(errMsg, "Attempt to open stream on non-existant file %s.", path->toString().get());
			FileException fex(errMsg, __FILE__, __LINE__);
			delete[] errMsg;
			throw fex;
		}
	}
}


ostream* File::openOutputStream(ostream::openmode mode) const
{
	return new ofstream(path->toString().get(), mode | ostream::out);
}


iostream* File::openInputOutputStream(iostream::openmode mode) const
{
	if (!physicallyExists()) {
		ofstream tmp(path->toString().get());
		tmp.close();
	}
	return new fstream(path->toString().get(), mode | iostream::out | iostream::in);
}


File* File::getChild(int childIdx) const
{
	if (physicallyExists()) {
		if (isRegularFile()) {
			FileContentType type = guessContentType();

			if (type == CONTENT_TYPE_DIR  ||  type == CONTENT_TYPE_IMG) {
				try {
					shared_ptr<IMGArchive> archive = getIMGArchive();

					if (childIdx >= archive->getEntryCount()) {
						return NULL;
					}

					IMGArchive::EntryIterator it = archive->getEntryBegin();
					for (int i = 0 ; it != archive->getEntryEnd()  &&  i < childIdx ; it++, i++);

					return new File(*this, (*it)->name);
				} catch (Exception& ex) {
					char* errMsg = new char[path->toString().length() + 64];
					sprintf(errMsg, "Exception thrown during child count of IMG archive %s.",
							path->toString().get());
					FileException fex(errMsg, __FILE__, __LINE__, &ex);
					delete[] errMsg;
					throw fex;
				}
			} else {
				char* errmsg = new char[path->toString().length() + 128];
				sprintf(errmsg, "Called getChild(int) with a regular non-archive file: '%s'",
						path->toString().get());
				FileException ex(errmsg, __FILE__, __LINE__);
				delete[] errmsg;
				throw ex;
			}
		} else if (isDirectory()) {
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
		} else {
			char* errmsg = new char[path->toString().length() + 128];
			sprintf(errmsg, "Called getChild(int) with file of invalid type: '%s'", path->toString().get());
			FileException ex(errmsg, __FILE__, __LINE__);
			delete[] errmsg;
			throw ex;
		}
	} else {
		char* errmsg = new char[path->toString().length() + 128];
		sprintf(errmsg, "Called getChild(int) with a non-existant file: '%s'", path->toString().get());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
}


int File::getChildCount(bool recursive, bool archiveEntries) const
{
	if (isRegularFile()) {
		FileContentType type = guessContentType();

		if (type != CONTENT_TYPE_DIR  &&  type != CONTENT_TYPE_IMG) {
			return 0;
		} else {
			if (!recursive  ||  archiveEntries) {
				//IMGArchive img(*this);
				//return img.getEntryCount();
				return getIMGArchive()->getEntryCount();
			} else {
				return 0;
			}
		}
	} else if (!isDirectory()) {
		return 0;
	}

	FileIterator* it = getIterator();

	int i = 0;
	File* child;

	while ((child = it->next())  !=  NULL) {
		if (recursive) {
			i += child->getChildCount(true, archiveEntries);
		}

		delete child;
		i++;
	}

	delete it;

	return i;
}


int File::indexOf(const File& other) const
{
	if (!isDirectory()  &&  !isArchiveFile()) {
		char* errmsg = new char[path->toString().length() + 128];
		sprintf(errmsg, "Called indexOf(const File&) with an invalid file type: '%s'",
				path->toString().get());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

	FileIterator* it = getIterator();

	int i = 0;
	File* child;

	while ((child = it->next())  !=  NULL) {
		if (child->getPath()->toString() == other.getPath()->toString()) {
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


File& File::operator=(const File& other)
{
	if (this == &other) {
		return *this;
	}

	delete path;
	path = new FilePath(*other.path);
	autoDeletePath = true;
	archivePtr = other.archivePtr;
	return *this;
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
	// TODO I disabled this for performance reasons, though I'm not sure if this might cause problems.
	/*if (!isRegularFile()) {
		return false;
	}*/

	FileContentType type = guessContentType();
	return type == CONTENT_TYPE_IMG  ||  type == CONTENT_TYPE_DIR;
}


File::filesize File::getSize() const
{
	if (!physicallyExists()) {
		if (path->isIMGPath()) {
			File* imgFile = getParent();

			try {
				shared_ptr<IMGArchive> img = getIMGArchive();
				const IMGEntry* entry = *img->getEntryByName(path->getFileName().get());
				filesize size = entry->size * IMG_BLOCK_SIZE;
				delete imgFile;
				return size;
			} catch (Exception& ex) {
				char* errMsg = new char[path->toString().length() + 64];
				sprintf(errMsg, "Exception thrown during size retrieval of IMG entry %s.",
						path->toString().get());
				FileException fex(errMsg, __FILE__, __LINE__, &ex);
				delete[] errMsg;
				throw fex;
			}
		} else {
			char* errMsg = new char[path->toString().length() + 64];
			sprintf(errMsg, "Attempt to get size of non-existent file %s", path->toString().get());
			FileException ex(errMsg);
			delete[] errMsg;
			throw ex;
		}
	}

	if (!isRegularFile()) {
		char* errmsg = new char[path->toString().length() + 128];
		sprintf(errmsg, "Attemp to get size of non-regular file: '%s'", path->toString().get());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

#ifdef _POSIX_VERSION
	struct stat fileInfo;

	if (stat(path->toString().get(), &fileInfo) != 0) {
		char* errStr = strerror(errno);
		char* errMsg = new char[strlen(errStr) + path->toString().length() + 64];
		sprintf(errMsg, "Internal error receiving size of file %s: %s", path->toString().get(), errStr);
		FileException ex(errMsg, __FILE__, __LINE__);
		delete[] errMsg;
		throw ex;
	}

	return fileInfo.st_size;
#elif defined(_WIN32)
    istream* stream = openInputStream(ifstream::binary | ifstream::in);
    stream->seekg(0, istream::end);
    filesize size = stream->tellg();
    delete stream;
    return size;
#endif
}


bool File::mkdir() const
{
#ifdef _POSIX_VERSION
	return ::mkdir(path->toString().get(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#elif defined(_WIN32)
	return CreateDirectory(path->toString().get(), NULL) != 0;
#endif
}


bool File::mkdirs() const
{
	File* parent = getParent();

	if (parent != NULL  &&  !parent->exists()) {
		if (!parent->mkdirs()) {
			return false;
		}
	}

	return mkdir();
}


bool File::isChildOf(const File& other, bool recursive) const
{
	return path->isChildOf(*other.path, recursive);
}


File* File::findChild(FileFinder& finder, bool recursive, bool archiveEntries) const
{
	FileIterator* it = getIterator();
	File* child;

	while ((child = it->next())  !=  NULL) {
		if (finder.matches(*child)) {
			delete it;
			return child;
		}

		if (finder.isInterrupted()) {
			delete child;
			return NULL;
		}

		if (recursive  &&  (child->isDirectory()  ||  (archiveEntries  &&  child->isArchiveFile()))) {
			File* match = child->findChild(finder, true, archiveEntries);

			if (match) {
				delete child;
				delete it;
				return match;
			}
		}

		delete child;
	}

	delete it;
	return NULL;
}


int File::findChildren(FileFinder& finder, vector<File*>& results, bool recursive,
		bool archiveEntries) const
{
	int matchCount = 0;

	FileIterator* it = getIterator();
	File* child;

	while ((child = it->next())  !=  NULL) {
		bool matches = finder.matches(*child);
		if (matches) {
			results.push_back(child);
			matchCount++;
		}

		if (finder.isInterrupted()) {
			if (!matches) {
				delete child;
			}
			return matchCount;
		}

		if (recursive  &&  (child->isDirectory()  ||  (archiveEntries  &&  child->isArchiveFile()))) {
			matchCount += child->findChildren(finder, results, true, archiveEntries);
		}

		if (!matches) {
			delete child;
		}
	}

	delete it;
	return matchCount;
}


shared_ptr<IMGArchive> File::getIMGArchive() const
{
	shared_ptr<IMGArchive>* sharedPtr = archivePtr.get();

	if (sharedPtr->get() == NULL) {
		if (path->isIMGPath()) {
			File* parent = getParent();
			parent->getIMGArchive();
			delete parent;
		} else {
			(*sharedPtr) = shared_ptr<IMGArchive>(new IMGArchive(*this));
		}
	}

	return *sharedPtr;
}


File File::createTemporaryFile()
{
	// tmpnam() is dangerous, so we try to use platform-specific functions

#ifdef _POSIX_VERSION
	const char* tmpDir;
#ifdef P_tmpdir
	tmpDir = P_tmpdir;
#else
	tmpDir = getenv("TMPDIR");

	if (!tmpDir) {
		tmpDir = "/tmp";
	}
#endif // P_tmpdir

	char* tmpTemplate = new char[strlen(tmpDir)+16];
	sprintf(tmpTemplate, "%s/gff-XXXXXX", tmpDir);
	close(mkstemp(tmpTemplate));
	File file(tmpTemplate);
	delete[] tmpTemplate;
	return file;
#elif defined(_WIN32)
	DWORD status;
	char tmpPath[512];
	if ((status = GetTempPath(sizeof(tmpPath), tmpPath))  ==  0) {
		char* errmsg = new char[128];
		sprintf(errmsg, "Internal error creating temporary file (in GetTempPath()): %d.", (int) status);
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
	char tmpFilePath[1024];
	if ((status = GetTempFileName(tmpPath, "gff", 0, tmpFilePath))  ==  0) {
		char* errmsg = new char[128];
		sprintf(errmsg, "Internal error creating temporary file (in GetTempFileName()): %d.", (int) status);
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
	return File(tmpFilePath);
#else
	return File(tmpnam());
#endif // _POSIX_VERSION
}


void File::copyTo(const File& newFile) const
{
	istream* inStream = openInputStream(istream::binary);
	newFile.copyFrom(inStream);
}


void File::copyFrom(istream* inStream) const
{
	ostream* outStream = openOutputStream(ostream::binary);

	char buf[8192];

	while (!inStream->eof()) {
		inStream->read(buf, sizeof(buf));
		outStream->write(buf, inStream->gcount());
	}

	inStream->clear();

	delete outStream;
}


void File::copyTo(ostream* stream) const
{
	istream* inStream = openInputStream(istream::binary);

	char buf[8192];

	while (!inStream->eof()) {
		inStream->read(buf, sizeof(buf));
		stream->write(buf, inStream->gcount());
	}

	delete inStream;
}


bool File::remove() const
{
	return ::remove(path->toString().get()) == 0;
}


void File::resize(filesize size) const
{
#ifdef _POSIX_VERSION
	truncate(path->toString().get(), size);
#elif defined(_WIN32)
	HANDLE fhandle = CreateFile(path->toString().get(), GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
	LARGE_INTEGER sizeVal;
	sizeVal.QuadPart = size;
	SetFilePointerEx(fhandle, sizeVal, 0, FILE_BEGIN);
	SetEndOfFile(fhandle);
	CloseHandle(fhandle);
#endif
}


uint64_t File::getModifyTime() const
{
#ifdef _POSIX_VERSION
	struct stat fileInfo;

	if (stat(path->toString().get(), &fileInfo) != 0) {
		char* errStr = strerror(errno);
		char* errmsg = new char[64 + strlen(errStr) + path->toString().length()];
		sprintf(errmsg, "Error getting modification time of file '%s': %s", path->toString().get(), errStr);
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

	return fileInfo.st_mtime*1000;
#else
	HANDLE handle = CreateFile(path->toString().get(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE
			| FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	FILETIME mtime;

	if (GetFileTime(handle, NULL, NULL, &mtime) == 0) {
		char* errmsg = new char[128 + path->toString().length()];
		sprintf(errmsg, "Error getting modification time of file '%s'. Error code: %d",
				path->toString().get(), GetLastError());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}

	CloseHandle(handle);
	uint64_t t =  (((uint64_t) mtime.dwLowDateTime | ((uint64_t) mtime.dwHighDateTime << 32))
			- 116444736000000000) / 10000;
	return t - (t % 1000);
#endif
}
