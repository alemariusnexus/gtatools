/*
 * IOException.cpp
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#include "IOException.h"

IOException::IOException(const char* message, const char* srcFile, int srcLine)
		: Exception(message, srcFile, srcLine)
{
}
