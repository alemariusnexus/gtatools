/*
 * FileInputStream.cpp
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#include "FileInputStream.h"
#include <fstream>

using std::ifstream;


FileInputStream::FileInputStream(const File* file, int flags)
		: STLInputStream(flags)
{
	init(file, flags);
}


FileInputStream::FileInputStream(const char* file, int flags)
		: STLInputStream(flags)
{
	File f(file);
	init(&f, flags);
}


void FileInputStream::init(const File* file, int flags)
{
	istream::openmode mode = istream::in;

	if ((STREAM_BINARY & flags) != 0) {
		mode |= istream::binary;
	}

	setBackend(new ifstream(file->getPath(), mode));
}


