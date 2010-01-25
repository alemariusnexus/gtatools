/*
 * TxdCommand.h
 *
 *  Created on: 06.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDVISITOR_H_
#define TXDVISITOR_H_

#include "TXDArchive.h"

class TXDTexture;

class TXDVisitor {
public:
	TXDVisitor();
	virtual bool handleHeader(TXDTexture* header, void*& userData) = 0;
	virtual bool handleTexture(TXDTexture* header, uint8_t* bmp, void*& userData) = 0;
};

#endif /* TXDVISITOR_H_ */
