/*
 * IOException.h
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#ifndef IOEXCEPTION_H_
#define IOEXCEPTION_H_

#include "../Exception.h"



class IOException : public Exception {
public:
	IOException(const char* message, const char* srcFile = NULL, int srcLine = -1);
};

#endif /* IOEXCEPTION_H_ */
