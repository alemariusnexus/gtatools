/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "DefaultIPLStreamingFileProvider.h"
#include <gtaformats/util/CString.h>
#include <cstring>



void DefaultIPLStreamingFileProvider::addSearchDirectory(const File& file)
{
	searchDirs.push_back(file);
}


void DefaultIPLStreamingFileProvider::findStreamingFiles(const File& file, StreamingFileList& sfiles)
{
	CString fname = file.getPath()->getFileName();

	CString baseName = fname.substr(0, fname.length()-4);

	for (unsigned int i = 0 ; true ; i++) {
		CString sfName(baseName);
		sfName.lower();

		char appendix[32];
		sprintf(appendix, "_stream%u.ipl", i);

		sfName.append(CString(appendix));

		bool found = false;

		for (list<File>::iterator it = searchDirs.begin() ; it != searchDirs.end()  &&  !found ; it++) {
			File sdir = *it;

			File sfile(sdir, sfName);

			if (sfile.exists()) {
				sfiles.push_back(sfile);
				found = true;
				break;
			}
		}

		if (!found)
			break;
	}
}
