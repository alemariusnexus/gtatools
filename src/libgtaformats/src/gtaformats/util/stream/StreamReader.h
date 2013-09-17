/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef STREAMREADER_H_
#define STREAMREADER_H_

#include <istream>

using std::istream;



class StreamReader
{
public:
	StreamReader(istream* stream) : s(stream) {}

	virtual void readU8(uint8_t* dest) { s->read((char*) dest, 1); }
	virtual void readU16(uint16_t* dest) { s->read((char*) dest, 2); }
	virtual void readU32(uint32_t* dest) { s->read((char*) dest, 4); }
	virtual void readU64(uint64_t* dest) { s->read((char*) dest, 8); }
	virtual void read8(int8_t* dest) { s->read((char*) dest, 1); }
	virtual void read16(int16_t* dest) { s->read((char*) dest, 2); }
	virtual void read32(int32_t* dest) { s->read((char*) dest, 4); }
	virtual void read64(int64_t* dest) { s->read((char*) dest, 8); }
	virtual void readFloat(float* dest) { s->read((char*) dest, 4); }
	virtual void readDouble(double* dest) { s->read((char*) dest, 8); }

	virtual void readArrayU8(uint8_t* dest, size_t num) { s->read((char*) dest, num); }
	virtual void readArrayU16(uint16_t* dest, size_t num) { s->read((char*) dest, num*2); }
	virtual void readArrayU32(uint32_t* dest, size_t num) { s->read((char*) dest, num*4); }
	virtual void readArrayU64(uint64_t* dest, size_t num) { s->read((char*) dest, num*8); }
	virtual void readArray8(int8_t* dest, size_t num) { s->read((char*) dest, num); }
	virtual void readArray16(int16_t* dest, size_t num) { s->read((char*) dest, num*2); }
	virtual void readArray32(int32_t* dest, size_t num) { s->read((char*) dest, num*4); }
	virtual void readArray64(int64_t* dest, size_t num) { s->read((char*) dest, num*8); }
	virtual void readArrayFloat(float* dest, size_t num) { s->read((char*) dest, num*4); }
	virtual void readArrayDouble(double* dest, size_t num) { s->read((char*) dest, num*8); }

	virtual uint8_t readU8() { uint8_t v; readU8(&v); return v; }
	virtual uint16_t readU16() { uint16_t v; readU16(&v); return v; }
	virtual uint32_t readU32() { uint32_t v; readU32(&v); return v; }
	virtual uint64_t readU64() { uint64_t v; readU64(&v); return v; }
	virtual int8_t read8() { int8_t v; read8(&v); return v; }
	virtual int16_t read16() { int16_t v; read16(&v); return v; }
	virtual int32_t read32() { int32_t v; read32(&v); return v; }
	virtual int64_t read64() { int64_t v; read64(&v); return v; }
	virtual float readFloat() { float v; readFloat(&v); return v; }
	virtual double readDouble() { double v; readDouble(&v); return v; }

protected:
	istream* s;
};

#endif /* STREAMREADER_H_ */
