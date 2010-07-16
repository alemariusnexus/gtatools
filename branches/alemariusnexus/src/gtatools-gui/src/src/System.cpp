/*
 * System.cpp
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#include "System.h"


System* System::getInstance()
{
	static System* inst = new System;
	return inst;
}


void System::openFile(const File& file)
{
	emit fileOpened(file);
}

