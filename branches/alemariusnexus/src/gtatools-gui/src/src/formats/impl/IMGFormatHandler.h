/*
 * IMGFormatHandler.h
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
 */

#ifndef IMGFORMATHANDLER_H_
#define IMGFORMATHANDLER_H_

#include "../FormatHandler.h"


class IMGFormatHandler : public FormatHandler {
public:
	virtual features getFileFeatures(const File& file, bool fromContent);
	virtual QString getFileFormatName(const File& file, bool fromContent = true);
};

#endif /* IMGFORMATHANDLER_H_ */
