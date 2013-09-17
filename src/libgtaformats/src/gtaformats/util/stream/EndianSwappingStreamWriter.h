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

#ifndef ENDIANSWAPPINGSTREAMWRITER_H_
#define ENDIANSWAPPINGSTREAMWRITER_H_

#include "StreamWriter.h"
#include "../util.h"




class EndianSwappingStreamWriter : public StreamWriter
{
public:
	EndianSwappingStreamWriter(ostream* stream) : StreamWriter(stream) {}

	virtual void writeU16(uint16_t v) { v = SwapEndianness16(v); s->write((char*) &v, 2); }
	virtual void writeU32(uint32_t v) { v = SwapEndianness32(v); s->write((char*) &v, 4); }
	virtual void writeU64(uint64_t v) { v = SwapEndianness64(v); s->write((char*) &v, 8); }
	virtual void write16(int16_t v) { v = SwapEndianness16(v); s->write((char*) &v, 2); }
	virtual void write32(int32_t v) { v = SwapEndianness32(v); s->write((char*) &v, 4); }
	virtual void write64(int64_t v) { v = SwapEndianness64(v); s->write((char*) &v, 8); }
	virtual void writeFloat(float v) { v = SwapEndiannessF32(v); s->write((char*) &v, 4); }
	virtual void writeDouble(double v) { v = SwapEndiannessF64(v); s->write((char*) &v, 8); }

	virtual void writeArrayU16(uint16_t* v, size_t n) { writeArrayU16(v, v, n); }
	virtual void writeArrayU32(uint32_t* v, size_t n) { writeArrayU32(v, v, n); }
	virtual void writeArrayU64(uint64_t* v, size_t n) { writeArrayU64(v, v, n); }
	virtual void writeArray16(int16_t* v, size_t n) { writeArray16(v, v, n); }
	virtual void writeArray32(int32_t* v, size_t n) { writeArray32(v, v, n); }
	virtual void writeArray64(int64_t* v, size_t n) { writeArray64(v, v, n); }
	virtual void writeArrayFloat(float* v, size_t n) { writeArrayFloat(v, v, n); }
	virtual void writeArrayDouble(double* v, size_t n) { writeArrayDouble(v, v, n); }

	virtual void writeArrayCopyU16(const uint16_t* v, size_t n)
			{ uint16_t* vc = new uint16_t[n]; writeArrayU16(v, vc, n); delete[] vc; }
	virtual void writeArrayCopyU32(const uint32_t* v, size_t n)
			{ uint32_t* vc = new uint32_t[n]; writeArrayU32(v, vc, n); delete[] vc; }
	virtual void writeArrayCopyU64(const uint64_t* v, size_t n)
			{ uint64_t* vc = new uint64_t[n]; writeArrayU64(v, vc, n); delete[] vc; }
	virtual void writeArrayCopy16(const int16_t* v, size_t n)
			{ int16_t* vc = new int16_t[n]; writeArray16(v, vc, n); delete[] vc; }
	virtual void writeArrayCopy32(const int32_t* v, size_t n)
			{ int32_t* vc = new int32_t[n]; writeArray32(v, vc, n); delete[] vc; }
	virtual void writeArrayCopy64(const int64_t* v, size_t n)
			{ int64_t* vc = new int64_t[n]; writeArray64(v, vc, n); delete[] vc; }
	virtual void writeArrayCopyFloat(const float* v, size_t n)
			{ float* vc = new float[n]; writeArrayFloat(v, vc, n); delete[] vc; }
	virtual void writeArrayCopyDouble(const double* v, size_t n)
			{ double* vc = new double[n]; writeArrayDouble(v, vc, n); delete[] vc; }

private:
	void writeArrayU16(const uint16_t* v, uint16_t* sb, size_t n)
			{ for (size_t i = 0 ; i < n ; i++) sb[i] = SwapEndianness16(v[i]); s->write((char*) sb, n*2); }
	void writeArrayU32(const uint32_t* v, uint32_t* sb, size_t n)
			{ for (size_t i = 0 ; i < n ; i++) sb[i] = SwapEndianness32(v[i]); s->write((char*) sb, n*4); }
	void writeArrayU64(const uint64_t* v, uint64_t* sb, size_t n)
			{ for (size_t i = 0 ; i < n ; i++) sb[i] = SwapEndianness64(v[i]); s->write((char*) sb, n*8); }
	void writeArray16(const int16_t* v, int16_t* sb, size_t n)
			{ for (size_t i = 0 ; i < n ; i++) sb[i] = SwapEndianness16(v[i]); s->write((char*) sb, n*2); }
	void writeArray32(const int32_t* v, int32_t* sb, size_t n)
			{ for (size_t i = 0 ; i < n ; i++) sb[i] = SwapEndianness32(v[i]); s->write((char*) sb, n*4); }
	void writeArray64(const int64_t* v, int64_t* sb, size_t n)
			{ for (size_t i = 0 ; i < n ; i++) sb[i] = SwapEndianness64(v[i]); s->write((char*) sb, n*8); }
	void writeArrayFloat(const float* v, float* sb, size_t n)
			{ for (size_t i = 0 ; i < n ; i++) sb[i] = SwapEndiannessF32(v[i]); s->write((char*) sb, n*4); }
	void writeArrayDouble(const double* v, double* sb, size_t n)
			{ for (size_t i = 0 ; i < n ; i++) sb[i] = SwapEndiannessF64(v[i]); s->write((char*) sb, n*8); }

};

#endif /* ENDIANSWAPPINGSTREAMWRITER_H_ */
