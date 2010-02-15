/*
 * ImgCommand.h
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#ifndef IMGVISITOR_H_
#define IMGVISITOR_H_

#include "gf_config.h"
#include "IMGArchive.h"
#include <istream>

using std::istream;

struct IMGEntry;

class IMGVisitor {
public:
	virtual bool readHeader(IMGEntry* header, void*& udata) = 0;
	virtual void readEntry(IMGEntry* entry, istream* stream, void*& udata) = 0;
};

#endif /* IMGVISITOR_H_ */
