/*
 * FileIterator.cpp
 *
 *  Created on: 04.07.2010
 *      Author: alemariusnexus
 */

#include "FileIterator.h"
#include "FileException.h"
#include <cstring>
#include "../img/IMGArchive.h"

#ifdef linux
#include <errno.h>
#endif



FileIterator::FileIterator(const File* file)
		: iteratedDir(file), archive(NULL), archiveIdx(0)
#ifdef linux
		  , dir(NULL)
#else
		  , dirHandle(NULL)
#endif
{
	if (file->physicallyExists()) {
		if (file->isRegularFile()) {
			FileContentType type = file->guessContentType();

			if (type == CONTENT_TYPE_DIR  ||  type == CONTENT_TYPE_IMG) {
				archive = new IMGArchive(*file);
			} else {
				throw FileException("Attempt to iterate over regular non-archive file!", __FILE__, __LINE__);
			}
		} else {
#ifdef linux
			dir = opendir(file->getPath()->toString());

			if (dir == NULL) {
				char* errStr = strerror(errno);
				char* errMsg = new char[strlen(errStr) + strlen(file->getPath()->toString()) + 128];
				sprintf(errMsg, "Internal error opening file %s for iteration: %s", file->getPath()->toString(), errStr);
				FileException ex(errMsg, __FILE__, __LINE__);
				delete[] errMsg;
				throw ex;
			}
#else
			WIN32_FIND_DATA fdata;
			char* pattern = new char[strlen(file->getPath()->toString())+3];
			strcpy(pattern, file->getPath()->toString());
			strcat(pattern, "/*");
			dirHandle = FindFirstFile(pattern, &fdata);

			while(strcmp(fdata.cFileName, ".") == 0  ||  strcmp(fdata.cFileName, "..") == 0) {
				FindNextFile(dirHandle, &fdata);
			}

			nextFile = new File(*file, fdata.cFileName);

#endif
		}
	} else {
		char* errmsg = new char[strlen(file->getPath()->toString())+128];
		sprintf(errmsg, "Attempt to iterate over non-existant file '%s'!", file->getPath()->toString());
		FileException ex(errmsg, __FILE__, __LINE__);
		delete[] errmsg;
		throw ex;
	}
}


FileIterator::~FileIterator()
{
	if (archive != NULL) {
		delete archive;
	}

#ifdef linux
	if (dir != NULL) {
		closedir(dir);
	}
#else
	FindClose(dirHandle);

	if (nextFile != NULL) {
		delete nextFile;
	}
#endif
}


File* FileIterator::next()
{
	if (isRealDirectory()) {
#ifdef linux
		struct dirent* entry;

		while ((entry = readdir(dir))  !=  NULL) {
			if (strcmp(entry->d_name, ".") != 0  &&  strcmp(entry->d_name, "..") != 0) {
				break;
			}
		}

		if (entry == NULL) {
			return NULL;
		}

		File* nextFile = new File(*iteratedDir, entry->d_name);
		return nextFile;
#else
		File* retFile = nextFile;

		WIN32_FIND_DATA fdata;

		while (true) {
			if (FindNextFile(dirHandle, &fdata) == 0) {
				nextFile = NULL;
			} else {
				if (strcmp(fdata.cFileName, ".") == 0  ||  strcmp(fdata.cFileName, "..") == 0) {
					continue;
				}

				nextFile = new File(*iteratedDir, fdata.cFileName);
			}

			break;
		}

		return retFile;
#endif
	} else {
		IMGEntry** entries = archive->getEntries();

		if (archiveIdx >= archive->getEntryCount()) {
			return NULL;
		}

		IMGEntry* entry = entries[archiveIdx++];

		File* nextFile = new File(*iteratedDir, entry->name);
		return nextFile;
	}
}

