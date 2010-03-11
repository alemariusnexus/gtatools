/*
 * ImgCommand.h
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#ifndef IMGVISITOR_H_
#define IMGVISITOR_H_

#include "../gf_config.h"
#include "IMGArchive.h"
#include <istream>

using std::istream;

struct IMGEntry;


/**	\brief A class implementing the Visitor design pattern used for IMG archives.
 *
 * 	Objects of this class can be used with IMGArchive::visit() and IMGArchive::visitAll(). Each time a
 * 	visitor visits an IMGEntry, it's readHeader() and readEntry() methods are called. readEntry() is only
 * 	called for those entries where readHeader() returns true. You can pass data to readEntry() by setting
 * 	the user data pointer passed as parameter to readHeader().
 */
class IMGVisitor {
public:
	/**	\brief This is called for each entry header.
	 *
	 * 	You can process the header data here. If this method returns true, a jump will be done to the
	 * 	contents of the entry and readEntry() will be called. You can pass a pointer to udata which can be
	 * 	accessed in readEntry.
	 *
	 *	@param header The IMGEntry which is visited.
	 *	@param udata Pointer to custom user data which is available for readEntry().
	 *	@return true if readEntry() should be called for this entry.
	 */
	virtual bool readHeader(IMGEntry* header, void*& udata) = 0;

	/**	\brief This is called for each entry header where readHeader() returned true.
	 *
	 * 	Before this is called, the stream will be set to the beginning of the entry's contents.
	 *
	 *	@param entry The IMGEntry which is visited.
	 *	@param stream The stream you can read the data from. Don't read beyond the end of the entry!
	 *	@param udata The user data set by readHeader(), if any. You are responsible for deleting it.
	 */
	virtual void readEntry(IMGEntry* entry, istream* stream, void*& udata) = 0;
};

#endif /* IMGVISITOR_H_ */
