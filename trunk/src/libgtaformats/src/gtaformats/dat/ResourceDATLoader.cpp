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

#include "ResourceDATLoader.h"

using std::streamoff;



bool ResourceDATLoader::loadEntry(istream* stream, Entry& entry)
{
	char line[4096];

	while (!stream->eof()) {
		stream->getline(line, sizeof(line));
		streamoff numChars = stream->gcount();

		if (numChars >= 2  &&  line[numChars-2] == '\r') {
			line[numChars-2] = '\0';
		}


		if (line[0] == '\0') {
			continue;
		}

		if (strncmp(line, "IDE", 3) == 0) {
			CString rawPath = CString(line+4);
			rawPath.trim();

			File file(rawPath, FilePath::Windows);
			file = file.correctCase(rootDir);

			entry.type = IDE;
			entry.rawPath = rawPath;
			entry.resolvedFile = file;

			return true;
		} else if (strncmp(line, "IPL", 3) == 0) {
			CString rawPath = CString(line+4);
			rawPath.trim();

			File file(rawPath, FilePath::Windows);
			file = file.correctCase(rootDir);

			entry.type = IPL;
			entry.rawPath = rawPath;
			entry.resolvedFile = file;

			return true;

			/*FilePath path(*rootDir.getPath(), line+4, FilePath::BackslashAsSeparator | FilePath::CorrectCase);
			File ipl(&path, false);
			iplRecurse(&ipl, rootDir, gameInfo);*/
		} else if (strncmp(line, "IMG", 3) == 0) {
			CString rawPath = CString(line+4);
			rawPath.trim();

			File file(rawPath, FilePath::Windows);
			file = file.correctCase(rootDir);

			entry.type = IMG;
			entry.rawPath = rawPath;
			entry.resolvedFile = file;

			return true;
		} else if (strncmp(line, "TEXDICTION", 10) == 0) {
			CString rawPath = CString(line+11);
			rawPath.trim();

			File file(rawPath, FilePath::Windows);
			file = file.correctCase(rootDir);

			entry.type = TXD;
			entry.rawPath = rawPath;
			entry.resolvedFile = file;

			return true;
		}
	}

	return false;
}
