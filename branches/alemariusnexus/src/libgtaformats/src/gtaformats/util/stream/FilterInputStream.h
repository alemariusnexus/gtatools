/*
 * FilterInputStream.h
 *
 *  Created on: 22.08.2010
 *      Author: alemariusnexus
 */

#ifndef FILTERINPUTSTREAM_H_
#define FILTERINPUTSTREAM_H_

#include "InputStream.h"


class FilterInputStream : public InputStream {
public:
	FilterInputStream(InputStream* backend, bool deleteBackend = false);
	virtual ~FilterInputStream();

protected:
	InputStream* backend;

private:
	bool deleteBackend;
};

#endif /* FILTERINPUTSTREAM_H_ */
