/*
 * FormatManager.h
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
 */

#ifndef FORMATMANAGER_H_
#define FORMATMANAGER_H_

#include "FormatHandler.h"
#include <qlinkedlist.h>


class FormatManager {
private:
	typedef QLinkedList<FormatHandler*> HandlerList;

public:
	static FormatManager* getInstance();

public:
	void registerFormatHandler(FormatHandler* provider);
	FormatHandler* getHandler(const File& file, FormatHandler::features feature, bool fromContent = true);
	QLinkedList<FormatHandler*> getHandlers(const File& file, FormatHandler::features feature, bool fromContent = true);

private:
	FormatManager();

private:
	HandlerList handlers;
};

#endif /* FORMATMANAGER_H_ */
