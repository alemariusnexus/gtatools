/*
 * CRC32.h
 *
 *  Created on: 20.12.2011
 *      Author: alemariusnexus
 */

#ifndef CRC32_H_
#define CRC32_H_

#include <gtaformats/config.h>
#include <cstring>


class CRC32
{
public:
	CRC32() : checksum(0xFFFFFFFF) {}
	void clear() { checksum = 0xFFFFFFFF; }
	void append(const char* data, size_t len);
	void append(const char* data) { append(data, strlen(data)); }
	uint32_t getChecksum() const { return checksum; }

private:
	uint32_t checksum;
};

#endif /* CRC32_H_ */
