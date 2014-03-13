/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDConverter.h>
#include <gtaformats/txd/TXDException.h>
#include <gtaformats/txd/TXDTextureHeader.h>
#include <nxcommon/CRC32.h>
#include <nxcommon/image.h>
#include <png.h>



void pngWriteCallback(png_structp png, png_bytep buffer, png_size_t size)
{
	ostream* out = (ostream*) png_get_io_ptr(png);
	out->write((char*) buffer, size);
}


void pngFlushCallback(png_structp png)
{
	ostream* out = (ostream*) png_get_io_ptr(png);
	out->flush();
}



struct TXDExpectedTexture
{
	CString diffuseName;
	CString alphaName;
	int32_t rasterFormat;
	TXDCompression compression;
	int16_t width;
	int16_t height;
	int8_t bpp;
	int8_t mipmapCount;
	bool hasAlpha;
	int8_t uWrapFlags;
	int8_t vWrapFlags;
	int16_t filterFlags;
	int64_t dataChecksum;
};



void WritePNG(uint8_t* data, int16_t w, int16_t h, ostream* out)
{
	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
			NULL);
	if (!png) {
		exit(10);
	}

	png_infop info = png_create_info_struct(png);
	if (!info) {
		png_destroy_write_struct(&png, (png_infopp) NULL);
		exit(11);
	}

	if (setjmp(png_jmpbuf(png))) {
		png_destroy_write_struct(&png, (png_infopp) info);
		exit(12);
	}

	png_set_write_fn(png, (void*) out, pngWriteCallback, pngFlushCallback);

	png_set_IHDR(png, info, w, h, 8, PNG_COLOR_TYPE_RGB_ALPHA,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	const char* softwareKey = "Software";
	const char* softwareText = "gtaformats-test using gtatools " GTATOOLS_VERSION;

	const char* commentKey = "Comment";
	const char* commentText = "Automatically converted from a GTA TXD texture.";

	char* softwareKeyCpy = new char[strlen(softwareKey)+1];
	char* softwareTextCpy = new char[strlen(softwareText)+1];
	strcpy(softwareKeyCpy, softwareKey);
	strcpy(softwareTextCpy, softwareText);
	png_text software;
	software.key = softwareKeyCpy;
	software.text = softwareTextCpy;
	software.text_length = strlen(softwareTextCpy);
	software.compression = PNG_TEXT_COMPRESSION_NONE;

	char* commentKeyCpy = new char[strlen(commentKey)+1];
	char* commentTextCpy = new char[strlen(commentText)+1];
	strcpy(commentKeyCpy, commentKey);
	strcpy(commentTextCpy, commentText);
	png_text comment;
	comment.key = commentKeyCpy;
	comment.text = commentTextCpy;
	comment.text_length = strlen(commentTextCpy);
	comment.compression = PNG_TEXT_COMPRESSION_NONE;

	png_text texts[] = {software, comment};

	png_set_text(png, info, texts, sizeof(texts)/sizeof(texts[0]));

	uint8_t** rowData = new uint8_t*[h];

	for (int k = 0 ; k < h ; k++) {
		rowData[k] = data + k*w*4;
	}

	png_set_rows(png, info, rowData);
	png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

	png_destroy_write_struct(&png, &info);

	delete[] rowData;

	delete[] softwareKeyCpy;
	delete[] softwareTextCpy;
	delete[] commentKeyCpy;
	delete[] commentTextCpy;
}


void TestTXDContents(TXDArchive* txd, TXDExpectedTexture* exTexes, unsigned int numExTexes)
{
	vector<TXDExpectedTexture*> vecExTexes;

	for (unsigned int i = 0 ; i < numExTexes ; i++) {
		vecExTexes.push_back(exTexes+i);
	}

	CRC32 crc;

	for (TXDArchive::TextureIterator it = txd->getHeaderBegin() ; it != txd->getHeaderEnd() ; it++) {
		TXDTextureHeader* tex = *it;

		vector<TXDExpectedTexture*>::iterator eit;
		for (eit = vecExTexes.begin() ; eit != vecExTexes.end() ; eit++) {
			TXDExpectedTexture* exTex = *eit;

			if (CString(tex->getDiffuseName()) == exTex->diffuseName) {
				EXPECT_EQ(exTex->alphaName, CString(tex->getAlphaName())) << "Alpha name of texture "
						<< exTex->diffuseName.get() << " does not match!";
				EXPECT_EQ(exTex->rasterFormat, tex->getFullRasterFormat()) << "Raster format of texture "
						<< exTex->diffuseName.get() << " does not match!";
				EXPECT_EQ(exTex->compression, tex->getCompression()) << "Compression of texture "
						<< exTex->diffuseName.get() << " does not match!";
				EXPECT_EQ(exTex->width, tex->getWidth()) << "Width of texture " << exTex->diffuseName.get()
						<< " does not match!";
				EXPECT_EQ(exTex->height, tex->getHeight()) << "Height of texture "
						<< exTex->diffuseName.get() << " does not match!";
				EXPECT_EQ(exTex->bpp, tex->getBytesPerPixel()) << "BPP of texture "
						<< exTex->diffuseName.get() << " does not match!";
				EXPECT_EQ(exTex->mipmapCount, tex->getMipmapCount()) << "Mipmap count of texture "
						<< exTex->diffuseName.get() << " does not match!";
				EXPECT_EQ(exTex->hasAlpha, tex->isAlphaChannelUsed()) << "Alpha flag of texture "
						<< exTex->diffuseName.get() << " does not match!";
				EXPECT_EQ(exTex->uWrapFlags, tex->getUWrapFlags()) << "U Wrap flags of texture "
						<< exTex->diffuseName.get() << " do not match!";
				EXPECT_EQ(exTex->vWrapFlags, tex->getVWrapFlags()) << "V Wrap flags of texture "
						<< exTex->diffuseName.get() << " do not match!";
				EXPECT_EQ(exTex->filterFlags, tex->getFilterFlags()) << "Filter flags of texture "
						<< exTex->diffuseName.get() << " do not match!";

				if (exTex->dataChecksum != -1) {
					uint8_t* data = txd->getTextureData(tex);
					int dsize = tex->computeDataSize();
					crc.append((char*) data, dsize);
					delete[] data;

					EXPECT_EQ(exTex->dataChecksum, crc.getChecksum()) << "Wrong data checksum for texture "
							<< exTex->diffuseName.get() << "!";

					crc.clear();
				}

				vecExTexes.erase(eit);

				break;
			}
		}
	}

	for (vector<TXDExpectedTexture*>::iterator it = vecExTexes.begin() ; it != vecExTexes.end() ; it++) {
		TXDExpectedTexture* tex = *it;
		ADD_FAILURE() << "Texture " << tex->diffuseName.get() << " was not found!";
	}
}


void TestTXDConversion(TXDTextureHeader* tex, uint8_t* rawData, const File& convDir)
{
	File convDirBefore(convDir, "before");
	File convDirAfter(convDir, "after");

	if (!convDirBefore.exists()) {
		if (!convDirBefore.mkdirs()) {
			ADD_FAILURE() << "Unable to create TXD conversion directory "
					<< convDirBefore.getPath().toString().get();
			return;
		}
	}
	if (!convDirAfter.exists()) {
		if (!convDirAfter.mkdirs()) {
			ADD_FAILURE() << "Unable to create TXD conversion directory "
					<< convDirAfter.getPath().toString().get();
			return;
		}
	}

	TXDConverter conv;

	// ********** Convert to R8G8B8A8 **********

	TXDTextureHeader rgba8888Tex(*tex);
	rgba8888Tex.setRasterFormat(RasterFormatR8G8B8A8);

	uint8_t* rgba8888Data = conv.convert(*tex, rgba8888Tex, rawData, 0, -1);

	char fname[128];
	sprintf(fname, "%s.png", tex->getDiffuseName().get());
	File outFile(convDirBefore, fname);

	ostream* out = outFile.openOutputStream(ostream::binary);

	WritePNG(rgba8888Data, tex->getWidth(), tex->getHeight(), out);

	delete out;



	// ********** Convert to R4G4B4A4 uncompressed **********

	TXDTextureHeader rgba4444Tex(*tex);
	rgba4444Tex.setRasterFormat(RasterFormatR4G4B4A4);

	uint8_t* rgba4444Data = conv.convert(*tex, rgba4444Tex, rawData, 0, -1);



	// ********** Now convert to R4G4B4A4 DXT3 **********

	TXDTextureHeader dxt3Tex(*tex);
	dxt3Tex.setRasterFormat(RasterFormatR4G4B4A4, DXT3);

	uint8_t* dxt3Data = conv.convert(rgba4444Tex, dxt3Tex, rgba4444Data, 0, -1);

	delete[] rgba4444Data;



	// ********** Convert back to R8G8B8A8 **********

	uint8_t* convRgba8888Data = conv.convert(dxt3Tex, rgba8888Tex, dxt3Data, 0, -1);

	delete[] dxt3Data;

	bool equal;
	float psnr = ComputePeakSignalToNoiseRatio(equal, rgba8888Data, convRgba8888Data, tex->getWidth(),
			tex->getHeight(), true);

	if (!equal) {
		EXPECT_GT(psnr, 25.0f) << "PSNR is too low for texture " << tex->getDiffuseName().get() << ". This "
				<< "might mean the texture conversion failed (or has very bad quality).";
	}

	sprintf(fname, "conv_%s.png", tex->getDiffuseName().get());
	outFile = File(convDirAfter, fname);

	out = outFile.openOutputStream(ostream::binary);

	WritePNG(convRgba8888Data, tex->getWidth(), tex->getHeight(), out);

	delete out;

	delete[] convRgba8888Data;

	delete[] rgba8888Data;
}


TEST(TXDReadTest, CheckOriginalTXDsSA)
{
	TXDExpectedTexture hudTexes[] = {
			{"radardisc", "", 0x00000300, DXT3, 64, 64, 2, 1, true, 0x00000000, 0x00000000, 0x00001102, 0x50383B09},
			{"skipicon", "", 0x00000300, DXT3, 128, 128, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xE61465F3},
			{"siterocket", "", 0x00000100, DXT1, 32, 32, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xD48AA56E},
			{"siteM16", "", 0x00000300, DXT3, 64, 64, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xB4B3D76F},
			{"radar_ZERO", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x0BE93DEF},
			{"radar_WOOZIE", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x0F3AF575},
			{"radar_waypoint", "", 0x00000300, DXT3, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x560428A9},
			{"radar_tshirt", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xAF6C012E},
			{"radar_truck", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x50BD3355},
			{"radar_triadsCasino", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x2B8AF58C},
			{"radar_triads", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x0FAA2DCC},
			{"radar_TorenoRanch", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x1BBB1976},
			{"radar_TORENO", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x2C57BD89},
			{"radar_THETRUTH", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xCCF06D6B},
			{"radar_tattoo", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x0CA3C537},
			{"radar_SWEET", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xB869B9D7},
			{"radar_spray", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xEDCDA29E},
			{"radar_school", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xC6EDBC94},
			{"radar_saveGame", "", 0x00000200, DXT1, 16, 16, 2, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xEA59E00A},
			{"radar_RYDER", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xE5DECC1F},
			{"radar_runway", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x199B0332},
			{"radar_race", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x593E6E29},
			{"radar_qmark", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x13853FDB},
			{"radar_propertyR", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xBD6BE494},
			{"radar_propertyG", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xB5D9BD7C},
			{"radar_police", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x35C3BB5E},
			{"radar_pizza", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xF79234DE},
			{"radar_OGLOC", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xDD99DC4F},
			{"radar_north", "", 0x00000300, DXT3, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xAEE94E91},
			{"radar_modGarage", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xFC86811E},
			{"radar_MCSTRAP", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xFC52B8FE},
			{"radar_mafiaCasino", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xD4004977},
			{"radar_MADDOG", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x65F31863},
			{"radar_LocoSyndicate", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x3BA8DB7D},
			{"radar_light", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x3B72C9E0},
			{"radar_impound", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xBE15940E},
			{"radar_hostpital", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x544600CC},
			{"radar_gym", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x217926FC},
			{"radar_girlfriend", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xA8C68904},
			{"radar_gangY", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x9BC2F3CA},
			{"radar_gangP", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x832890F2},
			{"radar_gangN", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x55E83090},
			{"radar_gangG", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x4A8B69AD},
			{"radar_gangB", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x0E19810F},
			{"radar_Flag", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x315FE615},
			{"radar_fire", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xF686C071},
			{"radar_enemyAttack", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x970A2BCB},
			{"radar_emmetGun", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xFCE153AE},
			{"radar_diner", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x076068B4},
			{"radar_dateFood", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x92B8BD7B},
			{"radar_dateDrink", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x98EA823A},
			{"radar_dateDisco", "", 0x00000300, DXT3, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x120394B3},
			{"radar_CRASH1", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xCF20044C},
			{"radar_CJ", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xC5F15EA6},
			{"radar_chicken", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x8EEE6557},
			{"radar_CESARVIAPANDO", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x14EF5C0D},
			{"radar_centre", "", 0x00000300, DXT3, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xD538904A},
			{"radar_CATALINAPINK", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xEE0EBEE0},
			{"radar_cash", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xF69E7F33},
			{"radar_burgerShot", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x015AB4AA},
			{"radar_bulldozer", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x3ABBA8B6},
			{"radar_boatyard", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x757A9A09},
			{"radar_BIGSMOKE", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xC5858737},
			{"radar_barbers", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x8F4612E3},
			{"radar_ammugun", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x9BA12012},
			{"radar_airYard", "", 0x00000100, DXT1, 16, 16, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x0BD27EEF},
			{"radarRingPlane", "", 0x00000300, DXT3, 64, 64, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xD4F51C86},
			{"fist", "", 0x00000300, DXT3, 64, 64, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x6CFB8C34},
			{"arrow", "", 0x00000300, DXT3, 32, 32, 2, 1, true, 0x00000000, 0x00000000, 0x00001101, 0x673FC372}
	};

	TXDExpectedTexture bistroTexes[] = {
			{"vent_64", "", 0x00000600, NONE, 64, 64, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xBC84C8EB},
			{"Tablecloth", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x04F5DC3F},
			{"sw_wallbrick_01", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xE34352D3},
			{"sw_door11", "", 0x00000600, NONE, 64, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xD1548922},
			{"StainedGlass", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x636B630C},
			{"SaltNPeppa", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x3ACAE6E6},
			{"rest_wall4", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x3B03DDE0},
			{"Plate", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x864341C0},
			{"Panel", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x6A5513C4},
			{"mp_snow", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x0850CDD8},
			{"mottled_grey_64HV", "", 0x00000600, NONE, 64, 64, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x88868044},
			{"marblekb_256128", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x0A61BEE5},
			{"Marble2", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xE51756E3},
			{"Marble", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xE35D68EE},
			{"DinerFloor", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x707E6ECC},
			{"deco_chair_1", "", 0x00000600, NONE, 256, 256, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x71A0C5B7},
			{"Cutlery", "", 0x00000500, NONE, 128, 128, 4, 1, true, 0x00000000, 0x00000000, 0x00001101, 0xF28A8A82},
			{"concretebig3_256", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x8893A340},
			{"Coffemachine", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x0A70B893},
			{"Candle_Holder", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xBB6EE547},
			{"Cakes", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x918B92B6},
			{"Cabinet", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xEF2371A4},
			{"Bow_Abattoir_Conc2", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xED25E5BB},
			{"BistroMenu", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xC862BF65},
			{"barbersflr1_LA", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0x24DDC6C2},
			{"ahoodfence2", "", 0x00000600, NONE, 128, 128, 4, 1, false, 0x00000000, 0x00000000, 0x00001101, 0xD6B9CEAA}
	};

	struct TXDTest {
		CString txdPath;
		TXDExpectedTexture* exTexes;
		unsigned int numExTexes;
	};

	TXDTest tests[] = {
			{"models/hud.txd", hudTexes, sizeof(hudTexes) / sizeof(TXDExpectedTexture)},
			{"models/gta3.img/bistro.txd", bistroTexes, sizeof(bistroTexes) / sizeof(TXDExpectedTexture)}
	};


	for (unsigned int i = 0 ; i < sizeof(tests) / sizeof(TXDTest) ; i++) {
		TXDTest& test = tests[i];

		TXDArchive txd(File(gtasaRoot, test.txdPath.get()));

		{
			char traceMsg[1024];
			sprintf(traceMsg, "TXD archive test #%d (%s)", i, test.txdPath.get());
			SCOPED_TRACE(traceMsg);

			TestTXDContents(&txd, test.exTexes, test.numExTexes);
		}
	}
}


TEST(TXDReadTest, CheckOriginalTXDsVC)
{
	TXDExpectedTexture miscTexes[] = {
			{"wheel_sport64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x99E52938},
			{"wheel_saloon64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x2A37A70F},
			{"wheel_classic64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x1D7AAB7D},
			{"wheel_lighttruck64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x48D4C477},
			{"wheel_lightvan64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x531824EF},
			{"wheel_alloy64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x7ECB868D},
			{"whee_rim64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x8FD23871},
			{"wheel_truck64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x417750BA},
			{"wheel_offroad64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0xF7FA6EF1},
			{"tyre64a", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x422997D8},
			{"wheel_smallcar64", "", 0x00000200, DXT1, 64, 64, 2, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x51D9A607}
	};

	TXDExpectedTexture portabarrierTexes[] = {
			{"Stop2_64", "Stop2_64a", 0x00002500, NONE, 64, 64, 1, 1, true, 0x00000000, 0x00000000, 0x00001106, 0xF5EE32C2},
			{"banding_blue_64HV", "", 0x00002600, NONE, 64, 64, 1, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x74FD4B5F},
			{"banding8_64HV", "", 0x00002600, NONE, 64, 64, 1, 1, false, 0x00000000, 0x00000000, 0x00001106, 0xADCFD06D},
			{"cratetop128", "", 0x00002600, NONE, 128, 128, 1, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x098AE6C2},
			{"glass_64a", "glass2_64alpha", 0x00002500, NONE, 64, 64, 1, 1, true, 0x00000000, 0x00000000, 0x00001106, 0xCDDC8999},
			{"bareboards_64a", "", 0x00002600, NONE, 64, 64, 1, 1, false, 0x00000000, 0x00000000, 0x00001106, 0xBE3E9AF1},
			{"redband_64Ha", "", 0x00002600, NONE, 64, 64, 1, 1, false, 0x00000000, 0x00000000, 0x00001106, 0x6054D3BA},
			{"chevron_red_64HVa", "", 0x00002600, NONE, 64, 64, 1, 1, false, 0x00000000, 0x00000000, 0x00001106, 0xE83231D0}
	};

	struct TXDTest {
		CString txdPath;
		TXDExpectedTexture* exTexes;
		unsigned int numExTexes;
	};

	TXDTest tests[] = {
			{"models/MISC.TXD", miscTexes, sizeof(miscTexes) / sizeof(TXDExpectedTexture)},
			{"models/gta3.img/portabarrier.txd", portabarrierTexes, sizeof(portabarrierTexes) / sizeof (TXDExpectedTexture)}
	};

	for (unsigned int i = 0 ; i < sizeof(tests) / sizeof(TXDTest) ; i++) {
		TXDTest& test = tests[i];

		TXDArchive txd(File(gtavcRoot, test.txdPath.get()));

		{
			char traceMsg[1024];
			sprintf(traceMsg, "TXD archive test #%d (%s)", i, test.txdPath.get());
			SCOPED_TRACE(traceMsg);

			TestTXDContents(&txd, test.exTexes, test.numExTexes);
		}
	}
}


TEST(TXDConversionTest, CheckOriginalTXDConversion)
{
	struct ConversionTest {
		GTATestVersion ver;
		CString relPath;
		CString texes[20];
	};

	ConversionTest tests[] = {
			{	TestGTASA, "models/misc.txd",
				{"whee_rim64", "wheel_offroad64", "tyre64a", NULL}
			},
			{	TestGTASA, "models/particle.txd",
				{"wincrack_32", "white", "target256", "cloudhigh", "shad_exp", NULL}
			},
			{	TestGTASA, "models/gta3.img/bistro.txd",
				{"Tablecloth", "vent_64", "Cutlery", "Coffemachine", "Candle_Holder", "Plate", NULL}
			},
			{	TestGTAVC, "models/gta3.img/portabarrier.txd",
				{"Stop2_64", "banding_blue_64HV", "cratetop128", "glass_64a", NULL}
			}
	};

	for (unsigned int i = 0 ; i < sizeof(tests) / sizeof(ConversionTest) ; i++) {
		ConversionTest& test = tests[i];

		File convDir(testRootDir, File(test.relPath.get()).getPath().getFileName());

		File txdFile("");

		if (test.ver == TestGTASA) {
			if (!gtasaEnabled) {
				continue;
			}

			txdFile = File(gtasaRoot, test.relPath.get());
		} else if (test.ver == TestGTAVC) {
			if (!gtavcEnabled) {
				continue;
			}

			txdFile = File(gtavcRoot, test.relPath.get());
		} else {
			if (!gta3Enabled) {
				continue;
			}

			txdFile = File(gta3Root, test.relPath.get());
		}

		TXDArchive txd(txdFile);

		vector<CString> texVec;

		for (unsigned int j = 0 ; test.texes[j].get() != NULL ; j++) {
			texVec.push_back(test.texes[j]);
		}

		for (TXDArchive::TextureIterator it = txd.getHeaderBegin() ; it != txd.getHeaderEnd() ; it++) {
			TXDTextureHeader* tex = *it;

			for (vector<CString>::iterator tit = texVec.begin() ; tit != texVec.end() ; tit++) {
				CString exName = *tit;

				if (exName == CString(tex->getDiffuseName())) {
					texVec.erase(tit);

					uint8_t* rawData = txd.getTextureData(tex);

					TestTXDConversion(tex, rawData, convDir);

					delete[] rawData;

					break;
				}
			}
		}

		for (vector<CString>::iterator tit = texVec.begin() ; tit != texVec.end() ; tit++) {
			CString exName = *tit;
			ADD_FAILURE() << "No texture of name " << exName.get() << " was found in TXD archive "
					<< txdFile.getPath().toString().get();
		}
	}
}


TEST(TXDWriteTest, CheckTXDWrite)
{
	TXDArchive txd;

	EXPECT_EQ(0, txd.getTextureCount());

	TXDTextureHeader* tex1 = new TXDTextureHeader("JustATexture", RasterFormatB8G8R8A8, NONE, 128, 128);
	tex1->setFilterFlags(0xDEAD);
	tex1->setWrappingFlags(0xF0, 0xBA);

	uint8_t data1[128*128*4];

	for (int i = 0 ; i < 128 ; i++) {
		for (int j = 0 ; j < 128 ; j++) {
			if ((i+j) % 2 == 0) {
				data1[(i*128+j)*4] = 0xFF;
				data1[(i*128+j)*4 + 1] = 0x00;
				data1[(i*128+j)*4 + 2] = 0x00;
				data1[(i*128+j)*4 + 3] = 0xFF;
			} else {
				data1[(i*128+j)*4] = 0x00;
				data1[(i*128+j)*4 + 1] = 0x00;
				data1[(i*128+j)*4 + 2] = 0xFF;
				data1[(i*128+j)*4 + 3] = 0xFF;
			}
		}
	}

	txd.addTexture(tex1, data1);

	EXPECT_EQ(1, txd.getTextureCount());

	File txdFile = File::createTemporaryFile();
	txd.write(txdFile);

	TXDArchive txd2(txdFile);

	EXPECT_EQ(1, txd2.getTextureCount());

	if (txd2.getTextureCount() == 1) {
		TXDArchive::TextureIterator it = txd2.getHeaderBegin();
		TXDTextureHeader* tex = *it;

		EXPECT_STREQ("JustATexture", tex->getDiffuseName().get());
		EXPECT_STREQ("", tex->getAlphaName().get());
		EXPECT_EQ(4, tex->getBytesPerPixel());
		EXPECT_EQ(NONE, tex->getCompression());
		EXPECT_EQ((int16_t) 0xDEAD, tex->getFilterFlags());
		EXPECT_EQ((int8_t) 0xF0, tex->getUWrapFlags());
		EXPECT_EQ((int8_t) 0xBA, tex->getVWrapFlags());

		tex->setRasterFormat(RasterFormatA1R5G5B5, DXT1);
		txd2.applyTextureHeader(tex);

		EXPECT_EQ(2, tex->getBytesPerPixel());
		EXPECT_EQ(RasterFormatA1R5G5B5, tex->getFullRasterFormat());
	}

	txdFile.remove();
}





