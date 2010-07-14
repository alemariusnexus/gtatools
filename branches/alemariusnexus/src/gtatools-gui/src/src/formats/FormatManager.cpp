/*
 * FormatManager.cpp
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
 */

#include "FormatManager.h"
#include "impl/IMGFormatHandler.h"
#include "impl/TXDFormatHandler.h"
#include "impl/IPLFormatHandler.h"
#include "impl/IDEFormatHandler.h"
#include "impl/DFFFormatHandler.h"



FormatManager::FormatManager()
{
	registerFormatHandler(new IMGFormatHandler);
	registerFormatHandler(new TXDFormatHandler);
	registerFormatHandler(new IPLFormatHandler);
	registerFormatHandler(new IDEFormatHandler);
	registerFormatHandler(new DFFFormatHandler);
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


QLinkedList<FormatHandler*> FormatManager::getHandlers(const File& file, FormatHandler::features feature, bool fromContent)
{
	QLinkedList<FormatHandler*> list;
	HandlerList::iterator it;

	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		FormatHandler* handler = *it;
		if ((feature & handler->getFileFeatures(file, fromContent)) == feature) {
			list << handler;
		}
	}

	return list;
}
