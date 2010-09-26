/*
 * GXTFormatHandler.cpp
 *
 *  Created on: 26.09.2010
 *      Author: alemariusnexus
 */

#include "GXTFormatHandler.h"



bool GXTFormatHandler::hasFileFormat(const File& file) const
{
	return QString(file.getPath()->getExtension()).toLower().compare("gxt") == 0;
}
