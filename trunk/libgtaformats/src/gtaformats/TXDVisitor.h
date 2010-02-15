/*
 * TxdCommand.h
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDVISITOR_H_
#define TXDVISITOR_H_

#include "gf_config.h"
#include "TXDArchive.h"

class TXDTexture;
class TXDArchive;

class TXDVisitor {
public:
	TXDVisitor();
	virtual void handleTexture(TXDArchive* archive, TXDTexture* texture) = 0;
};

#endif /* TXDVISITOR_H_ */
