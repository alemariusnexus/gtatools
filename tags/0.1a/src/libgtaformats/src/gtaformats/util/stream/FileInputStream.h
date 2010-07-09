/*
 * FileInputStream.h
 *
 *  Created on: 15.05.2010
 *      Author: alemariusnexus
 */

#ifndef FILEINPUTSTREAM_H_
#define FILEINPUTSTREAM_H_

#include "STLInputStream.h"
#include "../File.h"


#define STREAM_BINARY (1<<0)



class FileInputStream : public STLInputStream {
public:
	FileInputStream(const File* file, int flags = 0);
	FileInputStream(const char* file, int flags = 0);

private:
	void init(const File* file, int flags);
};

#endif /* FILEINPUTSTREAM_H_ */
