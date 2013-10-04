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

#ifndef UTIL_H_
#define UTIL_H_

#include <gtaformats/config.h>
#include <cstdlib>


#define PS() uint64_t psS = GetTickcount();
#define PE(n) uint64_t psE = GetTickcount(); printf("%s took %d\n", (n), (int) (psE-psS));


//#ifdef __GNUC__
#if defined(__GNUC__)  &&  (__GNUC__ > 4  ||  (__GNUC__ == 4  &&  __GNUC_MINOR__ >= 3))

#define SwapEndianness16(v) ((((v) >> 8) & 0xFF)  |  (((v) << 8) & 0xFF00))
#define SwapEndianness32(v) (__builtin_bswap32((v)))
#define SwapEndianness64(v) (__builtin_bswap64((v)))

#elif defined(_MSC_VER)

#define SwapEndianness16(v) (_byteswap_ushort((v)))
#define SwapEndianness32(v) (_byteswap_ulong((v)))
#define SwapEndianness64(v) (_byteswap_uint64((v)))

#else

#define SwapEndianness16(v) ((((v) >> 8) & 0xFF)  |  (((v) << 8) & 0xFF00))

#define SwapEndianness32(v) ( \
		(((v) >> 24) & 0xFF) \
		| (((v) >> 8) & 0xFF00) \
		| (((v) << 8) & 0xFF0000) \
		| (((v) << 24) & 0xFF000000))

#define SwapEndianness64(v) ( \
		(((v) >> 56) & 0xFFULL) \
		| (((v) >> 40) & 0xFF00ULL) \
		| (((v) >> 24) & 0xFF0000ULL) \
		| (((v) >> 8) & 0xFF000000ULL) \
		| (((v) << 8) & 0xFF00000000ULL) \
		| (((v) << 24) & 0xFF0000000000ULL) \
		| (((v) << 40) & 0xFF000000000000ULL) \
		| (((v) << 56) & 0xFF00000000000000ULL))

#endif

inline float SwapEndiannessF32(float val)
{
	uint32_t uswp = SwapEndianness32(*((uint32_t*) &val));
	return *((float*) &uswp);
}


inline double SwapEndiannessF64(double val)
{
	uint64_t uswp = SwapEndianness64(*((uint64_t*) &val));
	return *((double*) &uswp);
}


#ifdef GTAFORMATS_LITTLE_ENDIAN

#define ToLittleEndian16(v) (v)
#define ToLittleEndian32(v) (v)
#define ToLittleEndian64(v) (v)
#define ToLittleEndianF32(v) (v)
#define ToLittleEndianF64(v) (v)

#define ToBigEndian16(v) (SwapEndianness16((v)))
#define ToBigEndian32(v) (SwapEndianness32((v)))
#define ToBigEndian64(v) (SwapEndianness64((v)))
#define ToBigEndianF32(v) (SwapEndiannessF32((v)))
#define ToBigEndianF64(v) (SwapEndiannessF64((v)))

#define FromLittleEndian16(v) (v)
#define FromLittleEndian32(v) (v)
#define FromLittleEndian64(v) (v)
#define FromLittleEndianF32(v) (v)
#define FromLittleEndianF64(v) (v)

#define FromBigEndian16(v) (SwapEndianness16((v)))
#define FromBigEndian32(v) (SwapEndianness32((v)))
#define FromBigEndian64(v) (SwapEndianness64((v)))
#define FromBigEndianF32(v) (SwapEndiannessF32((v)))
#define FromBigEndianF64(v) (SwapEndiannessF64((v)))

#else

#define ToLittleEndian16(v) (SwapEndianness16((v)))
#define ToLittleEndian32(v) (SwapEndianness32((v)))
#define ToLittleEndian64(v) (SwapEndianness64((v)))
#define ToLittleEndianF32(v) (SwapEndiannessF32((v)))
#define ToLittleEndianF64(v) (SwapEndiannessF64((v)))

#define ToBigEndian16(v) (v)
#define ToBigEndian32(v) (v)
#define ToBigEndian64(v) (v)
#define ToBigEndianF32(v) (v)
#define ToBigEndianF64(v) (v)

#define FromLittleEndian16(v) (SwapEndianness16((v)))
#define FromLittleEndian32(v) (SwapEndianness32((v)))
#define FromLittleEndian64(v) (SwapEndianness64((v)))
#define FromLittleEndianF32(v) (SwapEndiannessF32((v)))
#define FromLittleEndianF64(v) (SwapEndiannessF64((v)))

#define FromBigEndian16(v) (v)
#define FromBigEndian32(v) (v)
#define FromBigEndian64(v) (v)
#define FromBigEndianF32(v) (v)
#define FromBigEndianF64(v) (v)

#endif



uint64_t GetTickcount();
uint64_t GetTickcountMicroseconds();

float RandomFloat(float min, float max);

bool RandomBool();

bool IsTimeBetween(int8_t timeH, int8_t timeM, int8_t startH, int8_t startM, int8_t endH, int8_t endM);

inline unsigned int GetNextPowerOfTwo(unsigned int v)
{
	v--;
	for (unsigned int i = 1 ; i < sizeof(unsigned int) * 8 ; i <<= 1)
		v |= v >> i;
	return v+1;
}

inline unsigned int RoundToMultiple(unsigned int val, unsigned int multiple)
{
	if (multiple == 0)
		return val;

	unsigned int rem = val % multiple;

	if (rem == 0)
		return val;

	return val + multiple - rem;
}

void SleepMilliseconds(unsigned int time);

#endif /* UTIL_H_ */
