/*
 * TXDFormatHandler.cpp
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#include "TXDFormatHandler.h"
#include "TXDGUIModule.h"
#include "TXDWidget.h"
#include <cstdio>


QWidget* TXDFormatHandler::createWidgetForFile(const File& file, QWidget* parent)
{
	return new TXDWidget(file, parent);
}
