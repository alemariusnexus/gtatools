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

#ifndef FILETYPE_H_
#define FILETYPE_H_

#include "gf_config.h"
#include <string>

using std::string;


enum GFFileType {
	GF_TYPE_UNKNOWN,

	GF_TYPE_IMG,
	GF_TYPE_TXD,
	GF_TYPE_DIR,
	GF_TYPE_DFF,
	GF_TYPE_IDE
};


GFFileType GFGuessFileType(const string& filename);

#endif /* FILETYPE_H_ */
