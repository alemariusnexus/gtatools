/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef RESOURCEDATLOADER_H_
#define RESOURCEDATLOADER_H_

#include <nxcommon/file/File.h>
#include <istream>

using std::istream;



class ResourceDATLoader
{
public:
	enum EntryType
	{
		IDE,
		IPL,
		IMG,
		TXD
	};

	struct Entry
	{
		EntryType type;
		CString rawPath;
		File resolvedFile;
	};

public:
	ResourceDATLoader(const File& rootDir) : rootDir(rootDir) {}
	void setRootDirectory(const File& rootDir) { this->rootDir = rootDir; }
	File getRootDirectory() const { return rootDir; }
	bool loadEntry(istream* stream, Entry& entry);

private:
	File rootDir;
};

#endif /* RESOURCEDATLOADER_H_ */
