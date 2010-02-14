/*
 * lang.h
 *
 *  Created on: 25.01.2010
 *      Author: alemariusnexus
 */

#ifndef LANG_H_
#define LANG_H_

#include <cstdarg>
#include <cstdio>
#include <wx/wx.h>


#define _LANG_BEGIN(name)\
	const LangEntry (name)[] = {
#define _LANG_END()\
		{INVALID_LANG_ID, wxT("")}\
	};
#define _LANG_VALUE(id, value)\
	{id, wxT(value)},



enum {
	INVALID_LANG_ID,

	Misc_Yes,
	Misc_No,

	ILError_FileAlreadyExists,
	ILError_Unknown,

	Format_TXD_description,
	Format_TXD_fileWildcard,
	Format_IMG_description,
	Format_IMG_fileWildcard,
	Format_IMG_VER1,
	Format_IMG_VER2,
	Format_Unknown_description,

	Dialog_ErrorTitle,
	Dialog_ErrorUnknownFileFormat,
	Dialog_ErrorOpeningIMG,
	Dialog_ErrorSavingTexture,
	Dialog_ErrorOpeningTXD,

	ImageFormat_PNG_description,
	ImageFormat_GIF_description,
	ImageFormat_JPEG_description,
	ImageFormat_TGA_description,
	ImageFormat_TIF_description,
	ImageFormat_SGI_description,
	ImageFormat_BMP_description,
	ImageFormat_CHEAD_description,
	ImageFormat_PSD_description,

	TXDPanel_nameLabel_emptyValue,
	TXDPanel_formatDescLabel_value,
	TXDPanel_bppDescLabel_value,
	TXDPanel_widthDescLabel_value,
	TXDPanel_heightDescLabel_value,
	TXDPanel_alphaTextureDescLabel_value,
	TXDPanel_compressionDescLabel_value,
	TXDPanel_alphaUsedDescLabel_value,
	TXDPanel_numMipmapsDescLabel_value,
	TXDPanel_paletteDescLabel_value,
	TXDPanel_dlgExtractItem_title,
	TXDPanel_dlgExtractItems_title,
	TXDPanel_dlgExtractFormat_title,
	TXDPanel_dlgExtractFormat_text,
	TXDPanel_compression_DXT1,
	TXDPanel_compression_DXT3,
	TXDPanel_compression_None,
	TXDPanel_extractButton_label,

	IMGPanel_imgVersionDescLabel_value,
	IMGPanel_imgNumEntriesDescLabel_value,
	IMGPanel_typeDescLabel_value,
	IMGPanel_offsetDescLabel_value,
	IMGPanel_sizeDescLabel_value,

	MainFrame_title,
	MainFrame_fileMenu_label,
	MainFrame_imgMenu_label,
	MainFrame_txdMenu_label,
	MainFrame_openItem_label,
	MainFrame_dlgOpenFile_title,
	MainFrame_closeItem_label,
	MainFrame_txdExtractItem_label,
};


wxString LangGet(int id);
wxString LangGetFormatted(int id, ...);



struct LangEntry {
	int id;
	wxString text;
};



// **********************************************
// *											*
// *				E N G L I S H				*
// *											*
// **********************************************

_LANG_BEGIN(English)
	_LANG_VALUE(Misc_Yes, "Yes")
	_LANG_VALUE(Misc_No, "No")

	_LANG_VALUE(ILError_FileAlreadyExists, "File already exists")
	_LANG_VALUE(ILError_Unknown, "Unknown error")

	_LANG_VALUE(Format_TXD_description, "GTA Texture Dictionary (TXD)")
	_LANG_VALUE(Format_TXD_fileWildcard, "TXD file (*.txd)|*.txd")
	_LANG_VALUE(Format_IMG_description, "IMG/DIR archive")
	_LANG_VALUE(Format_IMG_fileWildcard, "IMG/DIR archive (*.img ; *.dir)|*.img;*.dir")
	_LANG_VALUE(Format_IMG_VER1, "Version 1 (GTA III / GTA VC)")
	_LANG_VALUE(Format_IMG_VER2, "Version 2 (GTA SA)")
	_LANG_VALUE(Format_Unknown_description, "(Unknown)")

	_LANG_VALUE(Dialog_ErrorTitle, "Error")
	_LANG_VALUE(Dialog_ErrorUnknownFileFormat, "The file has an unknown format!")
	_LANG_VALUE(Dialog_ErrorOpeningIMG, "Error opening IMG file: %s")
	_LANG_VALUE(Dialog_ErrorSavingTexture, "Error saving texture! Error Code: %d (%s)")
	_LANG_VALUE(Dialog_ErrorOpeningTXD, "Error opening TXD file: %s")

	_LANG_VALUE(TXDPanel_nameLabel_emptyValue, "(No texture opened)")
	_LANG_VALUE(TXDPanel_formatDescLabel_value, "Format")
	_LANG_VALUE(TXDPanel_bppDescLabel_value, "Bytes per pixel")
	_LANG_VALUE(TXDPanel_widthDescLabel_value, "Width")
	_LANG_VALUE(TXDPanel_heightDescLabel_value, "Height")
	_LANG_VALUE(TXDPanel_alphaTextureDescLabel_value, "Alpha texture")
	_LANG_VALUE(TXDPanel_compressionDescLabel_value, "Compression")
	_LANG_VALUE(TXDPanel_alphaUsedDescLabel_value, "Alpha used")
	_LANG_VALUE(TXDPanel_numMipmapsDescLabel_value, "Mipmap count")
	_LANG_VALUE(TXDPanel_paletteDescLabel_value, "Palette type")
	_LANG_VALUE(TXDPanel_dlgExtractItem_title, "Choose the destination file")
	_LANG_VALUE(TXDPanel_dlgExtractItems_title, "Choose the destination directory")
	_LANG_VALUE(TXDPanel_dlgExtractFormat_title, "Choose output format")
	_LANG_VALUE(TXDPanel_dlgExtractFormat_text, "Choose the output file format")
	_LANG_VALUE(TXDPanel_compression_DXT1, "DXT1")
	_LANG_VALUE(TXDPanel_compression_DXT3, "DXT2")
	_LANG_VALUE(TXDPanel_compression_None, "None")
	_LANG_VALUE(TXDPanel_extractButton_label, "Extract...")

	_LANG_VALUE(ImageFormat_PNG_description, "PNG file")
	_LANG_VALUE(ImageFormat_GIF_description, "GIF file")
	_LANG_VALUE(ImageFormat_JPEG_description, "JPEG file")
	_LANG_VALUE(ImageFormat_TGA_description, "Truevision Targa file")
	_LANG_VALUE(ImageFormat_TIF_description, "TIFF file")
	_LANG_VALUE(ImageFormat_SGI_description, "Silicon Graphics SGI file")
	_LANG_VALUE(ImageFormat_BMP_description, "Windows Bitmap BMP file")
	_LANG_VALUE(ImageFormat_CHEAD_description, "Static C Header file")
	_LANG_VALUE(ImageFormat_PSD_description, "PhotoShop PSD file")

	_LANG_VALUE(IMGPanel_imgVersionDescLabel_value, "Version")
	_LANG_VALUE(IMGPanel_imgNumEntriesDescLabel_value, "Number of entries")
	_LANG_VALUE(IMGPanel_typeDescLabel_value, "File type")
	_LANG_VALUE(IMGPanel_offsetDescLabel_value, "Offset")
	_LANG_VALUE(IMGPanel_sizeDescLabel_value, "Size")

	_LANG_VALUE(MainFrame_title, "GTATools GUI Viewer")
	_LANG_VALUE(MainFrame_fileMenu_label, "File")
	_LANG_VALUE(MainFrame_txdMenu_label, "TXD Actions")
	_LANG_VALUE(MainFrame_imgMenu_label, "IMG Actions")
	_LANG_VALUE(MainFrame_openItem_label, "Open...")
	_LANG_VALUE(MainFrame_dlgOpenFile_title, "Choose a file to open")
	_LANG_VALUE(MainFrame_closeItem_label, "Close")
	_LANG_VALUE(MainFrame_txdExtractItem_label, "Extract...")
_LANG_END()




// **********************************************
// *											*
// *				G E R M A N					*
// *											*
// **********************************************

_LANG_BEGIN(German)
	_LANG_VALUE(Misc_Yes, "Ja")
	_LANG_VALUE(Misc_No, "Nein")

	_LANG_VALUE(ILError_FileAlreadyExists, "Datei existiert bereits")
	_LANG_VALUE(ILError_Unknown, "Unbekannter Fehler")

	_LANG_VALUE(Format_TXD_description, "GTA Texture Dictionary (TXD)")
	_LANG_VALUE(Format_TXD_fileWildcard, "TXD-Datei (*.txd)|*.txd")
	_LANG_VALUE(Format_IMG_description, "IMG/DIR-Archiv")
	_LANG_VALUE(Format_IMG_fileWildcard, "IMG/DIR-Archiv (*.img ; *.dir)|*.img;*.dir")
	_LANG_VALUE(Format_IMG_VER1, "Version 1 (GTA III / GTA VC)")
	_LANG_VALUE(Format_IMG_VER2, "Version 2 (GTA SA)")
	_LANG_VALUE(Format_Unknown_description, "(Unbekannt)")

	_LANG_VALUE(Dialog_ErrorTitle, "Fehler")
	_LANG_VALUE(Dialog_ErrorUnknownFileFormat, "Die Datei hat ein unbekanntes Format!")
	_LANG_VALUE(Dialog_ErrorOpeningIMG, "Fehler beim \u00D6ffnen der IMG-Datei: %s")
	_LANG_VALUE(Dialog_ErrorSavingTexture, "Fehler beim Speichern der Textur. Fehlercode: %d (%s)")
	_LANG_VALUE(Dialog_ErrorOpeningTXD, "Fehler beim \u00D6ffnen der TXD-Datei: %s")

	_LANG_VALUE(TXDPanel_nameLabel_emptyValue, "(Keine Textur ge\u00F6ffnet)")
	_LANG_VALUE(TXDPanel_formatDescLabel_value, "Format")
	_LANG_VALUE(TXDPanel_bppDescLabel_value, "Bytes pro Pixel")
	_LANG_VALUE(TXDPanel_widthDescLabel_value, "Breite")
	_LANG_VALUE(TXDPanel_heightDescLabel_value, "H\u00F6he")
	_LANG_VALUE(TXDPanel_alphaTextureDescLabel_value, "Alphatextur")
	_LANG_VALUE(TXDPanel_compressionDescLabel_value, "Kompression")
	_LANG_VALUE(TXDPanel_alphaUsedDescLabel_value, "Alpha verwendet")
	_LANG_VALUE(TXDPanel_numMipmapsDescLabel_value, "Anzahl Mipmaps")
	_LANG_VALUE(TXDPanel_paletteDescLabel_value, "Palettentyp")
	_LANG_VALUE(TXDPanel_dlgExtractItem_title, "Bitte die Zieldatei ausw\u00E4hlen")
	_LANG_VALUE(TXDPanel_dlgExtractItems_title, "Bitte das Zielverzeichnis ausw\u00E4hlen")
	_LANG_VALUE(TXDPanel_dlgExtractFormat_title, "Zielformat auswählen")
	_LANG_VALUE(TXDPanel_dlgExtractFormat_text, "Bitte das Format der Zieldatei angeben")
	_LANG_VALUE(TXDPanel_compression_DXT1, "DXT1")
	_LANG_VALUE(TXDPanel_compression_DXT3, "DXT3")
	_LANG_VALUE(TXDPanel_compression_None, "Keine")
	_LANG_VALUE(TXDPanel_extractButton_label, "Extrahieren...")

	_LANG_VALUE(ImageFormat_PNG_description, "PNG-Datei")
	_LANG_VALUE(ImageFormat_GIF_description, "GIF-Datei")
	_LANG_VALUE(ImageFormat_JPEG_description, "JPEG-Datei")
	_LANG_VALUE(ImageFormat_TGA_description, "Truevision Targa-Datei")
	_LANG_VALUE(ImageFormat_TIF_description, "TIFF-Datei")
	_LANG_VALUE(ImageFormat_SGI_description, "Silicon Graphics SGI-Datei")
	_LANG_VALUE(ImageFormat_BMP_description, "Windows Bitmap BMP-Datei")
	_LANG_VALUE(ImageFormat_CHEAD_description, "Statische C-Headerdatei")
	_LANG_VALUE(ImageFormat_PSD_description, "PhotoShop PSD-Datei")

	_LANG_VALUE(IMGPanel_imgVersionDescLabel_value, "Version")
	_LANG_VALUE(IMGPanel_imgNumEntriesDescLabel_value, "Anzahl Eintr\u00E4ge")
	_LANG_VALUE(IMGPanel_typeDescLabel_value, "Dateityp")
	_LANG_VALUE(IMGPanel_offsetDescLabel_value, "Offset")
	_LANG_VALUE(IMGPanel_sizeDescLabel_value, "Gr\u00F6\u00DFe")

	_LANG_VALUE(MainFrame_title, "GTATools GUI Viewer")
	_LANG_VALUE(MainFrame_fileMenu_label, "Datei")
	_LANG_VALUE(MainFrame_txdMenu_label, "TXD-Aktionen")
	_LANG_VALUE(MainFrame_imgMenu_label, "IMG-Aktionen")
	_LANG_VALUE(MainFrame_openItem_label, "\u00D6ffnen...")
	_LANG_VALUE(MainFrame_dlgOpenFile_title, "Bitte die zu \u00F6ffnende Datei ausw\u00E4hlen")
	_LANG_VALUE(MainFrame_closeItem_label, "Schlie\u00DFen")
	_LANG_VALUE(MainFrame_txdExtractItem_label, "Entpacken...")
_LANG_END()


static const LangEntry* CurrentLanguage = German;


inline wxString LangGet(int id) {
	for (int i = 0 ; CurrentLanguage[i].id != INVALID_LANG_ID ; i++) {
		if (CurrentLanguage[i].id == id) {
			return CurrentLanguage[i].text;
		}
	}

	return wxString(wxT(""));
}

inline wxString LangGetFormatted(int id, ...) {
	va_list list;
	va_start(list, id);

	for (int i = 0 ; CurrentLanguage[i].id != INVALID_LANG_ID ; i++) {
		if (CurrentLanguage[i].id == id) {
			return wxString::FormatV(CurrentLanguage[i].text, list);
		}
	}

	va_end(list);
	return wxString(wxT(""));
}


#endif /* LANG_H_ */
