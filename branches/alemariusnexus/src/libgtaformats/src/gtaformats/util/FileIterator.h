/*
 * FileIterator.h
 *
 *  Created on: 04.07.2010
 *      Author: alemariusnexus
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
