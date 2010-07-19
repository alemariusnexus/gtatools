/*
 * FormatManager.cpp
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
 */

#include "FormatManager.h"
#include "img/IMGFormatHandler.h"
#include "txd/TXDFormatHandler.h"
#include "ipl/IPLFormatHandler.h"
#include "ide/IDEFormatHandler.h"
#include "dff/DFFFormatHandler.h"



FormatManager::FormatManager()
{
	registerFormatHandler(TXDFormatHandler::getInstance());
	registerFormatHandler(new IMGFormatHandler);
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


QLinkedList<FormatHandler*> FormatManager::getHandlers(const File& file)
{
	QLinkedList<FormatHandler*> list;

	HandlerList::iterator it;
	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		FormatHandler* handler = *it;
		if (handler->hasFileFormat(file)) {
			list << handler;
		}
	}

	return list;
}


FormatHandler* FormatManager::getHandler(const File& file)
{
	HandlerList::iterator it;

	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		FormatHandler* handler = *it;
		if (handler->hasFileFormat(file)) {
			return handler;
		}
	}

	return NULL;
}
