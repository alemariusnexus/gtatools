/*
	Copyright 2010 David Lerch

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

#include "gf_filetype.h"
#include <cstring>
#include <algorithm>
#include <istream>

using std::istream;


GFFileType GFGuessFileType(const string& filename)
{
	string ext = filename.substr(filename.find_last_of('.')+1);
	std::transform(ext.begin(), ext.end(), ext.begin(), tolower);

	if (ext.compare("img") == 0) {
		return GF_TYPE_IMG;
	} else if (ext.compare("txd") == 0) {
		return GF_TYPE_TXD;
	} else if (ext.compare("dir") == 0) {
		return GF_TYPE_DIR;
	} else if (ext.compare("dff") == 0) {
		return GF_TYPE_DFF;
	} else if (ext.compare("ide") == 0) {
		return GF_TYPE_IDE;
	}

	return GF_TYPE_UNKNOWN;
}


/*GFFileType GFGuessFileType(istream* stream)
{

}*/


