/*
 * ExtractOptions.cpp
 *
 *  Created on: 03.01.2011
 *      Author: alemariusnexus
 */

#include "ExtractOptions.h"



ExtractOptions::~ExtractOptions()
{
	if (destination)
		delete destination;

	delete filter;
}
