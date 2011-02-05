/*
 * COLFormatHandler.cpp
 *
 *  Created on: 30.01.2011
 *      Author: alemariusnexus
 */

#include "COLFormatHandler.h"
#include "COLWidget.h"



QWidget* COLFormatHandler::createWidgetForFile(const FileOpenRequest& request, QWidget* parent)
{
	return new COLWidget(*request.getFile(), parent);
}
