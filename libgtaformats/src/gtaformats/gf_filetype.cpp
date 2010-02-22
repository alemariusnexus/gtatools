/*
 * filetype.cpp
 *
 *  Created on: 04.02.2010
 *      Author: alemariusnexus
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
	}

	return GF_TYPE_UNKNOWN;
}


GFFileType GFGuessFileType(istream* stream)
{

}


