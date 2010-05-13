/*
 * util.cpp
 *
 *  Created on: 24.04.2010
 *      Author: alemariusnexus
 */

#include <cctype>


void strtolower(char* dest, const char* src)
{
	int i;
	for (i = 0 ; src[i] != '\0' ; i++) {
		dest[i] = tolower(src[i]);
	}

	dest[i] = '\0';
}
