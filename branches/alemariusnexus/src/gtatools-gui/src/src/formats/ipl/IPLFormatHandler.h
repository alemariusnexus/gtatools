/*
 * TXDFormatHandler.h
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#ifndef IPLFORMATHANDLER_H_
#define IPLFORMATHANDLER_H_

#include "../FormatHandler.h"

class IPLFormatHandler: public FormatHandler {
	Q_OBJECT

public:
	virtual features getFileFeatures(const File& file, bool fromContent);
	virtual QString getFileFormatName(const File& file, bool fromContent = true);
};

#endif /* IPLFORMATHANDLER_H_ */
