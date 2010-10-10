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

#ifndef DEFAULTFILEITERATOR_H_
#define DEFAULTFILEITERATOR_H_

#include <cstdlib>

#ifdef linux
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#else
#include <windows.h>
#endif


class IMGArchive;
class File;


/**	\brief Represents a file iterator.
 *
 *	It can be used to iterate over plain directories or over IMG archives (in which case the entries will be listed).
 */
class FileIterator
{
public:
	/**	\brief Destructor.
	 */
	~FileIterator();

	/**	\brief Returns the next file.
	 *
	 * 	@return The next file or NULL if no more files can be retrieved.
	 */
	File* next();

private:
	/**	\brief Creates an iterator over the given directory or IMG archive.
	 *
	 */
	FileIterator(const File* file);

	bool isRealDirectory()
	{
#ifdef linux
		return dir != NULL;
#else
		return dirHandle != NULL;
#endif
	}

private:
	const File* iteratedDir;
	IMGArchive* archive;
	int archiveIdx;
#ifdef linux
		DIR* dir;
#else
	HANDLE dirHandle;
	File* nextFile;
#endif
private:
	friend class File;
};

#endif /* DEFAULTFILEITERATOR_H_ */
