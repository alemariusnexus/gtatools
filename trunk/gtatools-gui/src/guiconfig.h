/*
 * config.h
 *
 *  Created on: 24.01.2010
 *      Author: alemariusnexus
 */

#ifndef GUICONFIG_H_
#define GUICONFIG_H_

#include <IL/il.h>
#include "lang/lang.h"
#include <cstring>
#include <wx/wx.h>

struct ImageFileFormat {
	char extension[8];
	wxString description;
	ILenum format;
};

struct IdImageFileFormat {
	char extension[8];
	int descriptionId;
	ILenum format;
};

static const IdImageFileFormat SupportedImageFormats[] = {
		{"png", ImageFormat_PNG_description, IL_PNG},
		{"tga", ImageFormat_TGA_description, IL_TGA},
		{"jpeg", ImageFormat_JPEG_description, IL_PNG},
		{"bmp", ImageFormat_BMP_description, IL_BMP},
		{"gif", ImageFormat_GIF_description, IL_GIF},
		{"tif", ImageFormat_TIF_description, IL_TIF},
		{"h", ImageFormat_CHEAD_description, IL_CHEAD},
		{"psd", ImageFormat_PSD_description, IL_PSD},
		{"sgi", ImageFormat_SGI_description, IL_SGI}
};


inline int GetSupportedImageFormatCount()
{
	return sizeof(SupportedImageFormats)/sizeof(IdImageFileFormat);
}

inline int GetSupportedImageFormats(ImageFileFormat** formats)
{
	int numFormats = GetSupportedImageFormatCount();

	for (int i = 0 ; i < numFormats ; i++) {
		formats[i] = new ImageFileFormat;

		strcpy(formats[i]->extension, SupportedImageFormats[i].extension);
		//strcpy(formats[i]->description, LangGet(SupportedImageFormats[i].descriptionId));
		formats[i]->description = LangGet(SupportedImageFormats[i].descriptionId);
		formats[i]->format = SupportedImageFormats[i].format;
	}

	return numFormats;
}


#endif /* GUICONFIG_H_ */
