/*
 * CString.cpp
 *
 *  Created on: 04.12.2011
 *      Author: alemariusnexus
 */

#include "CString.h"



void CString::copy()
{
	size_t len = length();
	shared_array<char> cpy(new char[len+1]);
	strcpy(cpy.get(), cstr.get());
	cstr = cpy;
}
