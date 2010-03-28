/*
	Copyright 2010 David Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GUICONFIG_H_
#define GUICONFIG_H_

#define GGUI_VERSION_STRING "0.1a"



#include <wx/image.h>
#include "lang/lang.h"
#include <cstring>
#include <wx/wx.h>

struct ImageFileFormat {
	char extension[8];
	wxString description;
	int format;
};

struct IdImageFileFormat {
	char extension[8];
	const char* descriptionId;
	int format;
};

static const IdImageFileFormat SupportedImageFormats[] = {

#	if wxUSE_LIBPNG
		{"png", "ImageFormat_PNG_description", wxBITMAP_TYPE_PNG},
#	endif

#	if wxUSE_LIBJPEG
		{"jpeg", "ImageFormat_JPEG_description", wxBITMAP_TYPE_JPEG},
#	endif

#	if wxUSE_PCX
		{"pcx", "ImageFormat_PCX_description", wxBITMAP_TYPE_PCX},
#	endif

#	if wxUSE_PNM
		{"pnm", "ImageFormat_PNM_description", wxBITMAP_TYPE_PNM},
#	endif

#	if wxUSE_LIBTIFF
		// wxWidgets' documentation says it's wxBITMAP_TYPE_TIFF, but for me it seems to be
		// wxBITMAP_TYPE_TIF. We'll be sure and support both.
#		ifdef wxBITMAP_TYPE_TIFF
			{"tiff", "ImageFormat_TIFF_description", wxBITMAP_TYPE_TIFF},
#		else
			{"tiff", "ImageFormat_TIFF_description", wxBITMAP_TYPE_TIF},
#		endif
#	endif

#	if wxUSE_ICO
		{"ico", "ImageFormat_ICO_description", wxBITMAP_TYPE_ICO},
#	endif

#	if wxUSE_CUR
		{"cur", "ImageFormat_CUR_description", wxBITMAP_TYPE_CUR},
#	endif

	{"xpm", "ImageFormat_XPM_description", wxBITMAP_TYPE_XPM},

	{"bmp", "ImageFormat_BMP_description", wxBITMAP_TYPE_BMP}
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
		formats[i]->description = LangGet(SupportedImageFormats[i].descriptionId);
		formats[i]->format = SupportedImageFormats[i].format;
	}

	return numFormats;
}


#endif /* GUICONFIG_H_ */
