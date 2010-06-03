/*
 * util.cpp
 *
 *  Created on: 24.04.2010
 *      Author: alemariusnexus
 */

#include <cctype>

#ifdef linux
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif


void strtolower(char* dest, const char* src)
{
	int i;
	for (i = 0 ; src[i] != '\0' ; i++) {
		dest[i] = tolower(src[i]);
	}

	dest[i] = '\0';
}


bool isDirectory(const char* path)
{
	struct stat data;
	stat(path, &data);

	if (S_ISDIR(data.st_mode)) {
		return true;
	} else {
		return false;
	}
}


