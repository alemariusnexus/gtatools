/*
 * File.h
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#ifndef FILE_H_
#define FILE_H_

#ifdef linux
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif


class File {
public:
	enum FileType {
		TYPE_ERROR,
		TYPE_FILE,
		TYPE_DIRECTORY
	};

public:
	File(const char* path);
	~File();

	bool exists() const;
	FileType getType() const;
	File* getChild(const char* name) const;
	File* getFirstChild();
	File* getNextChild();
	File* getParent() const;
	const char* getPath() const { return path; }

private:
	char* path;
#ifdef linux
	DIR* dir;
#endif
};

#endif /* FILE_H_ */
