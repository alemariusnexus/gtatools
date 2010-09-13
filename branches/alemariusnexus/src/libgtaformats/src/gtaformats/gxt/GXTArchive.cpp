/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#include "GXTArchive.h"
#include <fstream>
#include <map>
#include <cstdlib>
#include <utility>
#include <cstring>
#include <cstdio>
#include <algorithm>
//#include <iconv.h>
#include <errno.h>

using std::ifstream;
using std::map;
using std::pair;
using std::sort;


struct KeyEntry {
	int32_t offset;
	int32_t hash;
};

struct Ver1KeyEntry {
	int32_t offset;
	char key[8];
};


/*int _KeySortComparator(const void* k1, const void* k2) {
	printf("1: %p\n", k1);
	printf("2: %p\n", ((const KeyEntry**) k1));
	printf("3: %p\n", *((const KeyEntry**) k1));
	printf("4: %d\n", (*(const_cast<const KeyEntry**>(k1)))->offset);
	return (*((const KeyEntry**) k1))->offset - (*((const KeyEntry**) k2))->offset;
}*/

bool _KeySortComparator(KeyEntry* k1, KeyEntry* k2)
{
	return k1->offset < k2->offset;
}

bool _Ver1KeySortComparator(Ver1KeyEntry* k1, Ver1KeyEntry* k2)
{
	return k1->offset < k2->offset;
}



GXTArchive::GXTVersion GXTArchive::guessVersion(istream* stream, bool returnToStart)
{
	int32_t version;
	stream->read((char*) &version, 4);

	if (returnToStart) {
		stream->seekg(-4, istream::cur);
	}

	if (version == 0x080004) {
		return VER2;
	} else {
		return VER1;
	}
}


GXTArchive::GXTVersion GXTArchive::guessVersion(const char* filename)
{
	ifstream stream(filename, ifstream::in | ifstream::binary);
	return guessVersion(&stream, false);
}


GXTArchive::GXTArchive(istream* stream, bool deleteStream)
		: stream(stream), version(guessVersion(stream, false)), deleteStream(deleteStream)
{
	init();
}


GXTArchive::GXTArchive(const char* filename)
		: stream(new ifstream(filename, ifstream::in | ifstream::binary)),
		  version(guessVersion(stream, false)), deleteStream(true)
{
	init();
}


GXTArchive::~GXTArchive()
{
	for (int32_t i = 0 ; i < numTables ; i++) {
		delete headers[i];
	}

	delete[] headers;

	if (deleteStream) {
		delete stream;
	}
}


wchar_t* GXTArchive::decode8(const uint8_t* src, int32_t len)
{
	wchar_t* dest = new wchar_t[len];

	for (int32_t i = 0 ; i < len ; i++) {
		printf("dest[%d] = %04x [%d]   // %c\n", i, encodingMap[src[i]], src[i], src[i]);
		dest[i] = encodingMap[src[i]];

		printf("  => wchar_t is %d\n", dest[i]);
	}

	return dest;

	/*char* dest = new char[len*2];

	for (int32_t i = 0, j = 0 ; i < len ; i++, j++) {
		if (src[i] == '\0') {
			dest[j] = '\0';
			break;
		}

		const char* utf8 = encodingMap[src[i]];
		memcpy(dest+j, utf8, strlen(utf8));

		j += strlen(utf8)-1;
	}

	printf("String: %s\n", dest);

	exit(0);

	return dest;*/
}


void GXTArchive::init()
{
	encodingMap.insert(pair<uint8_t, wchar_t>(0, 0));
	encodingMap.insert(pair<uint8_t, wchar_t>(1, 1));
	encodingMap.insert(pair<uint8_t, wchar_t>(2, 2));
	encodingMap.insert(pair<uint8_t, wchar_t>(3, 3));
	encodingMap.insert(pair<uint8_t, wchar_t>(4, 4));
	encodingMap.insert(pair<uint8_t, wchar_t>(5, 5));
	encodingMap.insert(pair<uint8_t, wchar_t>(6, 6));
	encodingMap.insert(pair<uint8_t, wchar_t>(7, 7));
	encodingMap.insert(pair<uint8_t, wchar_t>(8, 8));
	encodingMap.insert(pair<uint8_t, wchar_t>(9, 9));
	encodingMap.insert(pair<uint8_t, wchar_t>(10, 10));
	encodingMap.insert(pair<uint8_t, wchar_t>(11, 11));
	encodingMap.insert(pair<uint8_t, wchar_t>(12, 12));
	encodingMap.insert(pair<uint8_t, wchar_t>(13, 13));
	encodingMap.insert(pair<uint8_t, wchar_t>(14, 14));
	encodingMap.insert(pair<uint8_t, wchar_t>(15, 15));
	encodingMap.insert(pair<uint8_t, wchar_t>(16, 16));
	encodingMap.insert(pair<uint8_t, wchar_t>(17, 17));
	encodingMap.insert(pair<uint8_t, wchar_t>(18, 18));
	encodingMap.insert(pair<uint8_t, wchar_t>(19, 19));
	encodingMap.insert(pair<uint8_t, wchar_t>(20, 20));
	encodingMap.insert(pair<uint8_t, wchar_t>(21, 21));
	encodingMap.insert(pair<uint8_t, wchar_t>(22, 22));
	encodingMap.insert(pair<uint8_t, wchar_t>(23, 23));
	encodingMap.insert(pair<uint8_t, wchar_t>(24, 24));
	encodingMap.insert(pair<uint8_t, wchar_t>(25, 25));
	encodingMap.insert(pair<uint8_t, wchar_t>(26, 26));
	encodingMap.insert(pair<uint8_t, wchar_t>(27, 27));
	encodingMap.insert(pair<uint8_t, wchar_t>(28, 28));
	encodingMap.insert(pair<uint8_t, wchar_t>(29, 29));
	encodingMap.insert(pair<uint8_t, wchar_t>(30, 30));
	encodingMap.insert(pair<uint8_t, wchar_t>(31, 31));
	encodingMap.insert(pair<uint8_t, wchar_t>(32, 32));
	encodingMap.insert(pair<uint8_t, wchar_t>(33, 33));
	encodingMap.insert(pair<uint8_t, wchar_t>(34, 34));
	encodingMap.insert(pair<uint8_t, wchar_t>(35, 35));
	encodingMap.insert(pair<uint8_t, wchar_t>(36, 36));
	encodingMap.insert(pair<uint8_t, wchar_t>(37, 37));
	encodingMap.insert(pair<uint8_t, wchar_t>(38, 38));
	encodingMap.insert(pair<uint8_t, wchar_t>(39, 39));
	encodingMap.insert(pair<uint8_t, wchar_t>(40, 40));
	encodingMap.insert(pair<uint8_t, wchar_t>(41, 41));
	encodingMap.insert(pair<uint8_t, wchar_t>(42, 42));
	encodingMap.insert(pair<uint8_t, wchar_t>(43, 43));
	encodingMap.insert(pair<uint8_t, wchar_t>(44, 44));
	encodingMap.insert(pair<uint8_t, wchar_t>(45, 45));
	encodingMap.insert(pair<uint8_t, wchar_t>(46, 46));
	encodingMap.insert(pair<uint8_t, wchar_t>(47, 47));
	encodingMap.insert(pair<uint8_t, wchar_t>(48, 48));
	encodingMap.insert(pair<uint8_t, wchar_t>(49, 49));
	encodingMap.insert(pair<uint8_t, wchar_t>(50, 50));
	encodingMap.insert(pair<uint8_t, wchar_t>(51, 51));
	encodingMap.insert(pair<uint8_t, wchar_t>(52, 52));
	encodingMap.insert(pair<uint8_t, wchar_t>(53, 53));
	encodingMap.insert(pair<uint8_t, wchar_t>(54, 54));
	encodingMap.insert(pair<uint8_t, wchar_t>(55, 55));
	encodingMap.insert(pair<uint8_t, wchar_t>(56, 56));
	encodingMap.insert(pair<uint8_t, wchar_t>(57, 57));
	encodingMap.insert(pair<uint8_t, wchar_t>(58, 58));
	encodingMap.insert(pair<uint8_t, wchar_t>(59, 59));
	encodingMap.insert(pair<uint8_t, wchar_t>(60, 60));
	encodingMap.insert(pair<uint8_t, wchar_t>(61, 61));
	encodingMap.insert(pair<uint8_t, wchar_t>(62, 62));
	encodingMap.insert(pair<uint8_t, wchar_t>(63, 63));
	encodingMap.insert(pair<uint8_t, wchar_t>(161, 64));
	encodingMap.insert(pair<uint8_t, wchar_t>(65, 65));
	encodingMap.insert(pair<uint8_t, wchar_t>(66, 66));
	encodingMap.insert(pair<uint8_t, wchar_t>(67, 67));
	encodingMap.insert(pair<uint8_t, wchar_t>(68, 68));
	encodingMap.insert(pair<uint8_t, wchar_t>(69, 69));
	encodingMap.insert(pair<uint8_t, wchar_t>(70, 70));
	encodingMap.insert(pair<uint8_t, wchar_t>(71, 71));
	encodingMap.insert(pair<uint8_t, wchar_t>(72, 72));
	encodingMap.insert(pair<uint8_t, wchar_t>(73, 73));
	encodingMap.insert(pair<uint8_t, wchar_t>(74, 74));
	encodingMap.insert(pair<uint8_t, wchar_t>(75, 75));
	encodingMap.insert(pair<uint8_t, wchar_t>(76, 76));
	encodingMap.insert(pair<uint8_t, wchar_t>(77, 77));
	encodingMap.insert(pair<uint8_t, wchar_t>(78, 78));
	encodingMap.insert(pair<uint8_t, wchar_t>(79, 79));
	encodingMap.insert(pair<uint8_t, wchar_t>(80, 80));
	encodingMap.insert(pair<uint8_t, wchar_t>(81, 81));
	encodingMap.insert(pair<uint8_t, wchar_t>(82, 82));
	encodingMap.insert(pair<uint8_t, wchar_t>(83, 83));
	encodingMap.insert(pair<uint8_t, wchar_t>(84, 84));
	encodingMap.insert(pair<uint8_t, wchar_t>(85, 85));
	encodingMap.insert(pair<uint8_t, wchar_t>(86, 86));
	encodingMap.insert(pair<uint8_t, wchar_t>(87, 87));
	encodingMap.insert(pair<uint8_t, wchar_t>(88, 88));
	encodingMap.insert(pair<uint8_t, wchar_t>(89, 89));
	encodingMap.insert(pair<uint8_t, wchar_t>(90, 90));
	encodingMap.insert(pair<uint8_t, wchar_t>(91, 91));
	encodingMap.insert(pair<uint8_t, wchar_t>(92, 92));
	encodingMap.insert(pair<uint8_t, wchar_t>(93, 93));
	encodingMap.insert(pair<uint8_t, wchar_t>(166, 94));
	encodingMap.insert(pair<uint8_t, wchar_t>(95, 95));
	encodingMap.insert(pair<uint8_t, wchar_t>(96, 96));
	encodingMap.insert(pair<uint8_t, wchar_t>(97, 97));
	encodingMap.insert(pair<uint8_t, wchar_t>(98, 98));
	encodingMap.insert(pair<uint8_t, wchar_t>(99, 99));
	encodingMap.insert(pair<uint8_t, wchar_t>(100, 100));
	encodingMap.insert(pair<uint8_t, wchar_t>(101, 101));
	encodingMap.insert(pair<uint8_t, wchar_t>(102, 102));
	encodingMap.insert(pair<uint8_t, wchar_t>(103, 103));
	encodingMap.insert(pair<uint8_t, wchar_t>(104, 104));
	encodingMap.insert(pair<uint8_t, wchar_t>(105, 105));
	encodingMap.insert(pair<uint8_t, wchar_t>(106, 106));
	encodingMap.insert(pair<uint8_t, wchar_t>(107, 107));
	encodingMap.insert(pair<uint8_t, wchar_t>(108, 108));
	encodingMap.insert(pair<uint8_t, wchar_t>(109, 109));
	encodingMap.insert(pair<uint8_t, wchar_t>(110, 110));
	encodingMap.insert(pair<uint8_t, wchar_t>(111, 111));
	encodingMap.insert(pair<uint8_t, wchar_t>(112, 112));
	encodingMap.insert(pair<uint8_t, wchar_t>(113, 113));
	encodingMap.insert(pair<uint8_t, wchar_t>(114, 114));
	encodingMap.insert(pair<uint8_t, wchar_t>(115, 115));
	encodingMap.insert(pair<uint8_t, wchar_t>(116, 116));
	encodingMap.insert(pair<uint8_t, wchar_t>(117, 117));
	encodingMap.insert(pair<uint8_t, wchar_t>(118, 118));
	encodingMap.insert(pair<uint8_t, wchar_t>(119, 119));
	encodingMap.insert(pair<uint8_t, wchar_t>(120, 120));
	encodingMap.insert(pair<uint8_t, wchar_t>(121, 121));
	encodingMap.insert(pair<uint8_t, wchar_t>(122, 122));
	encodingMap.insert(pair<uint8_t, wchar_t>(123, 123));
	encodingMap.insert(pair<uint8_t, wchar_t>(186, 124));
	encodingMap.insert(pair<uint8_t, wchar_t>(125, 125));
	encodingMap.insert(pair<uint8_t, wchar_t>(126, 126));
	encodingMap.insert(pair<uint8_t, wchar_t>(128, 127));
	encodingMap.insert(pair<uint8_t, wchar_t>(192, 128));
	encodingMap.insert(pair<uint8_t, wchar_t>(193, 129));
	encodingMap.insert(pair<uint8_t, wchar_t>(194, 130));
	encodingMap.insert(pair<uint8_t, wchar_t>(196, 131));
	encodingMap.insert(pair<uint8_t, wchar_t>(198, 132));
	encodingMap.insert(pair<uint8_t, wchar_t>(199, 133));
	encodingMap.insert(pair<uint8_t, wchar_t>(200, 134));
	encodingMap.insert(pair<uint8_t, wchar_t>(201, 135));
	encodingMap.insert(pair<uint8_t, wchar_t>(202, 136));
	encodingMap.insert(pair<uint8_t, wchar_t>(203, 137));
	encodingMap.insert(pair<uint8_t, wchar_t>(204, 138));
	encodingMap.insert(pair<uint8_t, wchar_t>(205, 139));
	encodingMap.insert(pair<uint8_t, wchar_t>(206, 140));
	encodingMap.insert(pair<uint8_t, wchar_t>(207, 141));
	encodingMap.insert(pair<uint8_t, wchar_t>(210, 142));
	encodingMap.insert(pair<uint8_t, wchar_t>(211, 143));
	encodingMap.insert(pair<uint8_t, wchar_t>(212, 144));
	encodingMap.insert(pair<uint8_t, wchar_t>(214, 145));
	encodingMap.insert(pair<uint8_t, wchar_t>(217, 146));
	encodingMap.insert(pair<uint8_t, wchar_t>(218, 147));
	encodingMap.insert(pair<uint8_t, wchar_t>(219, 148));
	encodingMap.insert(pair<uint8_t, wchar_t>(220, 149));
	encodingMap.insert(pair<uint8_t, wchar_t>(223, 150));
	encodingMap.insert(pair<uint8_t, wchar_t>(224, 151));
	encodingMap.insert(pair<uint8_t, wchar_t>(225, 152));
	encodingMap.insert(pair<uint8_t, wchar_t>(226, 153));
	encodingMap.insert(pair<uint8_t, wchar_t>(228, 154));
	encodingMap.insert(pair<uint8_t, wchar_t>(230, 155));
	encodingMap.insert(pair<uint8_t, wchar_t>(231, 156));
	encodingMap.insert(pair<uint8_t, wchar_t>(232, 157));
	encodingMap.insert(pair<uint8_t, wchar_t>(233, 158));
	encodingMap.insert(pair<uint8_t, wchar_t>(234, 159));
	encodingMap.insert(pair<uint8_t, wchar_t>(235, 160));
	encodingMap.insert(pair<uint8_t, wchar_t>(236, 161));
	encodingMap.insert(pair<uint8_t, wchar_t>(237, 162));
	encodingMap.insert(pair<uint8_t, wchar_t>(238, 163));
	encodingMap.insert(pair<uint8_t, wchar_t>(239, 164));
	encodingMap.insert(pair<uint8_t, wchar_t>(242, 165));
	encodingMap.insert(pair<uint8_t, wchar_t>(243, 166));
	encodingMap.insert(pair<uint8_t, wchar_t>(244, 167));
	encodingMap.insert(pair<uint8_t, wchar_t>(246, 168));
	encodingMap.insert(pair<uint8_t, wchar_t>(249, 169));
	encodingMap.insert(pair<uint8_t, wchar_t>(250, 170));
	encodingMap.insert(pair<uint8_t, wchar_t>(251, 171));
	encodingMap.insert(pair<uint8_t, wchar_t>(252, 172));
	encodingMap.insert(pair<uint8_t, wchar_t>(209, 173));
	encodingMap.insert(pair<uint8_t, wchar_t>(241, 174));
	encodingMap.insert(pair<uint8_t, wchar_t>(191, 175));
	encodingMap.insert(pair<uint8_t, wchar_t>(176, 176));
	encodingMap.insert(pair<uint8_t, wchar_t>(177, 177));
	encodingMap.insert(pair<uint8_t, wchar_t>(178, 178));
	encodingMap.insert(pair<uint8_t, wchar_t>(179, 179));
	encodingMap.insert(pair<uint8_t, wchar_t>(180, 180));
	encodingMap.insert(pair<uint8_t, wchar_t>(181, 181));
	encodingMap.insert(pair<uint8_t, wchar_t>(182, 182));
	encodingMap.insert(pair<uint8_t, wchar_t>(183, 183));
	encodingMap.insert(pair<uint8_t, wchar_t>(184, 184));
	encodingMap.insert(pair<uint8_t, wchar_t>(185, 185));
	encodingMap.insert(pair<uint8_t, wchar_t>(124, 186));
	encodingMap.insert(pair<uint8_t, wchar_t>(187, 187));
	encodingMap.insert(pair<uint8_t, wchar_t>(188, 188));
	encodingMap.insert(pair<uint8_t, wchar_t>(189, 189));
	encodingMap.insert(pair<uint8_t, wchar_t>(190, 190));
	encodingMap.insert(pair<uint8_t, wchar_t>(175, 191));
	encodingMap.insert(pair<uint8_t, wchar_t>(138, 192));
	encodingMap.insert(pair<uint8_t, wchar_t>(129, 193));
	encodingMap.insert(pair<uint8_t, wchar_t>(130, 194));
	encodingMap.insert(pair<uint8_t, wchar_t>(131, 195));
	encodingMap.insert(pair<uint8_t, wchar_t>(195, 196));
	encodingMap.insert(pair<uint8_t, wchar_t>(197, 197));
	encodingMap.insert(pair<uint8_t, wchar_t>(132, 198));
	encodingMap.insert(pair<uint8_t, wchar_t>(133, 199));
	encodingMap.insert(pair<uint8_t, wchar_t>(134, 200));
	encodingMap.insert(pair<uint8_t, wchar_t>(135, 201));
	encodingMap.insert(pair<uint8_t, wchar_t>(136, 202));
	encodingMap.insert(pair<uint8_t, wchar_t>(137, 203));
	encodingMap.insert(pair<uint8_t, wchar_t>(127, 204));
	encodingMap.insert(pair<uint8_t, wchar_t>(139, 205));
	encodingMap.insert(pair<uint8_t, wchar_t>(140, 206));
	encodingMap.insert(pair<uint8_t, wchar_t>(141, 207));
	encodingMap.insert(pair<uint8_t, wchar_t>(208, 208));
	encodingMap.insert(pair<uint8_t, wchar_t>(173, 209));
	encodingMap.insert(pair<uint8_t, wchar_t>(142, 210));
	encodingMap.insert(pair<uint8_t, wchar_t>(143, 211));
	encodingMap.insert(pair<uint8_t, wchar_t>(144, 212));
	encodingMap.insert(pair<uint8_t, wchar_t>(213, 213));
	encodingMap.insert(pair<uint8_t, wchar_t>(145, 214));
	encodingMap.insert(pair<uint8_t, wchar_t>(215, 215));
	encodingMap.insert(pair<uint8_t, wchar_t>(216, 216));
	encodingMap.insert(pair<uint8_t, wchar_t>(146, 217));
	encodingMap.insert(pair<uint8_t, wchar_t>(147, 218));
	encodingMap.insert(pair<uint8_t, wchar_t>(148, 219));
	encodingMap.insert(pair<uint8_t, wchar_t>(149, 220));
	encodingMap.insert(pair<uint8_t, wchar_t>(221, 221));
	encodingMap.insert(pair<uint8_t, wchar_t>(222, 222));
	encodingMap.insert(pair<uint8_t, wchar_t>(150, 223));
	encodingMap.insert(pair<uint8_t, wchar_t>(151, 224));
	encodingMap.insert(pair<uint8_t, wchar_t>(152, 225));
	encodingMap.insert(pair<uint8_t, wchar_t>(153, 226));
	encodingMap.insert(pair<uint8_t, wchar_t>(227, 227));
	encodingMap.insert(pair<uint8_t, wchar_t>(154, 228));
	encodingMap.insert(pair<uint8_t, wchar_t>(229, 229));
	encodingMap.insert(pair<uint8_t, wchar_t>(155, 230));
	encodingMap.insert(pair<uint8_t, wchar_t>(156, 231));
	encodingMap.insert(pair<uint8_t, wchar_t>(157, 232));
	encodingMap.insert(pair<uint8_t, wchar_t>(158, 233));
	encodingMap.insert(pair<uint8_t, wchar_t>(159, 234));
	encodingMap.insert(pair<uint8_t, wchar_t>(160, 235));
	encodingMap.insert(pair<uint8_t, wchar_t>(94, 236));
	encodingMap.insert(pair<uint8_t, wchar_t>(162, 237));
	encodingMap.insert(pair<uint8_t, wchar_t>(163, 238));
	encodingMap.insert(pair<uint8_t, wchar_t>(164, 239));
	encodingMap.insert(pair<uint8_t, wchar_t>(240, 240));
	encodingMap.insert(pair<uint8_t, wchar_t>(174, 241));
	encodingMap.insert(pair<uint8_t, wchar_t>(165, 242));
	encodingMap.insert(pair<uint8_t, wchar_t>(64, 243));
	encodingMap.insert(pair<uint8_t, wchar_t>(167, 244));
	encodingMap.insert(pair<uint8_t, wchar_t>(245, 245));
	encodingMap.insert(pair<uint8_t, wchar_t>(168, 246));
	encodingMap.insert(pair<uint8_t, wchar_t>(247, 247));
	encodingMap.insert(pair<uint8_t, wchar_t>(248, 248));
	encodingMap.insert(pair<uint8_t, wchar_t>(169, 249));
	encodingMap.insert(pair<uint8_t, wchar_t>(170, 250));
	encodingMap.insert(pair<uint8_t, wchar_t>(171, 251));
	encodingMap.insert(pair<uint8_t, wchar_t>(172, 252));
	encodingMap.insert(pair<uint8_t, wchar_t>(253, 253));
	encodingMap.insert(pair<uint8_t, wchar_t>(254, 254));
	encodingMap.insert(pair<uint8_t, wchar_t>(255, 255));
	encodingMap.insert(pair<uint8_t, wchar_t>(0, 0));
	encodingMap.insert(pair<uint8_t, wchar_t>(1, 1));
	encodingMap.insert(pair<uint8_t, wchar_t>(2, 2));
	encodingMap.insert(pair<uint8_t, wchar_t>(3, 3));
	encodingMap.insert(pair<uint8_t, wchar_t>(4, 4));
	encodingMap.insert(pair<uint8_t, wchar_t>(5, 5));
	encodingMap.insert(pair<uint8_t, wchar_t>(6, 6));
	encodingMap.insert(pair<uint8_t, wchar_t>(7, 7));
	encodingMap.insert(pair<uint8_t, wchar_t>(8, 8));
	encodingMap.insert(pair<uint8_t, wchar_t>(9, 9));
	encodingMap.insert(pair<uint8_t, wchar_t>(10, 10));
	encodingMap.insert(pair<uint8_t, wchar_t>(11, 11));
	encodingMap.insert(pair<uint8_t, wchar_t>(12, 12));
	encodingMap.insert(pair<uint8_t, wchar_t>(13, 13));
	encodingMap.insert(pair<uint8_t, wchar_t>(14, 14));
	encodingMap.insert(pair<uint8_t, wchar_t>(15, 15));
	encodingMap.insert(pair<uint8_t, wchar_t>(16, 16));
	encodingMap.insert(pair<uint8_t, wchar_t>(17, 17));
	encodingMap.insert(pair<uint8_t, wchar_t>(18, 18));
	encodingMap.insert(pair<uint8_t, wchar_t>(19, 19));
	encodingMap.insert(pair<uint8_t, wchar_t>(20, 20));
	encodingMap.insert(pair<uint8_t, wchar_t>(21, 21));
	encodingMap.insert(pair<uint8_t, wchar_t>(22, 22));
	encodingMap.insert(pair<uint8_t, wchar_t>(23, 23));
	encodingMap.insert(pair<uint8_t, wchar_t>(24, 24));
	encodingMap.insert(pair<uint8_t, wchar_t>(25, 25));
	encodingMap.insert(pair<uint8_t, wchar_t>(26, 26));
	encodingMap.insert(pair<uint8_t, wchar_t>(27, 27));
	encodingMap.insert(pair<uint8_t, wchar_t>(28, 28));
	encodingMap.insert(pair<uint8_t, wchar_t>(29, 29));
	encodingMap.insert(pair<uint8_t, wchar_t>(30, 30));
	encodingMap.insert(pair<uint8_t, wchar_t>(31, 31));
	encodingMap.insert(pair<uint8_t, wchar_t>(32, 32));
	encodingMap.insert(pair<uint8_t, wchar_t>(33, 33));
	encodingMap.insert(pair<uint8_t, wchar_t>(34, 34));
	encodingMap.insert(pair<uint8_t, wchar_t>(35, 35));
	encodingMap.insert(pair<uint8_t, wchar_t>(36, 36));
	encodingMap.insert(pair<uint8_t, wchar_t>(37, 37));
	encodingMap.insert(pair<uint8_t, wchar_t>(38, 38));
	encodingMap.insert(pair<uint8_t, wchar_t>(39, 39));
	encodingMap.insert(pair<uint8_t, wchar_t>(40, 40));
	encodingMap.insert(pair<uint8_t, wchar_t>(41, 41));
	encodingMap.insert(pair<uint8_t, wchar_t>(42, 42));
	encodingMap.insert(pair<uint8_t, wchar_t>(43, 43));
	encodingMap.insert(pair<uint8_t, wchar_t>(44, 44));
	encodingMap.insert(pair<uint8_t, wchar_t>(45, 45));
	encodingMap.insert(pair<uint8_t, wchar_t>(46, 46));
	encodingMap.insert(pair<uint8_t, wchar_t>(47, 47));
	encodingMap.insert(pair<uint8_t, wchar_t>(48, 48));
	encodingMap.insert(pair<uint8_t, wchar_t>(49, 49));
	encodingMap.insert(pair<uint8_t, wchar_t>(50, 50));
	encodingMap.insert(pair<uint8_t, wchar_t>(51, 51));
	encodingMap.insert(pair<uint8_t, wchar_t>(52, 52));
	encodingMap.insert(pair<uint8_t, wchar_t>(53, 53));
	encodingMap.insert(pair<uint8_t, wchar_t>(54, 54));
	encodingMap.insert(pair<uint8_t, wchar_t>(55, 55));
	encodingMap.insert(pair<uint8_t, wchar_t>(56, 56));
	encodingMap.insert(pair<uint8_t, wchar_t>(57, 57));
	encodingMap.insert(pair<uint8_t, wchar_t>(58, 58));
	encodingMap.insert(pair<uint8_t, wchar_t>(59, 59));
	encodingMap.insert(pair<uint8_t, wchar_t>(60, 60));
	encodingMap.insert(pair<uint8_t, wchar_t>(61, 61));
	encodingMap.insert(pair<uint8_t, wchar_t>(62, 62));
	encodingMap.insert(pair<uint8_t, wchar_t>(63, 63));
	encodingMap.insert(pair<uint8_t, wchar_t>(243, 64));
	encodingMap.insert(pair<uint8_t, wchar_t>(65, 65));
	encodingMap.insert(pair<uint8_t, wchar_t>(66, 66));
	encodingMap.insert(pair<uint8_t, wchar_t>(67, 67));
	encodingMap.insert(pair<uint8_t, wchar_t>(68, 68));
	encodingMap.insert(pair<uint8_t, wchar_t>(69, 69));
	encodingMap.insert(pair<uint8_t, wchar_t>(70, 70));
	encodingMap.insert(pair<uint8_t, wchar_t>(71, 71));
	encodingMap.insert(pair<uint8_t, wchar_t>(72, 72));
	encodingMap.insert(pair<uint8_t, wchar_t>(73, 73));
	encodingMap.insert(pair<uint8_t, wchar_t>(74, 74));
	encodingMap.insert(pair<uint8_t, wchar_t>(75, 75));
	encodingMap.insert(pair<uint8_t, wchar_t>(76, 76));
	encodingMap.insert(pair<uint8_t, wchar_t>(77, 77));
	encodingMap.insert(pair<uint8_t, wchar_t>(78, 78));
	encodingMap.insert(pair<uint8_t, wchar_t>(79, 79));
	encodingMap.insert(pair<uint8_t, wchar_t>(80, 80));
	encodingMap.insert(pair<uint8_t, wchar_t>(81, 81));
	encodingMap.insert(pair<uint8_t, wchar_t>(82, 82));
	encodingMap.insert(pair<uint8_t, wchar_t>(83, 83));
	encodingMap.insert(pair<uint8_t, wchar_t>(84, 84));
	encodingMap.insert(pair<uint8_t, wchar_t>(85, 85));
	encodingMap.insert(pair<uint8_t, wchar_t>(86, 86));
	encodingMap.insert(pair<uint8_t, wchar_t>(87, 87));
	encodingMap.insert(pair<uint8_t, wchar_t>(88, 88));
	encodingMap.insert(pair<uint8_t, wchar_t>(89, 89));
	encodingMap.insert(pair<uint8_t, wchar_t>(90, 90));
	encodingMap.insert(pair<uint8_t, wchar_t>(91, 91));
	encodingMap.insert(pair<uint8_t, wchar_t>(92, 92));
	encodingMap.insert(pair<uint8_t, wchar_t>(93, 93));
	encodingMap.insert(pair<uint8_t, wchar_t>(236, 94));
	encodingMap.insert(pair<uint8_t, wchar_t>(95, 95));
	encodingMap.insert(pair<uint8_t, wchar_t>(96, 96));
	encodingMap.insert(pair<uint8_t, wchar_t>(97, 97));
	encodingMap.insert(pair<uint8_t, wchar_t>(98, 98));
	encodingMap.insert(pair<uint8_t, wchar_t>(99, 99));
	encodingMap.insert(pair<uint8_t, wchar_t>(100, 100));
	encodingMap.insert(pair<uint8_t, wchar_t>(101, 101));
	encodingMap.insert(pair<uint8_t, wchar_t>(102, 102));
	encodingMap.insert(pair<uint8_t, wchar_t>(103, 103));
	encodingMap.insert(pair<uint8_t, wchar_t>(104, 104));
	encodingMap.insert(pair<uint8_t, wchar_t>(105, 105));
	encodingMap.insert(pair<uint8_t, wchar_t>(106, 106));
	encodingMap.insert(pair<uint8_t, wchar_t>(107, 107));
	encodingMap.insert(pair<uint8_t, wchar_t>(108, 108));
	encodingMap.insert(pair<uint8_t, wchar_t>(109, 109));
	encodingMap.insert(pair<uint8_t, wchar_t>(110, 110));
	encodingMap.insert(pair<uint8_t, wchar_t>(111, 111));
	encodingMap.insert(pair<uint8_t, wchar_t>(112, 112));
	encodingMap.insert(pair<uint8_t, wchar_t>(113, 113));
	encodingMap.insert(pair<uint8_t, wchar_t>(114, 114));
	encodingMap.insert(pair<uint8_t, wchar_t>(115, 115));
	encodingMap.insert(pair<uint8_t, wchar_t>(116, 116));
	encodingMap.insert(pair<uint8_t, wchar_t>(117, 117));
	encodingMap.insert(pair<uint8_t, wchar_t>(118, 118));
	encodingMap.insert(pair<uint8_t, wchar_t>(119, 119));
	encodingMap.insert(pair<uint8_t, wchar_t>(120, 120));
	encodingMap.insert(pair<uint8_t, wchar_t>(121, 121));
	encodingMap.insert(pair<uint8_t, wchar_t>(122, 122));
	encodingMap.insert(pair<uint8_t, wchar_t>(123, 123));
	encodingMap.insert(pair<uint8_t, wchar_t>(186, 124));
	encodingMap.insert(pair<uint8_t, wchar_t>(125, 125));
	encodingMap.insert(pair<uint8_t, wchar_t>(126, 126));
	encodingMap.insert(pair<uint8_t, wchar_t>(204, 127));
	encodingMap.insert(pair<uint8_t, wchar_t>(127, 128));
	encodingMap.insert(pair<uint8_t, wchar_t>(193, 129));
	encodingMap.insert(pair<uint8_t, wchar_t>(194, 130));
	encodingMap.insert(pair<uint8_t, wchar_t>(195, 131));
	encodingMap.insert(pair<uint8_t, wchar_t>(198, 132));
	encodingMap.insert(pair<uint8_t, wchar_t>(199, 133));
	encodingMap.insert(pair<uint8_t, wchar_t>(200, 134));
	encodingMap.insert(pair<uint8_t, wchar_t>(201, 135));
	encodingMap.insert(pair<uint8_t, wchar_t>(202, 136));
	encodingMap.insert(pair<uint8_t, wchar_t>(203, 137));
	encodingMap.insert(pair<uint8_t, wchar_t>(192, 138));
	encodingMap.insert(pair<uint8_t, wchar_t>(205, 139));
	encodingMap.insert(pair<uint8_t, wchar_t>(206, 140));
	encodingMap.insert(pair<uint8_t, wchar_t>(207, 141));
	encodingMap.insert(pair<uint8_t, wchar_t>(210, 142));
	encodingMap.insert(pair<uint8_t, wchar_t>(211, 143));
	encodingMap.insert(pair<uint8_t, wchar_t>(212, 144));
	encodingMap.insert(pair<uint8_t, wchar_t>(214, 145));
	encodingMap.insert(pair<uint8_t, wchar_t>(217, 146));
	encodingMap.insert(pair<uint8_t, wchar_t>(218, 147));
	encodingMap.insert(pair<uint8_t, wchar_t>(219, 148));
	encodingMap.insert(pair<uint8_t, wchar_t>(220, 149));
	encodingMap.insert(pair<uint8_t, wchar_t>(223, 150));
	encodingMap.insert(pair<uint8_t, wchar_t>(224, 151));
	encodingMap.insert(pair<uint8_t, wchar_t>(225, 152));
	encodingMap.insert(pair<uint8_t, wchar_t>(226, 153));
	encodingMap.insert(pair<uint8_t, wchar_t>(228, 154));
	encodingMap.insert(pair<uint8_t, wchar_t>(230, 155));
	encodingMap.insert(pair<uint8_t, wchar_t>(231, 156));
	encodingMap.insert(pair<uint8_t, wchar_t>(232, 157));
	encodingMap.insert(pair<uint8_t, wchar_t>(233, 158));
	encodingMap.insert(pair<uint8_t, wchar_t>(234, 159));
	encodingMap.insert(pair<uint8_t, wchar_t>(235, 160));
	encodingMap.insert(pair<uint8_t, wchar_t>(64, 161));
	encodingMap.insert(pair<uint8_t, wchar_t>(237, 162));
	encodingMap.insert(pair<uint8_t, wchar_t>(238, 163));
	encodingMap.insert(pair<uint8_t, wchar_t>(239, 164));
	encodingMap.insert(pair<uint8_t, wchar_t>(242, 165));
	encodingMap.insert(pair<uint8_t, wchar_t>(94, 166));
	encodingMap.insert(pair<uint8_t, wchar_t>(244, 167));
	encodingMap.insert(pair<uint8_t, wchar_t>(246, 168));
	encodingMap.insert(pair<uint8_t, wchar_t>(249, 169));
	encodingMap.insert(pair<uint8_t, wchar_t>(250, 170));
	encodingMap.insert(pair<uint8_t, wchar_t>(251, 171));
	encodingMap.insert(pair<uint8_t, wchar_t>(252, 172));
	encodingMap.insert(pair<uint8_t, wchar_t>(209, 173));
	encodingMap.insert(pair<uint8_t, wchar_t>(241, 174));
	encodingMap.insert(pair<uint8_t, wchar_t>(191, 175));
	encodingMap.insert(pair<uint8_t, wchar_t>(176, 176));
	encodingMap.insert(pair<uint8_t, wchar_t>(177, 177));
	encodingMap.insert(pair<uint8_t, wchar_t>(178, 178));
	encodingMap.insert(pair<uint8_t, wchar_t>(179, 179));
	encodingMap.insert(pair<uint8_t, wchar_t>(180, 180));
	encodingMap.insert(pair<uint8_t, wchar_t>(181, 181));
	encodingMap.insert(pair<uint8_t, wchar_t>(182, 182));
	encodingMap.insert(pair<uint8_t, wchar_t>(183, 183));
	encodingMap.insert(pair<uint8_t, wchar_t>(184, 184));
	encodingMap.insert(pair<uint8_t, wchar_t>(185, 185));
	encodingMap.insert(pair<uint8_t, wchar_t>(124, 186));
	encodingMap.insert(pair<uint8_t, wchar_t>(187, 187));
	encodingMap.insert(pair<uint8_t, wchar_t>(188, 188));
	encodingMap.insert(pair<uint8_t, wchar_t>(189, 189));
	encodingMap.insert(pair<uint8_t, wchar_t>(190, 190));
	encodingMap.insert(pair<uint8_t, wchar_t>(175, 191));
	encodingMap.insert(pair<uint8_t, wchar_t>(128, 192));
	encodingMap.insert(pair<uint8_t, wchar_t>(129, 193));
	encodingMap.insert(pair<uint8_t, wchar_t>(130, 194));
	encodingMap.insert(pair<uint8_t, wchar_t>(196, 195));
	encodingMap.insert(pair<uint8_t, wchar_t>(131, 196));
	encodingMap.insert(pair<uint8_t, wchar_t>(197, 197));
	encodingMap.insert(pair<uint8_t, wchar_t>(132, 198));
	encodingMap.insert(pair<uint8_t, wchar_t>(133, 199));
	encodingMap.insert(pair<uint8_t, wchar_t>(134, 200));
	encodingMap.insert(pair<uint8_t, wchar_t>(135, 201));
	encodingMap.insert(pair<uint8_t, wchar_t>(136, 202));
	encodingMap.insert(pair<uint8_t, wchar_t>(137, 203));
	encodingMap.insert(pair<uint8_t, wchar_t>(138, 204));
	encodingMap.insert(pair<uint8_t, wchar_t>(139, 205));
	encodingMap.insert(pair<uint8_t, wchar_t>(140, 206));
	encodingMap.insert(pair<uint8_t, wchar_t>(141, 207));
	encodingMap.insert(pair<uint8_t, wchar_t>(208, 208));
	encodingMap.insert(pair<uint8_t, wchar_t>(173, 209));
	encodingMap.insert(pair<uint8_t, wchar_t>(142, 210));
	encodingMap.insert(pair<uint8_t, wchar_t>(143, 211));
	encodingMap.insert(pair<uint8_t, wchar_t>(144, 212));
	encodingMap.insert(pair<uint8_t, wchar_t>(213, 213));
	encodingMap.insert(pair<uint8_t, wchar_t>(145, 214));
	encodingMap.insert(pair<uint8_t, wchar_t>(215, 215));
	encodingMap.insert(pair<uint8_t, wchar_t>(216, 216));
	encodingMap.insert(pair<uint8_t, wchar_t>(146, 217));
	encodingMap.insert(pair<uint8_t, wchar_t>(147, 218));
	encodingMap.insert(pair<uint8_t, wchar_t>(148, 219));
	encodingMap.insert(pair<uint8_t, wchar_t>(149, 220));
	encodingMap.insert(pair<uint8_t, wchar_t>(221, 221));
	encodingMap.insert(pair<uint8_t, wchar_t>(222, 222));
	encodingMap.insert(pair<uint8_t, wchar_t>(150, 223));
	encodingMap.insert(pair<uint8_t, wchar_t>(151, 224));
	encodingMap.insert(pair<uint8_t, wchar_t>(152, 225));
	encodingMap.insert(pair<uint8_t, wchar_t>(153, 226));
	encodingMap.insert(pair<uint8_t, wchar_t>(227, 227));
	encodingMap.insert(pair<uint8_t, wchar_t>(154, 228));
	encodingMap.insert(pair<uint8_t, wchar_t>(229, 229));
	encodingMap.insert(pair<uint8_t, wchar_t>(155, 230));
	encodingMap.insert(pair<uint8_t, wchar_t>(156, 231));
	encodingMap.insert(pair<uint8_t, wchar_t>(157, 232));
	encodingMap.insert(pair<uint8_t, wchar_t>(158, 233));
	encodingMap.insert(pair<uint8_t, wchar_t>(159, 234));
	encodingMap.insert(pair<uint8_t, wchar_t>(160, 235));
	encodingMap.insert(pair<uint8_t, wchar_t>(161, 236));
	encodingMap.insert(pair<uint8_t, wchar_t>(162, 237));
	encodingMap.insert(pair<uint8_t, wchar_t>(163, 238));
	encodingMap.insert(pair<uint8_t, wchar_t>(164, 239));
	encodingMap.insert(pair<uint8_t, wchar_t>(240, 240));
	encodingMap.insert(pair<uint8_t, wchar_t>(174, 241));
	encodingMap.insert(pair<uint8_t, wchar_t>(165, 242));
	encodingMap.insert(pair<uint8_t, wchar_t>(166, 243));
	encodingMap.insert(pair<uint8_t, wchar_t>(167, 244));
	encodingMap.insert(pair<uint8_t, wchar_t>(245, 245));
	encodingMap.insert(pair<uint8_t, wchar_t>(168, 246));
	encodingMap.insert(pair<uint8_t, wchar_t>(247, 247));
	encodingMap.insert(pair<uint8_t, wchar_t>(248, 248));
	encodingMap.insert(pair<uint8_t, wchar_t>(169, 249));
	encodingMap.insert(pair<uint8_t, wchar_t>(170, 250));
	encodingMap.insert(pair<uint8_t, wchar_t>(171, 251));
	encodingMap.insert(pair<uint8_t, wchar_t>(172, 252));
	encodingMap.insert(pair<uint8_t, wchar_t>(253, 253));
	encodingMap.insert(pair<uint8_t, wchar_t>(254, 254));
	encodingMap.insert(pair<uint8_t, wchar_t>(255, 255));


	if (version == VER2) {
		char tabl[4];
		stream->read(tabl, 4);
	}

	int32_t tblBlockSize;
	stream->read((char*) &tblBlockSize, 4);
	numTables = tblBlockSize/12;

	headers = new GXTTableHeader*[numTables];

	for (int32_t i = 0 ; i < numTables ; i++) {
		GXTTableHeader* header = new GXTTableHeader;
		headers[i] = header;
		stream->read((char*) header, sizeof(GXTTableHeader));
	}

	position = 12 + tblBlockSize;
}


GXTTable* GXTArchive::readTable(GXTTableHeader* header)
{
	long long offset = header->offset - position;

	if (version == VER1) {
		offset += 4;
	}

	if (offset >= 0) {
		char skipBuffer[2048];
		SkipBytes(stream, offset, skipBuffer, sizeof(skipBuffer));
		position += offset;
	} else {
		stream->seekg(offset, istream::cur);
	}

	if (strcmp(header->name, "MAIN") != 0) {
		char tblName[8];
		stream->read(tblName, 8);
		position += 8;
	}

	char tkey[4];
	stream->read(tkey, 4);

	int32_t tkeySize;
	stream->read((char*) &tkeySize, 4);

	int32_t numEntries;

	position += 8;

	if (version == VER2) {
		numEntries = tkeySize/8;

		KeyEntry** keys = new KeyEntry*[numEntries];

		for (int32_t i = 0 ; i < numEntries ; i++) {
			KeyEntry* key = new KeyEntry;
			stream->read((char*) key, sizeof(KeyEntry));
			keys[i] = key;
		}

		sort(keys, keys+numEntries, _KeySortComparator);

		position += tkeySize;

		char tdat[4];
		stream->read(tdat, 4);
		position += 4;

		int32_t tdatSize;
		stream->read((char*) &tdatSize, 4);

		GXTTable::value_t** values = new GXTTable::value_t*[numEntries];

		for (int32_t i = 0 ; i < numEntries ; i++) {
			int32_t end;

			if (i != numEntries-1) {
				end = keys[i+1]->offset;
			} else {
				end = tdatSize;
			}

			int strlen = end - keys[i]->offset;

			uint8_t* rawValue = new uint8_t[strlen];
			stream->read((char*) rawValue, strlen);
			values[i] = decode8(rawValue, strlen);
			printf("Encoded: %s\n", rawValue);
			printf("Decoded: %ls\n", values[i]);
			exit(0);
			delete[] rawValue;

			//values[i] = new char[strlen];
			//stream->read(values[i], strlen);

			/*char* value = new char[strlen];
			stream->read(value, strlen);

			values[i] = decode8(value, strlen);

			delete[] value;*/
		}

		GXTTable::EntryMap* entries = new GXTTable::EntryMap;

		for (int32_t i = 0 ; i < numEntries ; i++) {
			entries->insert(pair<int32_t, const GXTTable::value_t*>(keys[i]->hash, values[i]));
			delete keys[i];
		}

		delete[] keys;
		delete[] values;

		return new GXTTable(entries);
	} /*else if (version == VER1) {
		numEntries = tkeySize/12;

		Ver1KeyEntry** keys = new Ver1KeyEntry*[numEntries];

		for (int32_t i = 0 ; i < numEntries ; i++) {
			Ver1KeyEntry* key = new Ver1KeyEntry;
			stream->read((char*) key, sizeof(Ver1KeyEntry));
			keys[i] = key;
		}

		sort(keys, keys+numEntries, _Ver1KeySortComparator);

		position += tkeySize;

		char tdat[4];
		stream->read(tdat, 4);
		position += 4;

		int32_t tdatSize;
		stream->read((char*) &tdatSize, 4);

		char** values = new char*[numEntries];

		for (int32_t i = 0 ; i < numEntries ; i++) {
			int32_t end;

			if (i != numEntries-1) {
				end = keys[i+1]->offset;
			} else {
				end = tdatSize;
			}

			int strlen = end - keys[i]->offset;

			int pos = (int) stream->tellg();

			char* utf16Value = new char[strlen];
			stream->read(utf16Value, strlen);

			iconv_t conv = iconv_open("UTF-8", "UTF-16");
			if (conv == (iconv_t)(-1)) {
				printf("Error 1!\n");
				exit(0);
			}

			char* utf8Value = new char[strlen];
			size_t inLeft = strlen;
			size_t outLeft = strlen;

			char* utf16ValueCpy = utf16Value;
			char* utf8ValueCpy = utf8Value;

			if (iconv(conv, &utf16ValueCpy, &inLeft, &utf8ValueCpy, &outLeft) == (size_t) (-1)) {
				printf("Error 2!\n");
				printf("%s\n", strerror(errno));

				if (errno == EINVAL) {
					printf("is EINVAL!\n");
				}

				exit(0);
			}
			values[i] = utf8Value;

			delete[] utf16Value;

			printf("The sequence (%d) is (\u00e4): %s\n", pos, utf8Value);

			//exit(0);

		}

		GXTTable::EntryMap* entries = new GXTTable::EntryMap;
		GXTPlainKeyTable::KeyMap* keyMap = new GXTPlainKeyTable::KeyMap;

		for (int32_t i = 0 ; i < numEntries ; i++) {
			int32_t hash = Crc32(keys[i]->key);
			keyMap->insert(pair<int32_t, const char*>(hash, keys[i]->key));
			entries->insert(pair<int32_t, const char*>(hash, values[i]));
			delete keys[i];
		}

		delete[] keys;
		delete[] values;

		return new GXTPlainKeyTable(entries, keyMap);
	}*/

	return NULL;
}





