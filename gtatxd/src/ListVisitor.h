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
	void handleTexture(TXDArchive* archive, TXDTexture* header);

private:
	bool useCommaAsSeparator;
};

#endif /* LISTCOMMAND_H_ */
