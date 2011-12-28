/*
 * StreamWriter.h
 *
 *  Created on: 22.12.2011
 *      Author: alemariusnexus
 */

#ifndef STREAMWRITER_H_
#define STREAMWRITER_H_

#include <ostream>

using std::ostream;



class StreamWriter
{
public:
	StreamWriter(ostream* stream) : s(stream) {}

	virtual void writeU8(uint8_t v) { s->write((char*) &v, 1); }
	virtual void writeU16(uint16_t v) { s->write((char*) &v, 2); }
	virtual void writeU32(uint32_t v) { s->write((char*) &v, 4); }
	virtual void writeU64(uint64_t v) { s->write((char*) &v, 8); }
	virtual void write8(int8_t v) { s->write((char*) &v, 1); }
	virtual void write16(int16_t v) { s->write((char*) &v, 2); }
	virtual void write32(int32_t v) { s->write((char*) &v, 4); }
	virtual void write64(int64_t v) { s->write((char*) &v, 8); }
	virtual void writeFloat(float v) { s->write((char*) &v, 4); }
	virtual void writeDouble(double v) { s->write((char*) &v, 8); }

	virtual void writeArrayCopyU8(const uint8_t* v, size_t n) { s->write((const char*) v, n); }
	virtual void writeArrayCopyU16(const uint16_t* v, size_t n) { s->write((const char*) v, n*2); }
	virtual void writeArrayCopyU32(const uint32_t* v, size_t n) { s->write((const char*) v, n*4); }
	virtual void writeArrayCopyU64(const uint64_t* v, size_t n) { s->write((const char*) v, n*8); }
	virtual void writeArrayCopy8(const int8_t* v, size_t n) { s->write((const char*) v, n); }
	virtual void writeArrayCopy16(const int16_t* v, size_t n) { s->write((const char*) v, n*2); }
	virtual void writeArrayCopy32(const int32_t* v, size_t n) { s->write((const char*) v, n*4); }
	virtual void writeArrayCopy64(const int64_t* v, size_t n) { s->write((const char*) v, n*8); }
	virtual void writeArrayCopyFloat(const float* v, size_t n) { s->write((const char*) v, n*4); }
	virtual void writeArrayCopyDouble(const double* v, size_t n) { s->write((const char*) v, n*8); }

	virtual void writeArrayU8(uint8_t* v, size_t n) { writeArrayCopyU8(v, n); }
	virtual void writeArrayU16(uint16_t* v, size_t n) { writeArrayCopyU16(v, n); }
	virtual void writeArrayU32(uint32_t* v, size_t n) { writeArrayCopyU32(v, n); }
	virtual void writeArrayU64(uint64_t* v, size_t n) { writeArrayCopyU64(v, n); }
	virtual void writeArray8(int8_t* v, size_t n) { writeArrayCopy8(v, n); }
	virtual void writeArray16(int16_t* v, size_t n) { writeArrayCopy16(v, n); }
	virtual void writeArray32(int32_t* v, size_t n) { writeArrayCopy32(v, n); }
	virtual void writeArray64(int64_t* v, size_t n) { writeArrayCopy64(v, n); }
	virtual void writeArrayFloat(float* v, size_t n) { writeArrayCopyFloat(v, n); }
	virtual void writeArrayDouble(double* v, size_t n) { writeArrayCopyDouble(v, n); }

protected:
	ostream* s;
};

#endif /* STREAMWRITER_H_ */
