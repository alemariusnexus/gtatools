/*
 * InputStream.cpp
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#include "InputStream.h"


int InputStream::readLine(char* dest, int len)
{
	for (int i = 0 ; i < len ; i++) {
		read(dest+i, 1);

		if (dest[i] == '\n') {
			dest[i] = '\0';
			return i+1;
		} else if (dest[i] == '\r') {
			dest[i] = '\0';
			seek(1);
			return i+2;
		}
	}

	return -1;
}


void InputStream::skip(int len)
{
	seek(len);
}

