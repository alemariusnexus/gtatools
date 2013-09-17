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

#include "encoding.h"
#include <iostream>
#include <errno.h>
#include <cstdio>
#include <cstring>
#include <cassert>

#ifdef _HAVE_ICONV
#include <iconv.h>
#elif defined(_WIN32)
#include <windows.h>
#else
#error "ERROR: Platform not supported (neither iconv nor WIN32 available. If you do have iconv, then " \
		"#define _HAVE_ICONV)"
#endif


/**	\brief Conversion table for GXT8 -> ISO-8859-1.
 */
const unsigned char GXT8ToISO88591Table[] = {
		//	-0   -1   -2   -3   -4   -5   -6   -7   -8   -9   -A   -B   -C   -D   -E   -F
			0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F, // 0-
			0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F, // 1-
			0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F, // 2-
			0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F, // 3-
			0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F, // 4-
			0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0x5E,0x5F, // 5-
			0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F, // 6-
			0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0xBA,0x7D,0x7E,0x80, // 7-
			0xC0,0xC1,0xC2,0xC4,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD2,0xD3, // 8-
			0xD4,0xD6,0xD9,0xDA,0xDB,0xDC,0xDF,0xE0,0xE1,0xE2,0xE4,0xE6,0xE7,0xE8,0xE9,0xEA, // 9-
			0xEB,0xEC,0xED,0xEE,0xEF,0xF2,0xF3,0xF4,0xF6,0xF9,0xFA,0xFB,0xFC,0xD1,0xF1,0xBF, // A-
			0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0x7C,0xBB,0xBC,0xBD,0xBE,0xAF, // B-
			0x8A,0x81,0x82,0x83,0xC3,0xC5,0x84,0x85,0x86,0x87,0x88,0x89,0x7F,0x8B,0x8C,0x8D, // C-
			0xD0,0xAD,0x8E,0x8F,0x90,0xD5,0x91,0xD7,0xD8,0x92,0x93,0x94,0x95,0xDD,0xDE,0x96, // D-
			0x97,0x98,0x99,0xE3,0x9A,0xE5,0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,0x5E,0xA2,0xA3,0xA4, // E-
			0xF0,0xAE,0xA5,0x40,0xA7,0xF5,0xA8,0xF7,0xF8,0xA9,0xAA,0xAB,0xAC,0xFD,0xFE,0xFF, // F-
};


/**	\brief Conversion table for ISO-8859-1 -> GXT8
 */
const unsigned char ISO88591ToGXT8Table[] = {
		//  -0   -1   -2   -3   -4   -5   -6   -7   -8   -9   -A   -B   -C   -D   -E   -F
			0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F, // 0-
			0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F, // 1-
			0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F, // 2-
			0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F, // 3-
			0xF3,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F, // 4-
			0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x5B,0x5C,0x5D,0xEC,0x5F, // 5-
			0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F, // 6-
			0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x7B,0xBA,0x7D,0x7E,0xCC, // 7-
			0x7F,0xC1,0xC2,0xC3,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xC0,0xCD,0xCE,0xCF,0xD2,0xD3, // 8-
			0xD4,0xD6,0xD9,0xDA,0xDB,0xDC,0xDF,0xE0,0xE1,0xE2,0xE4,0xE6,0xE7,0xE8,0xE9,0xEA, // 9-
			0xEB,0x40,0xED,0xEE,0xEF,0xF2,0x5E,0xF4,0xF6,0xF9,0xFA,0xFB,0xFC,0xD1,0xF1,0xBF, // A-
			0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0x7C,0xBB,0xBC,0xBD,0xBE,0xAF, // B-
			0x80,0x81,0x82,0xC4,0x83,0xC5,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D, // C-
			0xD0,0xAD,0x8E,0x8F,0x90,0xD5,0x91,0xD7,0xD8,0x92,0x93,0x94,0x95,0xDD,0xDE,0x96, // D-
			0x97,0x98,0x99,0xE3,0x9A,0xE5,0x9B,0x9C,0x9D,0x9E,0x9F,0xA0,0xA1,0xA2,0xA3,0xA4, // E-
			0xF0,0xAE,0xA5,0xA6,0xA7,0xF5,0xA8,0xF7,0xF8,0xA9,0xAA,0xAB,0xAC,0xFD,0xFE,0xFF, // F-
};





// Unfortunately, first parameter needs to be non-const because iconv() takes the inbuf as non-const char**
int Transcode(char* src, int srcBytes, char* dest, int destBytes, Encoding srcEnc, Encoding destEnc)
{
	if (srcEnc == None) {
		return ERR_INVALID_PARAMETER;
	}
	if (srcEnc == destEnc  ||  destEnc == None) {
		// If we have the same encoding in src and dest, we will just copy.
		int len = srcBytes < destBytes ? srcBytes : destBytes;
		memcpy(dest, src, len);

		if (srcBytes > destBytes) {
			return ERR_INSUFFICIENT_BUFFER;
		}

		return len;
	}

	if (srcEnc == GXT8) {
		// GXT8 -> destEnc conversion is done by converting to ISO-8859-1 first and then to destEnc.
		unsigned char* tmp = new unsigned char[destBytes];
		int len = srcBytes < destBytes ? srcBytes : destBytes;

		for (int i = 0 ; i < len ; i++) {
			tmp[i] = GXT8ToISO88591Table[((unsigned char*) src)[i]];
		}

		int endLen = Transcode((char*) tmp, len, dest, destBytes, ISO8859_1, destEnc);

		if (srcBytes > destBytes) {
			return ERR_INSUFFICIENT_BUFFER;
		}

		return endLen;
	} else if (srcEnc == GXT16) {
		// TODO We assume that GXT16 is just GXT8 with an unused second byte. Don't know if that's true.
		int tmpSize = srcBytes%2 == 0 ? srcBytes/2 : (srcBytes+1)/2;
		unsigned char* tmp = new unsigned char[tmpSize];
		int len = srcBytes < destBytes ? srcBytes : destBytes;

		for (int i = 0 ; i < len/2 ; i++) {
			tmp[i] = GXT8ToISO88591Table[((unsigned char*) src)[i*2]];
		}

		int endLen = Transcode((char*) tmp, tmpSize, dest, destBytes, ISO8859_1, destEnc);
		return endLen;
	}

	if (destEnc == GXT8) {
		// srcEnc -> GXT8 conversion is done by converting to ISO-8859-1 first and then to GXT8.
		unsigned char* tmp = new unsigned char[destBytes];
		unsigned char* udest = (unsigned char*) dest;
		int len = Transcode(src, srcBytes, (char*) tmp, destBytes, srcEnc, ISO8859_1);
		bool insufficient = false;

		if (len == ERR_INSUFFICIENT_BUFFER) {
			// We will still fill the buffer
			len = destBytes;
			insufficient = true;
		}

		for (int i = 0 ; i < len ; i++) {
			udest[i] = ISO88591ToGXT8Table[tmp[i]];
		}

		if (insufficient) {
			return ERR_INSUFFICIENT_BUFFER;
		}

		return len;
	} else if (destEnc == GXT16) {
		// TODO We assume that GXT16 is just GXT8 with an unused second byte. Don't know if that's true.
		int tmpSize = destBytes%2 == 0 ? destBytes/2 : (destBytes+1)/2;
		unsigned char* tmp = new unsigned char[tmpSize];
		unsigned char* udest = (unsigned char*) dest;
		int len = Transcode(src, srcBytes, (char*) tmp, tmpSize, srcEnc, ISO8859_1);
		bool insufficient = false;

		if (len == ERR_INSUFFICIENT_BUFFER) {
			// We will still fill the buffer
			len = tmpSize;
			insufficient = true;
		}

		for (int i = 0 ; i < len ; i++) {
			udest[i*2] = ISO88591ToGXT8Table[tmp[i]];
			udest[i*2+1] = '\0';
		}

		if (insufficient) {
			return ERR_INSUFFICIENT_BUFFER;
		}

		return len;
	}

#ifdef _HAVE_ICONV
	// On Linux we use iconv.

	const char* iconvSrcEnc;
	const char* iconvDestEnc;
	size_t inLeft = srcBytes;
	size_t outLeft = destBytes;

	switch (srcEnc) {
	case ASCII:
		iconvSrcEnc = "ASCII";
		break;
	case UTF8:
		iconvSrcEnc = "UTF-8";
		break;
	case UTF16:
		iconvSrcEnc = "UTF-16LE"; // "UTF-16" only would prepend a byte order mark (BOM)
		break;
	case WINDOWS1252:
		iconvSrcEnc = "WINDOWS-1252";
		break;
	case ISO8859_1:
		iconvSrcEnc = "ISO8859-1";
		break;
	default:
		// This should never happen
		assert(false);
	}

	switch (destEnc) {
	case ASCII:
		iconvDestEnc = "ASCII";
		break;
	case UTF8:
		iconvDestEnc = "UTF-8";
		break;
	case UTF16:
		iconvDestEnc = "UTF-16LE"; // "UTF-16" only would prepend a byte order mark (BOM)
		break;
	case WINDOWS1252:
		iconvDestEnc = "WINDOWS-1252";
		break;
	case ISO8859_1:
		iconvDestEnc = "ISO-8859-1";
		break;
	default:
		// This should never happen
		assert(false);
	}

	iconv_t ic = iconv_open(iconvDestEnc, iconvSrcEnc);
	size_t len = iconv(ic, &src, &inLeft, &dest, &outLeft);
	iconv_close(ic);

	if (len == (size_t)-1) {
		iconv_close(ic);

		switch (errno) {
		case EILSEQ:
		case EINVAL:
			return ERR_INVALID_SEQUENCE;
		case E2BIG:
			return ERR_INSUFFICIENT_BUFFER;
		}
	}

	// iconv() does NOT return the number of actual bytes written, but decrements outLeft for each byte.
	return destBytes-outLeft;
#elif defined(_WIN32)
	// On Windows we use The WinAPI Unicode functions.

	if (srcEnc == UTF16) {
		UINT cp;

		switch (destEnc) {
		case ASCII:
			cp = 20127;
			break;
		case UTF8:
			cp = 65001;
			break;
		case WINDOWS1252:
			cp = 1252;
			break;
		case ISO8859_1:
			cp = 28591;
			break;
		default:
			assert(false);
			break;
		}

		int len = WideCharToMultiByte(cp, 0, (LPWSTR) src, srcBytes/2, dest, destBytes, NULL, NULL);

		if (len == 0) {
			switch (GetLastError()) {
			case ERROR_INSUFFICIENT_BUFFER:
				return ERR_INSUFFICIENT_BUFFER;
			case ERROR_INVALID_FLAGS:
			case ERROR_INVALID_PARAMETER:
				return ERR_INVALID_PARAMETER;
			case ERROR_NO_UNICODE_TRANSLATION:
				return ERR_INVALID_SEQUENCE;
			}
		}

		return len;
	} else {
		if (destEnc == UTF16) {
			UINT cp;

			switch (srcEnc) {
			case ASCII:
				cp = 20127;
				break;
			case UTF8:
				cp = 65001;
				break;
			case WINDOWS1252:
				cp = 1252;
				break;
			case ISO8859_1:
				cp = 28591;
				break;
			default:
				assert(false);
				break;
			}

			int len = MultiByteToWideChar(cp, MB_PRECOMPOSED, src, srcBytes, (LPWSTR) dest, destBytes/2);

			if (len == 0) {
				switch (GetLastError()) {
				case ERROR_INSUFFICIENT_BUFFER:
					return ERR_INSUFFICIENT_BUFFER;
				case ERROR_INVALID_FLAGS:
				case ERROR_INVALID_PARAMETER:
					return ERR_INVALID_PARAMETER;
				case ERROR_NO_UNICODE_TRANSLATION:
					return ERR_INVALID_SEQUENCE;
				}
			}

			return len*2; // MultiByteToWideChar returns the number of CHARACTERS written to dest.
		} else {
			// Windows does not seem to have a function to directly convert one single-byte-encoding to
			// another, so we convert the string to UTF16 and then to destEnc.
			char* tmpBuf = new char[srcBytes*2];
			int len = Transcode(src, srcBytes, tmpBuf, srcBytes*2, srcEnc, UTF16);

			if (len == ERR_INSUFFICIENT_BUFFER) {
				// Actually, this should never happen because we choose tmpBuf as twice the size of srcBytes,
				// which should always fit perfect when converting a single-byte-encoding to UTF-16.
				len = srcBytes*2;
			} else if (len < 0) {
				return len;
			}

			len = Transcode(tmpBuf, srcBytes*2, dest, destBytes, UTF16, destEnc);
			delete[] tmpBuf;

			return len;
		}
	}
#endif
}


int GetSufficientTranscodeBufferSize(int length, Encoding srcEnc, Encoding destEnc)
{
	if (srcEnc == UTF16  ||  srcEnc == GXT16) {
		length = length%2 == 0 ? length : length+1;
		if (destEnc == UTF16  ||  destEnc == GXT16) {
			return length;
		} else if (destEnc == UTF8) {
			return length/2*3; // Yes, for some characters it needs 3 bytes
		} else {
			return length/2;
		}
	} else {
		if (destEnc == UTF16  ||  destEnc == GXT16) {
			return length*2;
		} else if (destEnc == UTF8) {
			return length*2;
		} else {
			return length;
		}
	}
}


size_t strlenUTF16(const char* str)
{
	const int16_t* utf16 = (const int16_t*) str;
	size_t len = 0;
	while (*utf16++ != 0) len++;
	return len;
}
