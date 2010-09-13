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

#include "FileInputStream.h"
#include <fstream>

using std::ifstream;


FileInputStream::FileInputStream(const File& file, int flags)
		: STLInputStream(flags)
{
	init(file, flags);
}


FileInputStream::FileInputStream(const char* file, int flags)
		: STLInputStream(flags)
{
	File f(file);
	init(f, flags);
}


void FileInputStream::init(const File& file, int flags)
{
	istream::openmode mode = istream::in;

	if ((STREAM_BINARY & flags) != 0) {
		mode |= istream::binary;
	}

	setBackend(new ifstream(file.getPath()->toString(), mode));
}


