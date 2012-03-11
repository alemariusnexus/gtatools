/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of gtaformats-test.

	gtaformats-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "global.h"
#include <gtaformats/util/util.h>
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/CString.h>
#include <gtaformats/util/Cache.h>
#include <gtaformats/util/CLIParser.h>
#include <gtaformats/util/CRC32.h>
#include <gtaformats/gta.h>
#include <gtaformats/util/File.h>
#include <vector>
#include <algorithm>
#include <gtaformats/util/math/intersection.h>
#include <gtaformats/util/math/Matrix3.h>
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/util/math/Quaternion.h>
#include <gtaformats/util/math/Vector3.h>
#include <gtaformats/util/math/Vector4.h>
#include <gtaformats/util/math/project.h>

using std::vector;
using std::find;



TEST(UtilTest, CheckUtilFunctions)
{
	for (int i = 0 ; i < 100 ; i++) {
		float v = RandomFloat(0.0f, 5.0f);
		EXPECT_TRUE(v >= 0.0f  &&  v <= 5.0f) << "RandomFloat() generated value out of range!";
	}

	EXPECT_TRUE(IsTimeBetween(10, 33, 9, 30, 12, 0));
	EXPECT_TRUE(IsTimeBetween(1, 10, 23, 20, 2, 0));
	EXPECT_TRUE(IsTimeBetween(0, 0, 23, 0, 1, 0));
	EXPECT_FALSE(IsTimeBetween(15, 0, 12, 0, 14, 0));

	EXPECT_EQ(1, GetNextPowerOfTwo(1));
	EXPECT_EQ(2, GetNextPowerOfTwo(2));
	EXPECT_EQ(4, GetNextPowerOfTwo(3));
	EXPECT_EQ(4, GetNextPowerOfTwo(4));
	EXPECT_EQ(16, GetNextPowerOfTwo(14));
	EXPECT_EQ(256, GetNextPowerOfTwo(200));
	EXPECT_EQ(67108864, GetNextPowerOfTwo(67100000));

	struct EndianTest16 {
		uint16_t base;
		uint16_t swapped;
	};
	struct EndianTest32 {
		uint32_t base;
		uint32_t swapped;
	};
	struct EndianTest64 {
		uint64_t base;
		uint64_t swapped;
	};

	EndianTest16 et16s[] = {
			{0xAABB, 0xBBAA},
			{0x1234, 0x3412},
			{0xFFFF, 0xFFFF}
	};

	EndianTest32 et32s[] = {
			{0xAABBCCDD, 0xDDCCBBAA},
			{0x12345678, 0x78563412},
			{0xFFFFFFFF, 0xFFFFFFFF}
	};

	EndianTest64 et64s[] = {
			{0x1234567890ABCDEFLL, 0xEFCDAB9078563412LL},
			{0x1122334455667788LL, 0x8877665544332211LL},
			{0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL}
	};

	for (unsigned int i = 0 ; i < sizeof(et16s) / sizeof(EndianTest16) ; i++) {
		EXPECT_EQ(et16s[i].swapped, (uint16_t) SwapEndianness16(et16s[i].base)) << "SwapEndianness16 test #"
				<< i << " failed!";

#ifdef GTAFORMATS_LITTLE_ENDIAN
		EXPECT_EQ(et16s[i].base, ToLittleEndian16(et16s[i].base)) << "ToLittleEndian16 test #" << i
				<< " failed!";
		EXPECT_EQ(et16s[i].swapped, ToBigEndian16(et16s[i].base)) << "ToBigEndian16 test #" << i
				<< " failed!";
		EXPECT_EQ(et16s[i].base, FromLittleEndian16(et16s[i].base)) << "FromLittleEndian16 test #" << i
				<< " failed!";
		EXPECT_EQ(et16s[i].swapped, FromBigEndian16(et16s[i].base)) << "FromBigEndian16 test #" << i
				<< " failed!";
#else
		EXPECT_EQ(et16s[i].swapped, ToLittleEndian16(et16s[i].base)) << "ToLittleEndian16 test #" << i
				<< " failed!";
		EXPECT_EQ(et16s[i].base, ToBigEndian16(et16s[i].base)) << "ToBigEndian16 test #" << i
				<< " failed!";
		EXPECT_EQ(et16s[i].swapped, FromLittleEndian16(et16s[i].base)) << "FromLittleEndian16 test #" << i
				<< " failed!";
		EXPECT_EQ(et16s[i].base, FromBigEndian16(et16s[i].base)) << "FromBigEndian16 test #" << i
				<< " failed!";
#endif
	}
	for (unsigned int i = 0 ; i < sizeof(et32s) / sizeof(EndianTest32) ; i++) {
		EXPECT_EQ(et32s[i].swapped, (uint32_t) SwapEndianness32(et32s[i].base)) << "SwapEndianness32 test #"
				<< i << " failed!";

#ifdef GTAFORMATS_LITTLE_ENDIAN
		EXPECT_EQ(et32s[i].base, (uint32_t) ToLittleEndian32(et32s[i].base)) << "ToLittleEndian32 test #"
				<< i << " failed!";
		EXPECT_EQ(et32s[i].swapped, (uint32_t) ToBigEndian32(et32s[i].base)) << "ToBigEndian32 test #"
				<< i << " failed!";
		EXPECT_EQ(et32s[i].base, (uint32_t) FromLittleEndian32(et32s[i].base)) << "FromLittleEndian32 test #"
				<< i << " failed!";
		EXPECT_EQ(et32s[i].swapped, (uint32_t) FromBigEndian32(et32s[i].base)) << "FromBigEndian32 test #"
				<< i << " failed!";
#else
		EXPECT_EQ(et32s[i].swapped, (uint32_t) ToLittleEndian32(et32s[i].base)) << "ToLittleEndian32 test #"
				<< i << " failed!";
		EXPECT_EQ(et32s[i].base, (uint32_t) ToBigEndian32(et32s[i].base)) << "ToBigEndian32 test #"
				<< i << " failed!";
		EXPECT_EQ(et32s[i].swapped, (uint32_t) FromLittleEndian32(et32s[i].base))
				<< "FromLittleEndian32 test #" << i << " failed!";
		EXPECT_EQ(et32s[i].base, (uint32_t) FromBigEndian32(et32s[i].base)) << "FromBigEndian32 test #"
				<< i << " failed!";
#endif
	}
	for (unsigned int i = 0 ; i < sizeof(et64s) / sizeof(EndianTest64) ; i++) {
		EXPECT_EQ(et64s[i].swapped, (uint64_t) SwapEndianness64(et64s[i].base)) << "SwapEndianness64 test #"
				<< i << " failed!";

#ifdef GTAFORMATS_LITTLE_ENDIAN
		EXPECT_EQ(et64s[i].base, (uint64_t) ToLittleEndian64(et64s[i].base)) << "ToLittleEndian64 test #"
				<< i << " failed!";
		EXPECT_EQ(et64s[i].swapped, (uint64_t) ToBigEndian64(et64s[i].base)) << "ToBigEndian64 test #"
				<< i << " failed!";
		EXPECT_EQ(et64s[i].base, (uint64_t) FromLittleEndian64(et64s[i].base)) << "FromLittleEndian64 test #"
				<< i << " failed!";
		EXPECT_EQ(et64s[i].swapped, (uint64_t) FromBigEndian64(et64s[i].base)) << "FromBigEndian64 test #"
				<< i << " failed!";
#else
		EXPECT_EQ(et64s[i].swapped, (uint64_t) ToLittleEndian64(et64s[i].base)) << "ToLittleEndian64 test #"
				<< i << " failed!";
		EXPECT_EQ(et64s[i].base, (uint64_t) ToBigEndian64(et64s[i].base)) << "ToBigEndian64 test #"
				<< i << " failed!";
		EXPECT_EQ(et64s[i].swapped, (uint64_t) FromLittleEndian64(et64s[i].base))
				<< "FromLittleEndian64 test #" << i << " failed!";
		EXPECT_EQ(et64s[i].base, (uint64_t) FromBigEndian64(et64s[i].base)) << "FromBigEndian64 test #"
				<< i << " failed!";
#endif
	}
}


TEST(UtilTest, CheckStrutilAndCString)
{
	struct CaseConvTest {
		const char* src;
		const char* dst;
		size_t len;
	};

	CaseConvTest utests[] = {
			{"Hallo Welt!",								"HALLO WELT!",							0},
			{" ?%StrINg  MIt}So**nderzei$$cH!en ",		" ?%STRING  MIT}SO**NDERZEI$$CH!EN ",	0},
			{"F",										"F",									0},
			{"a",										"A",									0},
			{"Fubar!",									"FUbar!",								2},
			{"The quick brown fox",						"THE QUICk brown fox",					8}
	};
	CaseConvTest ltests[] = {
			{"Hallo Welt!",								"hallo welt!",							0},
			{" ?%StrINg  MIt}So**nderzei$$cH!en ",		" ?%string  mit}so**nderzei$$ch!en ",	0},
			{"F",										"f",									0},
			{"a",										"a",									0},
			{"FubAr!",									"fubAr!",								2},
			{"The QUick bROWn fox",						"the quick bROWn fox",					8}
	};

	for (unsigned int i = 0 ; i < sizeof(utests) / sizeof(CaseConvTest) ; i++) {
		CaseConvTest& t = utests[i];
		char dst[512];

		if (t.len == 0) {
			strtoupper(dst, t.src);
		} else {
			strcpy(dst, t.src);
			strtoupper(dst, t.src, t.len);
		}

		EXPECT_STREQ(t.dst, dst) << "strtoupper test #" << i << " failed!";

		if (t.len == 0) {
			CString csrc(t.src);
			EXPECT_EQ(CString(t.dst), csrc.upper()) << "CString::upper() test #" << i << " failed!";
		}
	}
	for (unsigned int i = 0 ; i < sizeof(ltests) / sizeof(CaseConvTest) ; i++) {
		CaseConvTest& t = ltests[i];
		char dst[512];

		if (t.len == 0) {
			strtolower(dst, t.src);
		} else {
			strcpy(dst, t.src);
			strtolower(dst, t.src, t.len);
		}

		EXPECT_STREQ(t.dst, dst) << "strtolower test #" << i << " failed!";

		if (t.len == 0) {
			CString csrc(t.src);
			EXPECT_EQ(CString(t.dst), csrc.lower()) << "CString::lower() test #" << i << " failed!";
		}
	}

	enum TrimMode { Left, Right, Both };

	struct TrimTest {
		const char* src;
		const char* dst;
		char c;
		TrimMode mode;
	};

	TrimTest trims[] = {
			{"   Hallo Welt!  ",				"Hallo Welt!  ",				' ',	Left},
			{"    F  ranz! ",					"F  ranz! ",					' ',	Left},
			{"Hallo! ",							"Hallo! ",						' ',	Left},
			{"/home/alemariusnexus",			"home/alemariusnexus",			'/',	Left},
			{"/home/alemariusnexus/",			"/home/alemariusnexus",			'/',	Right},
			{"/home/alemariusnexus/",			"home/alemariusnexus",			'/',	Both},
			{"  <a>Hallo Welt!</a>  ",			"<a>Hallo Welt!</a>",			' ',	Both},
			{"None",							"None",							' ',	Both},
			{"  ",								"",								' ',	Left},
			{"  ",								"",								' ',	Right},
			{"  ",								"",								' ',	Both},
			{"",								"",								' ',	Left},
			{"",								"",								' ',	Right},
			{"",								"",								' ',	Both},
	};

	for (unsigned int i = 0 ; i < sizeof(trims) / sizeof(TrimTest) ; i++) {
		TrimTest& t = trims[i];

		if (t.mode == Left) {
			EXPECT_STREQ(t.dst, ltrim(t.src, t.c)) << "Trim test #" << i << " failed!";
			EXPECT_EQ(CString(t.dst), CString(t.src).ltrim(t.c)) << "CString trim test #" << i << " failed!";
		} else if (t.mode == Right) {
			char buf[512];
			strcpy(buf, t.src);
			rtrim(buf, t.c);
			EXPECT_STREQ(t.dst, buf) << "Trim test #" << i << " failed!";
			EXPECT_EQ(CString(t.dst), CString(t.src).rtrim(t.c)) << "CString trim test #" << i << " failed!";
		} else {
			EXPECT_EQ(CString(t.dst), CString(t.src).trim(t.c)) << "CString trim test #" << i << " failed!";
		}
	}

	struct IndentTest {
		const char* src;
		const char* dst;
		const char* ind;
	};

	IndentTest indents[] = {
			{	"  Hallo Welt!\nDieser Text sollte\n  eingerueckt werden,\n\ndenke ich!",
				"  Hallo Welt!\n  Dieser Text sollte\n    eingerueckt werden,\n  \n  denke ich!",
				"  "
			},
			{	"",
				"",
				"Franz"
			},
			{	"\n",
				"\nHallo Welt!",
				"Hallo Welt!"
			}
	};

	for (unsigned int i = 0 ; i < sizeof(indents) / sizeof(IndentTest) ; i++) {
		IndentTest& t = indents[i];
		char* indRes = indent(t.src, t.ind);
		EXPECT_STREQ(t.dst, indRes) << "indent() test #" << i << " failed!";
		delete[] indRes;
	}

	struct WildcardTest {
		const char* pattern;
		const char* str;
		bool matches;
	};

	WildcardTest wildcards[] = {
			{"*",								"*Hallo Welt!#**?aa",								true},
			{"*",								"",													true},
			{"Ha*t",							"Hallo Welt",										true},
			{"Ha*t",							"Hallo Welt!",										false},
			{"Hallo We?t!",						"Hallo Welt!",										true},
			{"Hallo We?t!",						"Hallo WeXt!",										true},
			{"Hallo We?t!",						"Hallo Wellt!",										false},
			{"Ich * ?ich!",						"Ich mag dich!",									true},
			{"Ich * ?ich!",						"Ich mag mich!",									true},
			{"Ich * ?ich!",						"Ich hasse dich!",									true},
			{"Ich * ?ich!",						"Ich bin ich!",										false},
			{"Ich * ?ich!",						"Ich wich!",										false},
			{"Ich * ?ich!",						"Ich  wich!",										true}
	};

	for (unsigned int i = 0 ; i < sizeof(wildcards) / sizeof(WildcardTest) ; i++) {
		WildcardTest& t = wildcards[i];

		if (t.matches) {
			EXPECT_TRUE(WildcardMatch(t.pattern, t.str)) << "Wildcard pattern #" << i << " (" << t.pattern
					<< ") didn't match the text (" << t.str << ")!";
		} else {
			EXPECT_FALSE(WildcardMatch(t.pattern, t.str)) << "Wildcard pattern #" << i << " (" << t.pattern
					<< ") matched the text (" << t.str << "), although it shouldn't!";
		}
	}

	EXPECT_EQ(Hash("hallo welt!"), LowerHash("Hallo Welt!")) << "LowerHash() function does not work!";


	char* tstr1 = new char[64];
	strcpy(tstr1, "Hallo schoene Welt!");
	CString cstr1(tstr1);
	EXPECT_EQ(CString("Hallo schoene Welt!"), cstr1);
	strcpy(tstr1, "Hallo grausame Welt!");
	EXPECT_EQ(CString("Hallo schoene Welt!"), cstr1);
	delete[] tstr1;

	char* tstr2 = new char[64];
	strcpy(tstr2, "Hallo schoene Welt!");
	CString cstr2 = CString::from(tstr2);
	EXPECT_EQ(CString("Hallo schoene Welt!"), cstr2);
	strcpy(tstr2, "Hallo grausame Welt!");
	EXPECT_EQ(CString("Hallo grausame Welt!"), cstr2);

	EXPECT_EQ(CString(NULL), CString());
	EXPECT_TRUE(CString().get() == NULL);

	cstr1 = CString("  ##Hallo Welt!#### ");
	cstr2 = cstr1;
	EXPECT_EQ(cstr1, CString("  ##Hallo Welt!#### "));
	EXPECT_EQ(cstr2, CString("  ##Hallo Welt!#### "));
	cstr2.lower();
	EXPECT_EQ(cstr1, CString("  ##Hallo Welt!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!#### "));
	cstr1.upper();
	EXPECT_EQ(cstr1, CString("  ##HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!#### "));
	EXPECT_EQ(20, cstr1.length());
	EXPECT_EQ(20, cstr2.length());
	cstr1.ltrim();
	EXPECT_EQ(cstr1, CString("##HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!#### "));
	EXPECT_EQ(18, cstr1.length());
	EXPECT_EQ(20, cstr2.length());
	cstr2.rtrim();
	EXPECT_EQ(cstr1, CString("##HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!####"));
	cstr1.trim('#');
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("  ##hallo welt!####"));
	cstr2.trim(' ');
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("##hallo welt!####"));
	cstr2.trim('#');
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("hallo welt!"));
	cstr2[0] = 'H';
	cstr2[6] = 'W';
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### "));
	EXPECT_EQ(cstr2, CString("Hallo Welt!"));
	cstr1.append(cstr2);
	EXPECT_EQ(cstr1, CString("HALLO WELT!#### Hallo Welt!"));
	EXPECT_EQ(cstr2, CString("Hallo Welt!"));
	EXPECT_EQ(27, cstr1.length());
	EXPECT_EQ(11, cstr2.length());

	cstr1 = CString("Hallo Welt!");
	cstr2 = cstr1;
	EXPECT_EQ(cstr1, cstr2);
	cstr1[6] = 'G';
	cstr1[9] = 'd';
	EXPECT_EQ(cstr1, CString("Hallo Geld!"));
	EXPECT_EQ(cstr2, CString("Hallo Welt!"));
	EXPECT_NE(cstr1, cstr2);

	cstr1 = CString("Hallo Welt!", 5);
	EXPECT_EQ(CString("Hallo"), cstr1);

	EXPECT_NE(CString(), CString("Hallo Welt!"));
	EXPECT_NE(CString("Hallo Welt!"), CString());

	EXPECT_LT(CString("Hallo Geld!"), CString("Hallo Welt"));
	EXPECT_GT(CString("Ich"), CString("Du"));
	EXPECT_GT(CString("Liebe"), CString("Geld"));
	EXPECT_LT(CString("Boese"), CString("Gut"));
}


TEST(UtilTest, CheckCacheCString)
{
	typedef Cache<CString, CString> StrCache;

	StrCache c(100);

	CString* str1 = new CString("Hallo grausame Welt!"); // 20
	CString* str2 = new CString("Lorem ipsum dolor sit amet"); // 26
	CString* str3 = new CString("Und auch ich bin ein CString!"); // 29
	CString* str4 = new CString("Dieser Text wird einen verdraengen!"); // 35

	c.insert("key1", str1, str1->length());
	c.insert("key2", str2, str2->length());
	c.insert("key3", str3, str3->length());

	EXPECT_TRUE(c.item("key1") != NULL);
	EXPECT_TRUE(c.item("key2") != NULL);
	EXPECT_TRUE(c.item("key3") != NULL);

	c.item("key1");

	c.insert("key4", str4, str4->length());

	EXPECT_TRUE(c.item("key4") != NULL);
	EXPECT_TRUE(c.item("key1") != NULL);
	EXPECT_TRUE(c.item("key3") != NULL);
	EXPECT_TRUE(c.item("key2") == NULL);

	c.item("key4");

	c.resize(70);

	EXPECT_TRUE(c.item("key4") != NULL);
	EXPECT_TRUE(c.item("key3") != NULL);
	EXPECT_TRUE(c.item("key1") == NULL);
	EXPECT_EQ(64, c.getOccupiedSize());

	c.remove("key4");

	EXPECT_TRUE(c.item("key4") == NULL);
	EXPECT_TRUE(c.item("key3") != NULL);
	EXPECT_EQ(29, c.getOccupiedSize());

	c.resize(100);

	EXPECT_TRUE(c.item("key4") == NULL);
	EXPECT_TRUE(c.item("key3") != NULL);
	EXPECT_EQ(29, c.getOccupiedSize());

	// Only str3 is left and valid now

	str1 = new CString("Und hier kommt ein neuer str1!"); // 30
	str2 = new CString("Krawehl, krawehl"); // 16
	str4 = new CString("Und dieser String hier ist mal wieder zu lang!"); // 46

	c.lock("key3", true);

	c.insert("key1", str1, str1->length());
	c.insert("key2", str2, str2->length());

	EXPECT_EQ(75, c.getOccupiedSize());

	c.insert("key4", str4, str4->length());

	EXPECT_TRUE(c.item("key4") != NULL);
	EXPECT_TRUE(c.item("key2") != NULL);
	EXPECT_TRUE(c.item("key3") != NULL); // key3 was locked
	EXPECT_TRUE(c.item("key1") == NULL);
	EXPECT_EQ(91, c.getOccupiedSize());

	// key2, key3 and key4 are left. key3 is locked

	c.lock("key2");
	c.lock("key4");

	EXPECT_EQ(91, c.getOccupiedSize());

	str1 = new CString("Dieser String hier wird den Cache ueberfuellen!"); // 47

	EXPECT_FALSE(c.insert("key1", str1, str1->length()));
	EXPECT_EQ(91, c.getOccupiedSize());

	str1 = new CString("Dieser String hier wird den Cache ueberfuellen!"); // 47

	EXPECT_TRUE(c.insert("key1", str1, str1->length(), true));

	EXPECT_EQ(138, c.getOccupiedSize());

	c.resize(70);

	EXPECT_EQ(138, c.getOccupiedSize());

	EXPECT_FALSE(c.clear());

	EXPECT_EQ(138, c.getOccupiedSize());

	EXPECT_TRUE(c.item("key1") != NULL);
	EXPECT_TRUE(c.item("key2") != NULL);
	EXPECT_TRUE(c.item("key3") != NULL);
	EXPECT_TRUE(c.item("key4") != NULL);

	c.resize(100);

	EXPECT_EQ(138, c.getOccupiedSize());

	c.unlock("key2");

	EXPECT_EQ(122, c.getOccupiedSize());

	c.unlock("key3");

	EXPECT_EQ(93, c.getOccupiedSize());

	c.unlock("key1");
	c.unlock("key4");

	EXPECT_EQ(93, c.getOccupiedSize());

	EXPECT_TRUE(c.item("key1") != NULL);
	EXPECT_TRUE(c.item("key4") != NULL);
	EXPECT_TRUE(c.item("key2") == NULL);
	EXPECT_TRUE(c.item("key3") == NULL);

	// key1 (47) and key4 (46) are left

	str2 = new CString("Ich bin ein so langer Text, dass ich alleine schon die Kapazitaet des Caches "
			"sprenge und daher gar nicht erst in ihn aufgenommen werde!"); // 135

	EXPECT_FALSE(c.insert("key2", str2, str2->length()));

	EXPECT_EQ(93, c.getOccupiedSize());

	EXPECT_TRUE(c.item("key1") != NULL);
	EXPECT_TRUE(c.item("key4") != NULL);
	EXPECT_TRUE(c.item("key2") == NULL);

	str2 = new CString("Ich bin ein so langer Text, dass ich alleine schon die Kapazitaet des Caches "
			"sprenge und daher gar nicht erst in ihn aufgenommen werde!"); // 135

	c.remove("key4");

	EXPECT_EQ(47, c.getOccupiedSize());
	EXPECT_TRUE(c.item("key1") != NULL);
	EXPECT_TRUE(c.item("key4") == NULL);

	EXPECT_TRUE(c.insert("key2", str2, str2->length(), true));

	EXPECT_EQ(135, c.getOccupiedSize());

	EXPECT_TRUE(c.item("key2") != NULL);
	EXPECT_TRUE(c.item("key1") == NULL);
	EXPECT_TRUE(c.item("key4") == NULL);

	c.clear();

	EXPECT_EQ(135, c.getOccupiedSize());
	EXPECT_TRUE(c.item("key2") != NULL);

	c.resize(135);

	c.unlock("key2");

	EXPECT_EQ(135, c.getOccupiedSize());

	c.resize(134);

	EXPECT_EQ(0, c.getOccupiedSize());

	EXPECT_TRUE(c.item("key1") == NULL);
	EXPECT_TRUE(c.item("key2") == NULL);
	EXPECT_TRUE(c.item("key3") == NULL);
	EXPECT_TRUE(c.item("key4") == NULL);

	// When an entry is unlocked in an overfilled cache, the unlocked entry is instantly deleted by
	// Cache::lock(). Here we test if Cache::lock() returns the NULL pointer in this case.
	c.resize(0);
	str1 = new CString("Hallo Welt!");
	c.insert("key1", str1, str1->length(), true);

	EXPECT_EQ(11, c.getOccupiedSize());

	CString* unlockedStr1 = c.unlock("key1");

	EXPECT_TRUE(unlockedStr1 == NULL);

	EXPECT_EQ(0, c.getOccupiedSize());
}


TEST(UtilTest, CheckCLIParser)
{
	CLIParser cli;

	int optA = cli.addOption('a', "option-a", "This is option A", false);
	int optB = cli.addOption('b', "option-b", "This is option B", true);
	int optC = cli.addOption(0, "option-c", "This is option C", false);
	int optD = cli.addOption('d', NULL, "This is option D", true);
	int optE = cli.addOption('e', "option-e", "This is option E", true);

	CString cargv[] = {
			"UncatOpt1", "-d" "Hallo Welt!", "-a", "And another uncategorized #?! one",
			"-eDirectlyAppended", "--option-b", "1", "--option-c", "And some values", "are following"
	};
	int argc = sizeof(cargv) / sizeof(CString);
	int oldArgc = argc;

	char** argv = new char*[argc];
	char** oldArgv = argv;

	for (unsigned int i = 0 ; i < argc ; i++) {
		argv[i] = new char[cargv[i].length()+1];
		strcpy(argv[i], cargv[i].get());
	}

	int opt;
	char* arg;

	bool aFound = false, bFound = false, cFound = false, dFound = false, eFound = false;
	int numUncat = 0;

	while ((opt = cli.parse(argc, argv, arg))  >=  0) {
		if (opt == optA) {
			aFound = true;
		} else if (opt == optB) {
			bFound = true;
			EXPECT_STREQ("1", arg) << "Argument of option -b is wrong!";
		} else if (opt == optC) {
			cFound = true;
		} else if (opt == optD) {
			dFound = true;
			EXPECT_STREQ("Hallo Welt!", arg) << "Argument of option -d is wrong!";
		} else if (opt == optE) {
			eFound = true;
			EXPECT_STREQ("DirectlyAppended", arg) << "Argument of option -e is wrong!";
		} else if (opt == 0) {
			switch (numUncat) {
			case 0:
				EXPECT_STREQ("UncatOpt1", arg) << "Wrong uncategorized option 1!";
				break;
			case 1:
				EXPECT_STREQ("And another uncategorized #?! one", arg) << "Wrong uncategorized option 2!";
				break;
			case 2:
				EXPECT_STREQ("And some values", arg) << "Wrong uncategorized option 3!";
				break;
			case 3:
				EXPECT_STREQ("are following", arg) << "Wrong uncategorized option 4!";
				break;
			default:
				ADD_FAILURE() << "Too many uncategorized options!";
				break;
			}

			numUncat++;
		} else {
			ADD_FAILURE() << "Invalid option: " << opt;
		}
	}

	EXPECT_TRUE(aFound) << "Option -a not found!";
	EXPECT_TRUE(bFound) << "Option -b not found!";
	EXPECT_TRUE(cFound) << "Option -c not found!";
	EXPECT_TRUE(dFound) << "Option -d not found!";
	EXPECT_TRUE(eFound) << "Option -e not found!";
	EXPECT_EQ(4, numUncat) << "Some uncategorized options were not found!";

	for (int i = 0 ; i < oldArgc ; i++) {
		delete[] oldArgv[i];
	}

	delete[] oldArgv;
}


TEST(UtilTest, CheckCRC32)
{
	CRC32 crc;

	struct CRC32Test {
		const char* data;
		uint32_t checksum;
	};

	CRC32Test crcs[] = {
			{"A_TIME",												0x483F7562},
			{"ABATOIR",												0x3C9B124E},
			{"ZER4_OG",												0x7F80108E},
			{"BRIDGE",												0x667F37E6},
			{"LOC4_BG",												0x323B5A63},
			{"DRV3_2",												0xA5989121},
			{"MWUZ06E",												0xA94662F0},
			{"INTRO2E",												0xBAC5FE86},
			{"INFERNU",												0xE7E843FF},
			{"RYD3_AD",												0x4D86BAA6}
	};

	for (unsigned int i = 0 ; i < sizeof(crcs) / sizeof(CRC32Test) ; i++) {
		CRC32Test& t = crcs[i];

		crc.append(t.data);
		EXPECT_EQ(t.checksum, crc.getChecksum()) << "CRC32 test #" << i << " failed!";

		crc.clear();

		EXPECT_EQ(t.checksum, Crc32(t.data)) << "CRC32 function test #" << i << " failed!";
	}
}


TEST(UtilTest, CheckFile)
{
	File testFile("/home/alemariusnexus/A Path With Whitespaces/hello.txt");

	const FilePath* testPath = testFile.getPath();

	EXPECT_EQ(CString("hello.txt"), testPath->getFileName());
	EXPECT_EQ(CString("txt"), testPath->getExtension());


	File file = File::createTemporaryFile();

	ostream* out = file.openOutputStream();
	*out << "Hallo Welt!";
	delete out;

	istream* in = file.openInputStream();
	char data[32];
	in->read(data, sizeof(data)-1);
	data[in->gcount()] = '\0';
	delete in;

	EXPECT_STREQ("Hallo Welt!", data);

	bool removed = file.remove();
	EXPECT_TRUE(removed) << "Temporary file could not be removed!";

	if (removed) {
		EXPECT_FALSE(file.physicallyExists());
	} else {
		EXPECT_TRUE(file.physicallyExists());
	}


	File f1(gtasaRoot);
	File f2(f1, "data");
	File f3(f1, "data/maps");

	File* f3Parent = f3.getParent();
	EXPECT_EQ(f2, *f3Parent);
	delete f3Parent;

	File* f2Parent = f2.getParent();
	EXPECT_EQ(f1, *f2Parent);
	delete f2Parent;

	EXPECT_TRUE(f2.isChildOf(f1));
	EXPECT_FALSE(f3.isChildOf(f1));
	EXPECT_TRUE(f3.isChildOf(f1, true));

	EXPECT_TRUE(gtasaRoot.exists());
	EXPECT_TRUE(f2.exists());
	EXPECT_TRUE(f3.exists());

	EXPECT_TRUE(gtasaRoot.physicallyExists());
	EXPECT_TRUE(f2.physicallyExists());
	EXPECT_TRUE(f3.physicallyExists());

	File f4(gtasaRoot, "models/gta3.img/bistro.txd");

	EXPECT_FALSE(f4.physicallyExists());
	EXPECT_TRUE(f4.exists());

	EXPECT_EQ(TYPE_DIRECTORY, gtasaRoot.getType());
	EXPECT_EQ(TYPE_DIRECTORY, f2.getType());
	EXPECT_EQ(TYPE_DIRECTORY, f3.getType());
	EXPECT_EQ(TYPE_FILE, File(gtasaRoot, "models/gta3.img").getType());
	EXPECT_EQ(TYPE_FILE, f4.getType());

	EXPECT_TRUE(gtasaRoot.isDirectory());
	EXPECT_TRUE(f4.isRegularFile());
	EXPECT_FALSE(f4.isArchiveFile());


	File f5(gtasaRoot, "models/grass");
	FileIterator* it = f5.getIterator();
	File* child;
	int childCount = f5.getChildCount();

	const char* expectedNames[] = {
			"grass0_1.dff", "grass0_2.dff", "grass0_3.dff", "grass0_4.dff",
			"grass1_1.dff", "grass1_2.dff", "grass1_3.dff", "grass1_4.dff",
			"grass2_1.dff", "grass2_2.dff", "grass2_3.dff", "grass2_4.dff",
			"grass3_1.dff", "grass3_2.dff", "grass3_3.dff", "grass3_4.dff",
			"plant1.dff", "plant1.txd"

	};
	vector<CString> vecExNames;

	for (unsigned int i = 0 ; i < sizeof(expectedNames) / sizeof(const char*) ; i++) {
		vecExNames.push_back(expectedNames[i]);
	}

	EXPECT_EQ(vecExNames.size(), childCount);

	while ((child = it->next())  !=  NULL) {
		vector<CString>::iterator it = find(vecExNames.begin(), vecExNames.end(),
				CString(child->getPath()->getFileName()));

		EXPECT_NE(vecExNames.end(), it);

		if (it != vecExNames.end()) {
			vecExNames.erase(it);
		}

		delete child;
	}

	delete it;

	EXPECT_TRUE(vecExNames.empty());

	EXPECT_EQ(CONTENT_TYPE_COL, File(gtasaRoot, "models/coll/peds.col").guessContentType());
	EXPECT_EQ(CONTENT_TYPE_DAT, File(gtasaRoot, "data/gta.dat").guessContentType());
	EXPECT_EQ(CONTENT_TYPE_DFF, File(gtasaRoot, "hypothetical_file.DfF").guessContentType());
	EXPECT_EQ(CONTENT_TYPE_DIR, File(gtasaRoot, "DoesNotExist.DIR").guessContentType());
	EXPECT_EQ(CONTENT_TYPE_IDE, File(gtasaRoot, "AnIDEfile.ide").guessContentType());
	EXPECT_EQ(CONTENT_TYPE_IFP, File(gtasaRoot, "anim/ped.ifp").guessContentType());
	EXPECT_EQ(CONTENT_TYPE_IMG, File(gtasaRoot, "models/gta3.img").guessContentType());
	EXPECT_EQ(CONTENT_TYPE_IPL, File(gtasaRoot, "this/does/not/exist.ipL").guessContentType());
	EXPECT_EQ(CONTENT_TYPE_TXD, File(gtasaRoot, "models/fonts.txd").guessContentType());

	EXPECT_NO_THROW({
		File invalidFile(gtasaRoot, "ThisFileDoesNotExistgasd5g45asd");
		File invalidFileChild(invalidFile, "AchildOfIt");
	});

	// TODO Complete these tests
}


TEST(UtilTest, CheckMath)
{
	EXPECT_TRUE(IntersectAABoxSphere(20.0f, 20.0f, 20.0f, 100.0f, 50.0f, 50.0f, 50.0f, 150.0f, 150.0f, 150.0f));
	EXPECT_TRUE(IntersectAABoxSphere(0.0f, 0.0f, 0.0f, 100.0f, -10.0f, -10.0f, -10.0f, 10.0f, 10.0f, 10.0f));
	EXPECT_TRUE(IntersectAABoxSphere(0.0f, 0.0f, 0.0f, 10.0f, -100.0f, -100.0f, -100.0f,
			100.0f, 100.0f, 100.0f));
	EXPECT_FALSE(IntersectAABoxSphere(100.0f, 100.0f, 0.0f, 100.0f, 350.0f, 350.0f, 0.0f, 500.0f, 500.0f,
			50.0f));

	EXPECT_TRUE(IntersectSphereSphere(0.0f, 0.0f, 0.0f, 50.0f, 0.0f, 0.0f, 0.0f, 50.0f));
	EXPECT_TRUE(IntersectSphereSphere(50.0f, 50.0f, 50.0f, 100.0f, 0.0f, 0.0f, 0.0f, 100.0f));
	EXPECT_TRUE(IntersectSphereSphere(500.0f, 0.0f, 500.0f, 100.0f, 500.0f, 0.0f, 500.0f, 20.0f));
	EXPECT_FALSE(IntersectSphereSphere(50.0f, 50.0f, 50.0f, 50.0f, -75.0f, -75.0f, -75.0f, 50.0f));
}










