/*
 * FormatManager.cpp
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
 */

#include "FormatManager.h"
#include "impl/IMGFormatHandler.h"



FormatManager::FormatManager()
{
	registerFormatHandler(new IMGFormatHandler);
}


FormatManager* FormatManager::getInstance()
{
	static FormatManager* manager = new FormatManager;
	return manager;
}


void FormatManager::registerFormatHandler(FormatHandler* handler)
{
	handlers << handler;
}


FormatHandler* FormatManager::getHandler(const File& file, FormatHandler::features feature, bool fromContent)
{
	HandlerList::iterator it;

	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		FormatHandler* handler = *it;
		if ((feature & handler->getFileFeatures(file, fromContent)) == feature) {
			return handler;
		}
	}

	return NULL;
}
