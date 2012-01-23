/*
 * IFPFormatHandler.cpp
 *
 *  Created on: 02.01.2012
 *      Author: alemariusnexus
 */

#include "IFPFormatHandler.h"
#include "IFPWidget.h"
#include "../../DefaultDisplayedFile.h"



DisplayedFile* IFPFormatHandler::openFile(const FileOpenRequest& request)
{
	File* file = request.getFile();

	IFPWidget* widget = new IFPWidget(*file);

	DefaultDisplayedFile* dfile = new DefaultDisplayedFile(*file, this, widget);

	return dfile;

}
