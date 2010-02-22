/*
 * filetype.h
 *
 *  Created on: 04.02.2010
 *      Author: alemariusnexus
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
	GF_TYPE_DFF
};


GFFileType GFGuessFileType(const string& filename);

#endif /* FILETYPE_H_ */
