/*
 * TXDFormatHandler.h
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#ifndef DFFFORMATHANDLER_H_
#define DFFFORMATHANDLER_H_

#include "../FormatHandler.h"

class DFFFormatHandler: public FormatHandler {
	Q_OBJECT

public:
	virtual features getFileFeatures(const File& file, bool fromContent);
	virtual QString getFileFormatName(const File& file, bool fromContent = true);
};

#endif /* DFFFORMATHANDLER_H_ */
