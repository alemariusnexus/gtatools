/*
 * ListCommand.h
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#ifndef LISTCOMMAND_H_
#define LISTCOMMAND_H_

#include <IMGVisitor.h>

class ListVisitor: public IMGVisitor {
public:
	ListVisitor(int argc, char** argv);
	bool readHeader(IMGEntry* header, void*& udata);
	void readEntry(IMGEntry* entry, istream* stream, void*& udata);
};

#endif /* LISTCOMMAND_H_ */
