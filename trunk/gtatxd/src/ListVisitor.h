/*
 * ListCommand.h
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#ifndef LISTCOMMAND_H_
#define LISTCOMMAND_H_

#include <TXDVisitor.h>

class ListVisitor : public TXDVisitor {
public:
	ListVisitor(int argc, char** argv);
	bool handleHeader(TXDTexture* header, void*& udata);
	bool handleTexture(TXDTexture* header, uint8_t* bmp, void*& udata);

	void setArchive(TXDArchive* archive) { this->archive = archive; }

private:
	TXDArchive* archive;
	bool useCommaAsSeparator;
};

#endif /* LISTCOMMAND_H_ */
