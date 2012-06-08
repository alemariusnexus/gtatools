/*
 * IFPFormatHandler.cpp
 *
 *  Created on: 02.01.2012
 *      Author: alemariusnexus
 */

#include "IFPFormatHandler.h"
#include "IFPWidget.h"
#include "../../DisplayedFile.h"
#include "../../System.h"



IFPFormatHandler::IFPFormatHandler()
{
	guiModule = new IFPGUIModule;
	System::getInstance()->installGUIModule(guiModule);
}


bool IFPFormatHandler::canHandle(const EntityOpenRequest& req) const
{
	QVariant fileVar = req.getAttribute("file");

	if (fileVar.isNull())
		return false;

	File file(fileVar.toString().toLocal8Bit().constData());

	return file.guessContentType() == CONTENT_TYPE_IFP;
}


DisplayedEntity* IFPFormatHandler::openEntity(const EntityOpenRequest& request)
{
	File file(request.getAttribute("file").toString().toLocal8Bit().constData());

	IFPWidget* widget = new IFPWidget(file);

	DisplayedFile* dfile = new DisplayedFile(file, this, widget);

	return dfile;
}
