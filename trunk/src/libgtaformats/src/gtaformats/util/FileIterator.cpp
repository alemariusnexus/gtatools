/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "FileIterator.h"
#include "FileException.h"
#include <cstring>
#include "../img/IMGArchive.h"
#include "File.h"

#ifdef _POSIX_VERSION
#include <errno.h>
#endif



FileIterator::FileIterator(const File* file)
		: iteratedDir(file), archive(shared_ptr<IMGArchive>())
#ifdef _POSIX_VERSION
		  , dir(NULL)
#else
		  , dirHandle(NULL), nextFile(NULL)
#endif
{
	if (file->physicallyExists()) {
		if (file->isRegularFile()) {
			FileContentType type = file->guessContentType();

			if (type == CONTENT_TYPE_DIR  ||  type == CONTENT_TYPE_IMG) {
				try {
					//archive = new IMGArchive(*file);
					archive = file->getIMGArchive();
					archiveIt = archive->getEntryBegin();
				} catch (Exception& ex) {
					char* errMsg = new char[file->getPath()->toString().length() + 128];
					sprintf(errMsg, "Exception thrown during initialization of a FileIterator over IMG archive %s.",
							file->getPath()->toString().get());
					FileException fex(errMsg, __FILE__, __LINE__, &ex);
					delete[] errMsg;
					throw fex;
				}
			} else {
				char* errmsg = new char[file->getPath()->toString().length() + 64];
				sprintf(errmsg, "Attempt to iterate over regular non-archive file: %s!",
						file->getPath()->toString().get());
				FileException ex(errmsg, __FILE__, __LINE__);
				delete[] errmsg;
				throw ex;
			}
		} else {
#ifdef _POSIX_VERSION
			dir = opendir(file->getPath()->toString().get());

			if (dir == NULL) {
				char* errStr = strerror(errno);
				char* errMsg = new char[strlen(errStr) + file->getPath()->toString().length() + 128];
				sprintf(errMsg, "Internal error opening file %s for iteration: %s",
						file->getPath()->toString().get(), errStr);
				FileException ex(errMsg, __FILE__, __LINE__);
				delete[] errMsg;
				throw ex;
			}
#else
			WIN32_FIND_DATA fdata;
			char* pattern = new char[file->getPath()->toString().length() + 3];
			strcpy(pattern, file->getPath()->toString().get());
			strcat(pattern, "/*");
			dirHandle = FindFirstFile(pattern, &fdata);

			bool empty = false;

			while(strcmp(fdata.cFileName, ".") == 0  ||  strcmp(fdata.cFileName, "..") == 0) {
				if (FindNextFile(dirHandle, &fdata) == 0) {
					empty = true;
					break;
				}
			}

			if (!empty) {
				nextFile = new File(*file, fdata.cFileName);
			} else {
				nextFile = NULL;
			}
#endif
		}
	} else {
		char* errmsg = new char[file->getPath()->toString().length() + 128];
		sprintf(errmsg, "Attempt to iterate over non-existant file '%s'!", file->getPath()->toString().get());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
}


FileIterator::~FileIterator()
{
#ifdef _POSIX_VERSION
	if (dir != NULL) {
		closedir(dir);
	}
#else
	if (dirHandle != NULL) {
		FindClose(dirHandle);
	}

	if (nextFile != NULL) {
		delete nextFile;
	}
#endif
}


File* FileIterator::next()
{
	if (isRealDirectory()) {
#ifdef _POSIX_VERSION
		struct dirent* entry;

		while ((entry = readdir(dir))  !=  NULL) {
			if (strcmp(entry->d_name, ".") != 0  &&  strcmp(entry->d_name, "..") != 0) {
				break;
			}
		}

		if (entry == NULL) {
			return NULL;
		}

		// Way faster than File(const File&, const char*) because it skips the parent-is-directory
		// check
		FilePath* path = new FilePath(*iteratedDir->getPath(), entry->d_name);
		File* nextFile = new File(path, true);

		return nextFile;
#else
		File* retFile = nextFile;

		WIN32_FIND_DATA fdata;

		while (true) {
			if (FindNextFile(dirHandle, &fdata) == 0) {
				nextFile = NULL;
			} else {
				int len = strlen(fdata.cFileName);

				if (len == 1  &&  fdata.cFileName[0] == '.') {
					continue;
				}
				if (len == 2  &&  fdata.cFileName[0] == '.'  &&  fdata.cFileName[1] == '.') {
					continue;
				}

				// Way faster than File(const File&, const char*) because it skips the parent-is-directory
				// check
				FilePath* path = new FilePath(*iteratedDir->getPath(), fdata.cFileName);
				nextFile = new File(path, true);
			}

			break;
		}

		return retFile;
#endif
	} else {
		try {
			if (archiveIt == archive->getEntryEnd()) {
				return NULL;
			}

			const IMGEntry* entry = *archiveIt++;

			// Way faster than File(const File&, const char*) because it skips the parent-is-directory check
			FilePath* path = new FilePath(*iteratedDir->getPath(), entry->name);
			File* nextFile = new File(path, true);
			nextFile->archivePtr = iteratedDir->archivePtr;

			return nextFile;
		} catch (Exception& ex) {
			char* errMsg = new char[iteratedDir->getPath()->toString().length() + 128];
			sprintf(errMsg, "Exception thrown during iteration over IMG archive %s",
					iteratedDir->getPath()->toString().get());
			FileException fex(errMsg, __FILE__, __LINE__, &ex);
			delete[] errMsg;
			throw fex;
		}
	}
}

