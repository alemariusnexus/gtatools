/*
 * TXDFormatHandler.h
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#ifndef IDEFORMATHANDLER_H_
#define IDEFORMATHANDLER_H_

#include "../FormatHandler.h"

class IDEFormatHandler: public FormatHandler {
	Q_OBJECT

public:
	virtual features getFileFeatures(const File& file, bool fromContent);
	virtual QString getFileFormatName(const File& file, bool fromContent = true);
};

#endif /* IDEFORMATHANDLER_H_ */
