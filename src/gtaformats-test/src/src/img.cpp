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
#include <gtaformats/util/File.h>
#include <gtaformats/img/IMGArchive.h>
#include <gtaformats/util/CRC32.h>
#include <gtaformats/util/CString.h>
#include <algorithm>

using std::min;
using std::max;
using std::distance;




struct IMGExpectedEntry {
	CString name;
	int32_t size;
	uint32_t checksumLen;
	uint32_t checksum;
};



void TestIMGContents(IMGArchive* img, IMGExpectedEntry* exEntries, unsigned int numExEntries)
{
	CRC32 crc;

	for (unsigned int i = 0 ; i < numExEntries ; i++) {
		IMGExpectedEntry& exEntry = exEntries[i];

		if (!exEntry.name.get())
			continue;

		IMGArchive::EntryIterator it = img->getEntryByName(File(exEntry.name.get()).getPath()->getFileName());

		EXPECT_NE(img->getEntryEnd(), it) << "Test entry #" << i << " (" << exEntry.name.get() << ")"
				<< " not found!";

		if (it != img->getEntryEnd()) {
			EXPECT_EQ(exEntry.size, (*it)->size) << "Test entry #" << i << " (" << exEntry.name.get() << ") has "
					<< "wrong size!";

			if (exEntry.checksumLen > 0) {
				istream* stream = img->gotoEntry(it);

				char buf[2048];

				File::filesize numRead = 0;

				while (numRead < exEntry.checksumLen  &&  !stream->eof()) {
					int numToRead = min(exEntry.checksumLen - numRead, (File::filesize) sizeof(buf));
					stream->read(buf, numToRead);
					crc.append(buf, numToRead);
					numRead += numToRead;
				}

				delete stream;

				EXPECT_EQ(exEntry.checksum, crc.getChecksum()) << "Checksum of test entry #" << i << "("
						<< exEntry.name.get() << ") is not correct!";

				crc.clear();
			}
		}
	}
}




IMGExpectedEntry CreatedBaseEntries[] = {
		{"models/effects.fxp",						IMG_BYTES2BLOCKS(616708),		616708,		0xFDAE3431},
		{"models/fonts.txd",						IMG_BYTES2BLOCKS(524584),		524584,		0xF1A56646},
		{"models/generic/hoop.dff",					IMG_BYTES2BLOCKS(3701),			3701,		0x17421C32},
		{"data/maps/paths.ipl",						IMG_BYTES2BLOCKS(2658607),		2048000,	0x30847D83},
		{"data/maps/txd.ide",						IMG_BYTES2BLOCKS(323),			323,		0x3E07A97A},
		{"models/particle.txd",						IMG_BYTES2BLOCKS(650536),		650536,		0x9B601063},
		{"models/fronten_pc.txd",					IMG_BYTES2BLOCKS(2344),			2344,		0x117894F2},
		{"models/grass/grass0_3.dff",				IMG_BYTES2BLOCKS(1600),			1600,		0x5EFD2A9B},
		{"text/german.gxt",							IMG_BYTES2BLOCKS(812958),		812958,		0x91FADC64},
		{"data/object.dat",							IMG_BYTES2BLOCKS(126118),		126118,		0x7BD831CE}
};


unsigned int FillIMG(IMGArchive* img)
{
	unsigned int numEntries = sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry);

	img->reserveHeaderSpace(numEntries);

	for (unsigned int i = 0 ; i < numEntries ; i++) {
		IMGExpectedEntry& exEntry = CreatedBaseEntries[i];

		File inFile(rootDir, exEntry.name.get());

		IMGArchive::EntryIterator it = img->addEntry(File(exEntry.name.get()).getPath()->getFileName(),
				IMG_BYTES2BLOCKS(inFile.getSize()));

		iostream* stream = (iostream*) img->gotoEntry(it);

		inFile.copyTo(stream);

		delete stream;
	}

	return numEntries;
}


void TestIMGAddAndWrite(IMGArchive* img)
{
	unsigned int numEntries = FillIMG(img);

	EXPECT_EQ(numEntries, img->getEntryCount());

	TestIMGContents(img, CreatedBaseEntries, numEntries);
}


void TestIMGOrder(IMGArchive* img, const char** entries, unsigned int numEntries)
{
	if (numEntries == 0)
		return;

	IMGArchive::EntryIterator it;

	if (entries[0]) {
		it = img->getEntryByName(entries[0]);
		EXPECT_NE(img->getEntryEnd(), it) << "Range begin was not found!";

		if (it == img->getEntryEnd())
			return;

		it++;
	} else {
		it = img->getEntryBegin();
	}

	unsigned int i;
	for (i = 1 ; i < numEntries  &&  it != img->getEntryEnd() ; i++) {
		const char* entry = entries[i];

		if (strcmp((*it)->name, entry) != 0) {
			ADD_FAILURE() << "Order not correct!";
			return;
		}

		it++;
	}

	if (i < numEntries) {
		ADD_FAILURE() << "Order not correct!";
	}
}


IMGExpectedEntry* FindExpectedEntry(const CString& name, IMGExpectedEntry* exEntries,
		unsigned int numExEntries)
{
	CString baseName = File(name.get()).getPath()->getFileName();

	for (unsigned int i = 0 ; i < numExEntries ; i++) {
		IMGExpectedEntry* exEntry = exEntries+i;

		if (	exEntry->name.get()
				&&  strcmp(File(exEntry->name.get()).getPath()->getFileName(), baseName.get()) == 0
		) {
			return exEntry;
		}
	}

	return NULL;
}


void CopyExpectedEntries(IMGExpectedEntry* to, IMGExpectedEntry* from, unsigned int num)
{
	for (unsigned int i = 0 ; i < num ; i++) {
		to[i].name = from[i].name;
		to[i].size = from[i].size;
		to[i].checksum = from[i].checksum;
		to[i].checksumLen = from[i].checksumLen;
	}
}


void TestIMGMoveToBegin(IMGArchive* img, IMGArchive::MoveMode mmode)
{
	IMGArchive::EntryIterator beg, end, pos;

	pos = img->getEntryBegin();
	beg = img->getEntryByName("hoop.dff");
	end = img->getEntryByName("fronten_pc.txd");

	img->moveEntries(pos, beg, end, mmode);

	if (mmode == IMGArchive::MoveToEnd) {
		const char* eo[] = {NULL, "hoop.dff", "paths.ipl", "txd.ide", "particle.txd"};
		TestIMGOrder(img, eo, sizeof(eo) / sizeof(const char*));
	} else {
		const char* eo[] = {
				NULL, "hoop.dff", "paths.ipl", "txd.ide", "particle.txd", "effects.fxp", "fonts.txd",
				"fronten_pc.txd", "grass0_3.dff", "german.gxt", "object.dat"
		};
		TestIMGOrder(img, eo, sizeof(eo) / sizeof(const char*));
	}
}


void TestIMGMoveToEnd(IMGArchive* img, IMGArchive::MoveMode mmode)
{
	IMGArchive::EntryIterator beg, end, pos;

	pos = img->getEntryEnd();
	beg = img->getEntryByName("fonts.txd");
	end = img->getEntryByName("fronten_pc.txd");

	img->moveEntries(pos, beg, end, mmode);

	if (mmode == IMGArchive::MoveToEnd) {
		const char* eo[] = {"object.dat", "fonts.txd", "hoop.dff", "paths.ipl", "txd.ide", "particle.txd"};
		TestIMGOrder(img, eo, sizeof(eo) / sizeof(const char*));
	} else {
		const char* eo[] = {
				NULL, "effects.fxp", "fronten_pc.txd", "grass0_3.dff", "german.gxt", "object.dat",
				"fonts.txd", "hoop.dff", "paths.ipl", "txd.ide", "particle.txd"
		};
		TestIMGOrder(img, eo, sizeof(eo) / sizeof(const char*));
	}
}


void TestIMGMoveToMid(IMGArchive* img, IMGArchive::MoveMode mmode)
{
	IMGArchive::EntryIterator beg, end, pos;

	pos = img->getEntryByName("hoop.dff");
	beg = img->getEntryByName("particle.txd");
	end = img->getEntryByName("german.gxt");

	img->moveEntries(pos, beg, end, mmode);

	if (mmode == IMGArchive::MoveToEnd) {
		const char* eo[] = {"fonts.txd", "particle.txd", "fronten_pc.txd", "grass0_3.dff"};
		TestIMGOrder(img, eo, sizeof(eo) / sizeof(const char*));
	} else {
		const char* eo[] = {
				NULL, "effects.fxp", "fonts.txd", "particle.txd", "fronten_pc.txd", "grass0_3.dff",
				"hoop.dff", "paths.ipl", "txd.ide", "german.gxt", "object.dat"
		};
		TestIMGOrder(img, eo, sizeof(eo) / sizeof(const char*));
	}
}


void TestIMGMoveToInside(IMGArchive* img, IMGArchive::MoveMode mmode)
{
	IMGArchive::EntryIterator beg, end, pos;

	pos = img->getEntryByName("paths.ipl");
	beg = img->getEntryByName("hoop.dff");
	end = img->getEntryByName("german.gxt");

	img->moveEntries(pos, beg, end, mmode);

	if (mmode == IMGArchive::MoveToEnd) {
		const char* eo[] = {
				"fonts.txd", "hoop.dff", "paths.ipl", "txd.ide", "particle.txd", "fronten_pc.txd",
				"grass0_3.dff"
		};
		TestIMGOrder(img, eo, sizeof(eo) / sizeof(const char*));
	} else {
		const char* eo[] = {
				NULL, "effects.fxp", "fonts.txd", "hoop.dff", "paths.ipl", "txd.ide", "particle.txd",
				"fronten_pc.txd", "grass0_3.dff", "german.gxt", "object.dat"
		};
		TestIMGOrder(img, eo, sizeof(eo) / sizeof(const char*));
	}
}


void TestIMGMove(const File& imgFile, IMGArchive::MoveMode mmode)
{
	IMGArchive* img = IMGArchive::createArchive(imgFile);

	FillIMG(img);

	TestIMGMoveToBegin(img, mmode);

	{
		SCOPED_TRACE("Content test after the move to begin test");
		TestIMGContents(img, CreatedBaseEntries, sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry));
	}

	delete img;

	imgFile.remove();



	img = IMGArchive::createArchive(imgFile);

	FillIMG(img);

	TestIMGMoveToEnd(img, mmode);

	{
		SCOPED_TRACE("Content test after the move to end test");
		TestIMGContents(img, CreatedBaseEntries, sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry));
	}

	delete img;



	img = IMGArchive::createArchive(imgFile);

	FillIMG(img);

	TestIMGMoveToMid(img, mmode);

	{
		SCOPED_TRACE("Content test after the move to mid test");
		TestIMGContents(img, CreatedBaseEntries, sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry));
	}

	delete img;



	img = IMGArchive::createArchive(imgFile);

	FillIMG(img);

	TestIMGMoveToInside(img, mmode);

	{
		SCOPED_TRACE("Content test after the move to inside test");
		TestIMGContents(img, CreatedBaseEntries, sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry));
	}

	delete img;
}









TEST(IMGReadTest, CheckGTA3IMG)
{
	IMGArchive img(File(rootDir, "models/gta3.img"));

	IMGArchive::IMGVersion ver = img.getVersion();

	EXPECT_EQ(IMGArchive::VER2, ver);
	EXPECT_EQ(16316, img.getEntryCount());

	// Check for some hand-chosen entries...

	IMGExpectedEntry exEntries[] = {
			{"bbb_lr_slv1.dff",			5,		2048,	0x4AE89F2E},
			{"bbb_lr_slv2.dff",			3,		2048,	0xCAF0C1ED},
			{"airport_05_sfse.dff",		31,		2048,	0xA8C98B2A},
			{"land2_sfn14.dff",			6,		2048,	0x9B435A04},
			{"lodebuild12_lvs.dff",		2,		2048,	0xAC4E020C},
			{"lodassedge03.dff",		1,		2048,	0xC6CC3B03},
			{"lahills_stream4.ipl",		4,		2048,	0x345B37C6},
			{"lahills_stream3.ipl",		5,		2048,	0x7F47F2E4},
			{"laeexaminerbuild1.dff",	100,	2048,	0x4562DEAD},
			{"sex.ifp",					124,	2048,	0x97ECB763},
			{"lhand.txd",				129,	2048,	0xFED12604},
			{"veh_mods.col",			12,		2048,	0xABF280AC},
			{"fbmp_a_f.dff",			28,		2048,	0xD0284B87},
			{"des_byofficeint.dff",		37,		2048,	0xCD740D6B}
	};

	TestIMGContents(&img, exEntries, sizeof(exEntries) / sizeof(IMGExpectedEntry));
}


TEST(IMGWriteTest, CheckCreateAndFillVER1)
{
	File dirFile = File::createTemporaryFile();
	File imgFile = File::createTemporaryFile();

	IMGArchive* img = IMGArchive::createArchive(dirFile, imgFile);

	EXPECT_EQ(IMGArchive::VER1, img->getVersion());
	EXPECT_EQ(0, img->getEntryCount());

	TestIMGAddAndWrite(img);

	delete img;

	dirFile.remove();
	imgFile.remove();
}


TEST(IMGWriteTest, CheckCreateAndFillVER2)
{
	File imgFile = File::createTemporaryFile();

	IMGArchive* img = IMGArchive::createArchive(imgFile);

	EXPECT_EQ(IMGArchive::VER2, img->getVersion());
	EXPECT_EQ(0, img->getEntryCount());

	TestIMGAddAndWrite(img);

	delete img;

	imgFile.remove();
}


TEST(IMGWriteTest, CheckAddRemoveRename)
{
	File imgFile = File::createTemporaryFile();

	IMGArchive* img = IMGArchive::createArchive(imgFile);

	FillIMG(img);



	// ********** CHECK ADD **********

	IMGExpectedEntry entriesToAdd[] = {
			{"data/popcycle.dat",					IMG_BYTES2BLOCKS(127108),		127108,		0xF30F7D99},
			{"anim/ped.ifp",						IMG_BYTES2BLOCKS(1433248),		1433248,	0xD9768B37},
			{"models/hud.txd",						IMG_BYTES2BLOCKS(51624),		51624,		0x71860C43}
	};

	unsigned int numToAdd = sizeof(entriesToAdd) / sizeof(IMGExpectedEntry);

	IMGEntry** entries = new IMGEntry*[numToAdd];

	for (unsigned int i = 0 ; i < numToAdd ; i++) {
		entries[i] = new IMGEntry;
		strcpy(entries[i]->name, File(entriesToAdd[i].name.get()).getPath()->getFileName());
		entries[i]->size = entriesToAdd[i].size;
	}

	int32_t numEntriesBefore = img->getEntryCount();

	img->reserveHeaderSpace(numEntriesBefore + numToAdd);

	ASSERT_TRUE(img->addEntries(entries, numToAdd)) << "Failed to add the entries!";

	EXPECT_EQ(numEntriesBefore + numToAdd, img->getEntryCount()) << "Wrong entry count!";

	for (unsigned int i = 0 ; i < numToAdd ; i++) {
		IMGExpectedEntry& entry = entriesToAdd[i];

		iostream* stream = (iostream*) img->gotoEntry(File(entry.name.get()).getPath()->getFileName());

		EXPECT_TRUE(stream != NULL);

		if (stream) {
			File inFile(rootDir, entry.name.get());
			inFile.copyTo(stream);

			delete stream;
		}
	}

	unsigned int numFusedEntries = (sizeof(CreatedBaseEntries) + sizeof(entriesToAdd))
			/ sizeof(IMGExpectedEntry);
	IMGExpectedEntry fusedEntries[(sizeof(CreatedBaseEntries) + sizeof(entriesToAdd))
	                              / sizeof(IMGExpectedEntry)];

	unsigned int fusedI = 0;

	for (unsigned int i = 0 ; i < sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry) ; i++) {
		fusedEntries[fusedI].name = CreatedBaseEntries[i].name;
		fusedEntries[fusedI].size = CreatedBaseEntries[i].size;
		fusedEntries[fusedI].checksum = CreatedBaseEntries[i].checksum;
		fusedEntries[fusedI].checksumLen = CreatedBaseEntries[i].checksumLen;
		fusedI++;
	}

	for (unsigned int i = 0 ; i < sizeof(entriesToAdd) / sizeof(IMGExpectedEntry) ; i++) {
		fusedEntries[fusedI].name = entriesToAdd[i].name;
		fusedEntries[fusedI].size = entriesToAdd[i].size;
		fusedEntries[fusedI].checksum = entriesToAdd[i].checksum;
		fusedEntries[fusedI].checksumLen = entriesToAdd[i].checksumLen;
		fusedI++;
	}

	{
		SCOPED_TRACE("Content test after add test");
		TestIMGContents(img, fusedEntries, numFusedEntries);
	}



	// ********** CHECK REMOVE **********

	const char* entriesToRemove[] = {
			"hoop.dff", "ped.ifp", "hud.txd"
	};

	unsigned int numToRemove = sizeof(entriesToRemove) / sizeof(const char*);

	numEntriesBefore = img->getEntryCount();



	for (unsigned int i = 0 ; i < numToRemove ; i++) {
		const char* name = entriesToRemove[i];
		IMGArchive::EntryIterator it = img->getEntryByName(name);

		EXPECT_NE(img->getEntryEnd(), it) << "Entry " << name << " not found!";

		if (it != img->getEntryEnd()) {
			img->removeEntry(it);

			IMGExpectedEntry* exEntry = FindExpectedEntry(name, fusedEntries, numFusedEntries);
			if (exEntry)
				exEntry->name = CString();
		}
	}

	EXPECT_EQ(numEntriesBefore - numToRemove, img->getEntryCount()) << "Wrong entry count (removing failed)!";

	for (unsigned int i = 0 ; i < numToRemove ; i++) {
		IMGArchive::EntryIterator it = img->getEntryByName(entriesToRemove[i]);
		EXPECT_EQ(img->getEntryEnd(), it) << "Entry " << entriesToRemove[i] << " was not removed!";
	}

	{
		SCOPED_TRACE("Content test after remove test");
		TestIMGContents(img, fusedEntries, numFusedEntries);
	}



	// ********** CHECK RENAME **********

	struct RenameRequest {
		const char* before;
		const char* after;
	};

	RenameRequest renames[] = {
			{"popcycle.dat",				"fu.bar"},
			{"fonts.txd",					"HaLLO.weLt"}
	};

	unsigned int numToRename = sizeof(renames) / sizeof(RenameRequest);

	numEntriesBefore = img->getEntryCount();

	for (unsigned int i = 0 ; i < numToRename ; i++) {
		RenameRequest& ren = renames[i];

		IMGArchive::EntryIterator it = img->getEntryByName(ren.before);

		EXPECT_NE(img->getEntryEnd(), it) << "Entry to be renamed was not found: " << ren.before;

		img->renameEntry(it, ren.after);

		it = img->getEntryByName(ren.before);
		EXPECT_EQ(img->getEntryEnd(), it) << "Entry was still found under old name after renaming from "
				<< ren.before << " to " << ren.after;

		it = img->getEntryByName(ren.after);
		EXPECT_NE(img->getEntryEnd(), it) << "Entry " << ren.before << " was not successfully renamed to "
				<< ren.after;

		IMGExpectedEntry* exEntry = FindExpectedEntry(ren.before, fusedEntries, numFusedEntries);
		if (exEntry)
			exEntry->name = ren.after;
	}

	EXPECT_EQ(numEntriesBefore, img->getEntryCount()) << "Entry count changed during rename operation!";


	{
		SCOPED_TRACE("Content test after rename test");
		TestIMGContents(img, fusedEntries, numFusedEntries);
	}



	img->sync();



	delete img;

	imgFile.remove();
}


TEST(IMGWriteTest, CheckResizePack)
{
	File imgFile = File::createTemporaryFile();

	IMGArchive* img = IMGArchive::createArchive(imgFile);

	FillIMG(img);


	unsigned int numModifiedEntries = sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry);
	IMGExpectedEntry modifiedEntries[sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry)];

	CopyExpectedEntries(modifiedEntries, CreatedBaseEntries, numModifiedEntries);


	struct ResizeRequest {
		CString name;
		int32_t size;
	};

	ResizeRequest resizes[] = {
			{"fonts.txd",							300},
			{"paths.ipl",							1000},
			{"effects.fxp",							302}
	};

	unsigned int numResizes = sizeof(resizes) / sizeof(ResizeRequest);

	for (unsigned int i = 0 ; i < numResizes ; i++) {
		ResizeRequest& res = resizes[i];

		IMGArchive::EntryIterator it = img->getEntryByName(res.name.get());

		EXPECT_NE(img->getEntryEnd(), it) << "Entry #" << i << " (" << res.name.get() << ") was not found!";

		if (it != img->getEntryEnd()) {
			img->resizeEntry(it, res.size);

			IMGExpectedEntry* exEntry = FindExpectedEntry(res.name, modifiedEntries, numModifiedEntries);
			if (exEntry) {
				exEntry->size = res.size;

				if (exEntry->checksumLen > IMG_BLOCKS2BYTES(res.size)) {
					exEntry->checksumLen = 0;
				}
			}
		}
	}

	{
		SCOPED_TRACE("Content test after resize test");
		TestIMGContents(img, modifiedEntries, numModifiedEntries);
	}



	int32_t numPacked = img->getSize() - img->pack();
	RecordProperty("pack_space_gained", numPacked);

	{
		SCOPED_TRACE("Content test after packing");
		TestIMGContents(img, modifiedEntries, numModifiedEntries);
	}


	delete img;

	imgFile.remove();
}


TEST(IMGWriteTest, CheckSwap)
{
	File imgFile = File::createTemporaryFile();

	IMGArchive* img = IMGArchive::createArchive(imgFile);

	FillIMG(img);


	// ********** SWAP TEST 1 (RANGE 2 SMALLER) **********

	IMGArchive::EntryIterator r1Begin = img->getEntryByName("hoop.dff");
	IMGArchive::EntryIterator r1End = img->getEntryByName("particle.txd");
	IMGArchive::EntryIterator r2End = img->getEntryByName("grass0_3.dff");

	img->swapConsecutive(r1Begin, r1End, r2End);

	const char* expectedOrder[] = {
			"fonts.txd", "particle.txd", "fronten_pc.txd", "hoop.dff", "paths.ipl", "txd.ide", "grass0_3.dff",
			"german.gxt"
	};

	TestIMGOrder(img, expectedOrder, sizeof(expectedOrder) / sizeof(const char*));

	{
		SCOPED_TRACE("Content test after the first swap test");
		TestIMGContents(img, CreatedBaseEntries, sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry));
	}



	// ********** SWAP TEST 2 (RANGE 1 SMALLER) **********

	r1Begin = img->getEntryByName("fonts.txd");
	r1End = img->getEntryByName("paths.ipl");
	r2End = img->getEntryByName("german.gxt");

	img->swapConsecutive(r1Begin, r1End, r2End);

	const char* expectedOrder2[] = {
			"effects.fxp", "paths.ipl", "txd.ide", "grass0_3.dff", "fonts.txd", "particle.txd",
			"fronten_pc.txd", "hoop.dff", "german.gxt", "object.dat"
	};

	TestIMGOrder(img, expectedOrder2, sizeof(expectedOrder2) / sizeof(const char*));

	{
		SCOPED_TRACE("Content test after the second swap test");
		TestIMGContents(img, CreatedBaseEntries, sizeof(CreatedBaseEntries) / sizeof(IMGExpectedEntry));
	}


	delete img;

	imgFile.remove();
}


TEST(IMGWriteTest, CheckMoveToEnd)
{
	File imgFile = File::createTemporaryFile();
	TestIMGMove(imgFile, IMGArchive::MoveToEnd);
	imgFile.remove();
}


TEST(IMGWriteTest, CheckMoveSwap)
{
	File imgFile = File::createTemporaryFile();
	TestIMGMove(imgFile, IMGArchive::MoveSwap);
	imgFile.remove();
}


TEST(IMGWriteTest, CheckCombinedGTA3IMG)
{
	File imgFile(testRootDir, "gta3.img");
	File exDir(testRootDir, "extracted");


	exDir.mkdir();

	File(rootDir, "models/gta3.img").copyTo(imgFile);

	IMGArchive img(imgFile, IMGArchive::ReadWrite);


	// ********** ADD A NEW ENTRY **********

	IMGArchive::EntryIterator it = img.addEntry("FuBarHi2gA4uUU.1u7", 15);

	it = img.getEntryByName("FuBarHi2gA4uUU.1u7");
	EXPECT_NE(img.getEntryEnd(), it) << "Failed to add entry!";

	if (it != img.getEntryEnd()) {
		IMGArchive::EntryIterator pos = img.getEntryByName("hedge09_sfn_cm.dff");
		EXPECT_NE(img.getEntryEnd(), pos) << "Entry hedge09_sfn_cm.dff not found!";

		iostream* stream = (iostream*) img.gotoEntry(it);

		const char* str = "This is a dummy file added by gtaformats-test.";
		stream->write(str, strlen(str));
		stream->ignore(12*IMG_BLOCK_SIZE);
		stream->write(str, strlen(str));

		delete stream;

		img.moveEntries(pos, it, img.getEntryEnd(), IMGArchive::MoveToEnd);

		it = img.getEntryByName("FuBarHi2gA4uUU.1u7");
		EXPECT_NE(img.getEntryEnd(), it) << "Entry wasn't found after moving!";

		const char* eo1[] = {
				"hedge03sfn_cm.dff", "hedge04_sfn_cm.dff", "FuBarHi2gA4uUU.1u7"
		};

		{
			SCOPED_TRACE("Order test after moving added entry");
			TestIMGOrder(&img, eo1, sizeof(eo1) / sizeof(const char*));
		}
	}


	// ********** EXTRACT SOME ENTRIES **********

	IMGExpectedEntry extracts[] = {
			{"lodcuntw09.dff",				11,		IMG_BLOCKS2BYTES(11),		0x28F60629},
			{"cunteground43a.dff",			2,		IMG_BLOCKS2BYTES(2),		0xAA39F981},
			{"miragebuild04.dff",			2,		IMG_BLOCKS2BYTES(2),		0x4E6C0F5C},
			{"bonaplazagr_lan.dff",			8,		IMG_BLOCKS2BYTES(8),		0x867568C0},
			{"packing_carates04.dff",		6,		IMG_BLOCKS2BYTES(6),		0x289C3522},
			{"bbb_lr_slv1.dff",				5,		IMG_BLOCKS2BYTES(5),		0x095BBBD0},
			{"mall_sfse.txd",				111,	IMG_BLOCKS2BYTES(111),		0xC2E9FD59},
			{"midtownshops_sfs.txd",		112,	IMG_BLOCKS2BYTES(112),		0xDF24DF20},
			{"vgsbboardsigns01.dff",		1,		IMG_BLOCKS2BYTES(1),		0x5BE3EED5},
			{"bikdrug.txd",					65,		IMG_BLOCKS2BYTES(65),		0x082AFAAC},
			{"lahills_stream4.ipl",			4,		IMG_BLOCKS2BYTES(4),		0x0A596771},
			{"seabed7.col",					5,		IMG_BLOCKS2BYTES(5),		0x1400435A},
			{"cj_commerciax.txd",			5,		IMG_BLOCKS2BYTES(5),		0xC604DF6C},
	};

	int32_t numEntriesBefore = img.getEntryCount();

	unsigned int numExtracts = sizeof(extracts) / sizeof(IMGExpectedEntry);

	{
		SCOPED_TRACE("Content test before extraction");
		TestIMGContents(&img, extracts, numExtracts);
	}

	for (unsigned int i = 0 ; i < numExtracts ; i++) {
		IMGExpectedEntry& exEntry = extracts[i];
		IMGArchive::EntryIterator it = img.getEntryByName(exEntry.name.get());

		EXPECT_NE(img.getEntryEnd(), it) << "Extraction test entry #" << i << " (" << exEntry.name.get()
				<< ") not found!";

		if (it != img.getEntryEnd()) {
			istream* stream = img.gotoEntry(it);

			File outFile(exDir, exEntry.name.get());
			outFile.copyFrom(stream);

			img.removeEntry(it);
		}
	}

	EXPECT_EQ(numEntriesBefore - numExtracts, img.getEntryCount()) << "Wrong entry count after extraction!";


	// ********** ADD THE ENTRIES AGAIN, BUT UNDER THEIR NEW NAME **********

	for (unsigned int i = 0 ; i < numExtracts ; i++) {
		IMGExpectedEntry& exEntry = extracts[i];

		File inFile(exDir, exEntry.name.get());

		char newName[24];
		sprintf(newName, "GFT15671-test%d.418", i);

		IMGArchive::EntryIterator it = img.addEntry(newName, IMG_BYTES2BLOCKS(inFile.getSize()));

		iostream* stream = (iostream*) img.gotoEntry(it);
		inFile.copyTo(stream);
		delete stream;

		inFile.remove();
	}

	EXPECT_EQ(numEntriesBefore, img.getEntryCount()) << "Wrong entry count after adding extracted entries!";

	numEntriesBefore = img.getEntryCount();


	// ********** AND RENAME THEM TO THEIR OLD NAME **********

	for (unsigned int i = 0 ; i < numExtracts ; i++) {
		IMGExpectedEntry& exEntry = extracts[i];

		char newName[24];
		sprintf(newName, "GFT15671-test%d.418", i);

		IMGArchive::EntryIterator it = img.getEntryByName(newName);

		EXPECT_NE(img.getEntryEnd(), it) << "Renamed entry #" << i << " (" << newName << ")"
				<< " could not be found!";

		if (it != img.getEntryEnd()) {
			img.renameEntry(it, exEntry.name.get());
		}
	}

	{
		SCOPED_TRACE("Content test after extraction and renaming");
		TestIMGContents(&img, extracts, numExtracts);
	}


	// ********** SWAP SOME ENTRIES **********

	struct SwapRequest {
		const char* r1BeginName;
		const char* r1EndName;
		const char* r2EndName;
		const char* expectedOrder1[20];
		const char* expectedOrder2[20];
		const char* expectedOrder3[20];
	};

	SwapRequest swapRequests[] = {
			{	"des_wgarage.txd", "countryn_3.col", "countryn_8.col",
				{	"des_wdam.txd", "countryn_3.col", "countryn_4.col", "countryn_5.col", "countryn_6.col",
					"countryn_7.col", "des_wgarage.txd", "des_wires.txd", "des_woodbr.txd",
					"des_wtownmain.txd", "lod_countryn.txd", "mp_ranchcut.txd", "countryn_1.col",
					"countryn_2.col", "countryn_8.col", "countryn_9.col", NULL
				},
				NULL,
				NULL
			},

			{	"hotelbits_sfe02.dff", "vgnntrainfence05b.dff", "swfotr1.txd",
				{	"grnwhite_sfe.dff", "hotelbits_sfe01.dff", "vgnntrainfence05b.dff", "vgnorthcoast02.dff",
					"vgnorthcoast02b.dff", "vgnorthcoast03.dff", NULL
				},
				{	"swfori.txd", "swfost.txd", "hotelbits_sfe02.dff", "hotelbits_sfe03.dff",
					"hotelbits_sfe04.dff", "hotelbits_sfe05.dff", NULL
				},
				{	"vgnntrainfence04b.dff", "vgnntrainfence05.dff", "swfotr1.txd", "swfyri.txd",
					"swfyst.txd", NULL
				}
			}
	};

	unsigned int numSwapRequests = sizeof(swapRequests) / sizeof(SwapRequest);

	for (unsigned int i = 0 ; i < numSwapRequests ; i++) {
		SwapRequest& swap = swapRequests[i];

		IMGArchive::EntryIterator r1Begin = img.getEntryByName(swap.r1BeginName);
		IMGArchive::EntryIterator r1End = img.getEntryByName(swap.r1EndName);
		IMGArchive::EntryIterator r2End = img.getEntryByName(swap.r2EndName);

		EXPECT_NE(img.getEntryEnd(), r1Begin) << "Range 1 begin entry for swap request #" << i << " ("
				<< swap.r1BeginName << ") was not found!";
		EXPECT_NE(img.getEntryEnd(), r1End) << "Range 1 end entry for swap request #" << i << " ("
				<< swap.r1EndName << ") was not found!";
		EXPECT_NE(img.getEntryEnd(), r2End) << "Range 2 end entry for swap request #" << i << " ("
				<< swap.r2EndName << ") was not found!";

		ASSERT_NO_THROW({
			img.swapConsecutive(r1Begin, r1End, r2End);
		}) << "Exception thrown during swap test #" << i << "!";

		if (swap.expectedOrder1) {
			char traceMsg[64];
			sprintf(traceMsg, "Order test 1 in swap test #%d", i);
			SCOPED_TRACE(traceMsg);

			unsigned int len;
			for (len = 0 ; swap.expectedOrder1[len] ; len++);

			TestIMGOrder(&img, swap.expectedOrder1, len);
		}
		if (swap.expectedOrder2) {
			char traceMsg[64];
			sprintf(traceMsg, "Order test 2 in swap test #%d", i);
			SCOPED_TRACE(traceMsg);

			unsigned int len;
			for (len = 0 ; swap.expectedOrder2[len] ; len++);

			TestIMGOrder(&img, swap.expectedOrder2, len);
		}
		if (swap.expectedOrder3) {
			char traceMsg[64];
			sprintf(traceMsg, "Order test 3 in swap test #%d", i);
			SCOPED_TRACE(traceMsg);

			unsigned int len;
			for (len = 0 ; swap.expectedOrder3[len] ; len++);

			TestIMGOrder(&img, swap.expectedOrder3, len);
		}
	}

	EXPECT_EQ(numEntriesBefore, img.getEntryCount()) << "Entry count changed during swap test!";

	{
		SCOPED_TRACE("Content test after swap test");
		TestIMGContents(&img, extracts, numExtracts);
	}


	// ********** MOVE SOME ENTRIES **********

	struct MoveRequest {
		const char* posName;
		const char* begName;
		const char* endName;
		IMGArchive::MoveMode mmode;
		const char* expectedOrder1[20];
		const char* expectedOrder2[20];
		const char* expectedOrder3[20];
	};

	MoveRequest moves[] = {
			{	"psycho.txd", "bonaplazagr_lan.dff", "bikdrug.txd", IMGArchive::MoveToEnd,
				{	"player.txd", "poolguy.txd", "bonaplazagr_lan.dff", "packing_carates04.dff",
					"bbb_lr_slv1.dff", "mall_sfse.txd", "midtownshops_sfs.txd", "vgsbboardsigns01.dff",
					NULL
				},
				NULL,
				NULL
			},

			{	"elmdead_hi.dff", "wheel_lr3.dff", "des_miscbits.txd", IMGArchive::MoveToEnd,
				{	"cedar2_hi.dff", "cedar3_hi.dff", "wheel_lr3.dff", "wheel_lr4.dff", "wheel_lr5.dff",
					NULL
				},
				{	"des_clifftown.txd", "des_dinerw.txd", "des_farmstuff.txd", NULL
				},
				NULL
			},

			{	"lod_sfs037.dff", "crates.dff", "burbdoor.dff", IMGArchive::MoveSwap,
				{	"counts_lights01.dff", "count_ammundoor.dff", "burbdoor.dff", "burbdoor2.dff",
					"busdepot_lod.dff", NULL
				},
				{	"lod_sfs035.dff", "lod_sfs036.dff", "crates.dff", "crates01.dff", "cstwnland03.dff",
					"cstwnland_lod.dff", NULL
				},
				{	"countrys_stream4.ipl", "bigsprunkpole.dff", "lod_sfs037.dff", "lod_sfs047.dff",
					"lod_sfs049.dff", NULL
				}
			},

			// The following is a full archive swap operation!
			{	"bbb_lr_slv2.dff", "lodstrd2_las2.dff", NULL, IMGArchive::MoveSwap,
				{	NULL, "lodstrd2_las2.dff", "lodstrd3_las2.dff", "lodstripbar02.dff", NULL
				},
				{	"bbb_lr_slv2.dff", "bntl_b_ov.dff", "bntl_b_sq.dff", NULL
				},
				{	"lodstormbrid_las2.dff", "lodstpshp1a_las2.dff", NULL
				}
			}
	};

	unsigned int numMoves = sizeof(moves) / sizeof(MoveRequest);

	for (unsigned int i = 0 ; i < numMoves ; i++) {
		MoveRequest& move = moves[i];

		IMGArchive::EntryIterator pos = img.getEntryByName(move.posName);
		IMGArchive::EntryIterator beg = img.getEntryByName(move.begName);

		EXPECT_NE(img.getEntryEnd(), pos) << "Insertion position iterator for move request #" << i << "("
				<< move.posName << ") was not found!";
		EXPECT_NE(img.getEntryEnd(), beg) << "Begin iterator for move request #" << i << "("
				<< move.begName << ") was not found!";

		IMGArchive::EntryIterator end;

		if (move.endName) {
			end = img.getEntryByName(move.endName);
			EXPECT_NE(img.getEntryEnd(), end) << "End iterator for move request #" << i << "("
					<< move.endName << ") was not found!";
		} else {
			end = img.getEntryEnd();
		}

		ASSERT_NO_THROW({
			img.moveEntries(pos, beg, end, move.mmode);
		}) << "Exception thrown during move test #" << i << "!";

		if (move.expectedOrder1) {
			char traceMsg[64];
			sprintf(traceMsg, "Order test 1 in move test #%d", i);
			SCOPED_TRACE(traceMsg);

			unsigned int len;
			for (len = 0 ; move.expectedOrder1[len] ; len++);

			TestIMGOrder(&img, move.expectedOrder1, len);
		}
		if (move.expectedOrder2) {
			char traceMsg[64];
			sprintf(traceMsg, "Order test 2 in move test #%d", i);
			SCOPED_TRACE(traceMsg);

			unsigned int len;
			for (len = 0 ; move.expectedOrder2[len] ; len++);

			TestIMGOrder(&img, move.expectedOrder2, len);
		}
		if (move.expectedOrder3) {
			char traceMsg[64];
			sprintf(traceMsg, "Order test 3 in move test #%d", i);
			SCOPED_TRACE(traceMsg);

			unsigned int len;
			for (len = 0 ; move.expectedOrder3[len] ; len++);

			TestIMGOrder(&img, move.expectedOrder3, len);
		}
	}

	EXPECT_EQ(numEntriesBefore, img.getEntryCount()) << "Entry count changed during move test!";

	{
		SCOPED_TRACE("Content test after move test");
		TestIMGContents(&img, extracts, numExtracts);
	}

	exDir.remove();


	img.sync();
}














