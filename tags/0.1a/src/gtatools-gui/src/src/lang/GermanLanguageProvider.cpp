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

#include "GermanLanguageProvider.h"

GermanLanguageProvider::GermanLanguageProvider()
{
	addEntry("ProgramName", wxT("GTATools GUI Viewer"));
	addEntry("ProgramInternalName", wxT("gtatools-gui"));
	addEntry("Author", wxT("Alemarius Nexus"));
	addEntry("License", wxT("GNU General Public License"));

	addEntry("Misc_Yes", wxT("Ja"));
	addEntry("Misc_No", wxT("Nein"));

	addEntry("ILError_FileAlreadyExists", wxT("Datei existiert bereits"));
	addEntry("ILError_Unknown", wxT("Unbekannter Fehler"));

	addEntry("Format_TXD_description", wxT("GTA Texture Dictionary (TXD)"));
	addEntry("Format_TXD_fileWildcard", wxT("TXD-Datei (*.txd)|*.txd"));
	addEntry("Format_IMG_description", wxT("IMG/DIR-Archiv"));
	addEntry("Format_IMG_fileWildcard", wxT("IMG/DIR-Archiv (*.img ; *.dir)|*.img;*.dir"));
	addEntry("Format_IMG_VER1", wxT("Version 1 (GTA III / GTA VC)"));
	addEntry("Format_IMG_VER2", wxT("Version 2 (GTA SA)"));
	addEntry("Format_Unknown_description", wxT("(Unbekannt)"));
	addEntry("Format_DFF_description", wxT("DFF Meshdatei"));
	addEntry("Format_DFF_fileWildcard", wxT("DFF-Datei (*.dff)|*.dff"));

	addEntry("Dialog_ErrorTitle", wxT("Fehler"));
	addEntry("Dialog_ErrorUnknownFileFormat", wxT("Die Datei hat ein unbekanntes Format!"));
	addEntry("Dialog_ErrorOpeningIMG", wxT("Fehler beim \u00D6ffnen der IMG-Datei: %s"));
	addEntry("Dialog_ErrorSavingTexture", wxT("Fehler beim Speichern der Textur. Fehlercode: %d (%s)"));
	addEntry("Dialog_ErrorOpeningTXD", wxT("Fehler beim \u00D6ffnen der TXD-Datei: %s"));

	addEntry("TXDPanel_nameLabel_emptyValue", wxT("(Keine Textur ge\u00F6ffnet)"));
	addEntry("TXDPanel_formatDescLabel_value", wxT("Format"));
	addEntry("TXDPanel_bppDescLabel_value", wxT("Bytes pro Pixel"));
	addEntry("TXDPanel_widthDescLabel_value", wxT("Breite"));
	addEntry("TXDPanel_heightDescLabel_value", wxT("H\u00F6he"));
	addEntry("TXDPanel_alphaTextureDescLabel_value", wxT("Alphatextur"));
	addEntry("TXDPanel_compressionDescLabel_value", wxT("Kompression"));
	addEntry("TXDPanel_alphaUsedDescLabel_value", wxT("Alpha verwendet"));
	addEntry("TXDPanel_numMipmapsDescLabel_value", wxT("Anzahl Mipmaps"));
	addEntry("TXDPanel_paletteDescLabel_value", wxT("Palettentyp"));
	addEntry("TXDPanel_dlgExtractItem_title", wxT("Bitte die Zieldatei ausw\u00E4hlen"));
	addEntry("TXDPanel_dlgExtractItems_title", wxT("Bitte das Zielverzeichnis ausw\u00E4hlen"));
	addEntry("TXDPanel_dlgExtractFormat_title", wxT("Zielformat auswählen"));
	addEntry("TXDPanel_dlgExtractFormat_text", wxT("Bitte das Format der Zieldatei angeben"));
	addEntry("TXDPanel_compression_DXT1", wxT("DXT1"));
	addEntry("TXDPanel_compression_DXT3", wxT("DXT3"));
	addEntry("TXDPanel_compression_None", wxT("Keine"));
	addEntry("TXDPanel_extractButton_label", wxT("Extrahieren..."));
	addEntry("TXDPanel_infoSizer_label", wxT("Informationen"));
	addEntry("TXDPanel_nameLabel_label", wxT("%s"));
	addEntry("TXDPanel_infoPanel_label", wxT("Informationen"));
	addEntry("TXDPanel_imagePanel_label", wxT("Bildanzeige"));

	addEntry("ImageFormat_PNG_description", wxT("PNG-Datei"));
	addEntry("ImageFormat_PCX_description", wxT("PCX-Datei"));
	addEntry("ImageFormat_JPEG_description", wxT("JPEG-Datei"));
	addEntry("ImageFormat_PNM_description", wxT("PNM-Datei"));
	addEntry("ImageFormat_TIFF_description", wxT("TIFF-Datei"));
	addEntry("ImageFormat_ICO_description", wxT("ICO-Datei"));
	addEntry("ImageFormat_CUR_description", wxT("CUR-Datei"));
	addEntry("ImageFormat_XPM_description", wxT("X Pixmap XPM-Datei"));
	addEntry("ImageFormat_BMP_description", wxT("Windows Bitmap BMP-Datei"));
	addEntry("ImageFormat_PCX_description", wxT("PCX-Datei"));

	addEntry("IMGPanel_imgVersionDescLabel_value", wxT("Version"));
	addEntry("IMGPanel_imgNumEntriesDescLabel_value", wxT("Anzahl Eintr\u00E4ge"));
	addEntry("IMGPanel_typeDescLabel_value", wxT("Dateityp"));
	addEntry("IMGPanel_offsetDescLabel_value", wxT("Offset"));
	addEntry("IMGPanel_sizeDescLabel_value", wxT("Gr\u00F6\u00DFe"));
	addEntry("IMGPanel_infoSizer_label", wxT("Informationen"));
	addEntry("IMGPanel_entriesSizer_label", wxT("Eintr\u00E4ge"));
	addEntry("IMGPanel_entryInfoSizer_label", wxT("Informationen"));
	addEntry("IMGPanel_entryContentSizer_label", wxT("Inhalt"));
	addEntry("IMGPanel_imgFileLabel_label", wxT("%s"));
	addEntry("IMGPanel_fileLabel_label", wxT("%s"));
	addEntry("IMGPanel_extractButton_label", wxT("Extrahieren..."));
	addEntry("IMGPanel_dlgExtractItem_title", wxT("Bitte die Zieldatei ausw\u00E4hlen"));
	addEntry("IMGPanel_dlgExtractItems_title", wxT("Bitte das Zielverzeichnis ausw\u00E4hlen"));
	addEntry("IMGPanel_entryInfoPanel_label", wxT("Informationen"));
	addEntry("IMGPanel_entryContentPanel_label", wxT("Inhalt"));
	addEntry("IMGPanel_infoPanel_label", wxT("Informationen"));
	addEntry("IMGPanel_entriesPanel_label", wxT("Eintr\u00E4ge"));

	addEntry("DFFPanel_framePanel_label", wxT("Frames"));
	addEntry("DFFPanel_frameOffsetDescLabel_label", wxT("Offset"));
	addEntry("DFFPanel_frameRotationDescLabel_label", wxT("Rotationsmatrix"));
	addEntry("DFFPanel_frameParentDescLabel_label", wxT("Parent"));
	addEntry("DFFPanel_frameParentLabel_noParent_label", wxT("(Kein)"));
	addEntry("DFFPanel_geometriesPanel_label", wxT("Geometrien"));
	addEntry("DFFPanel_geometryInfoPanel_label", wxT("Informationen"));
	addEntry("DFFPanel_geometryMaterialsPanel_label", wxT("Materialien"));
	addEntry("DFFPanel_geometryPartsPanel_label", wxT("Einzelteile"));
	addEntry("DFFPanel_geometryFormatDescLabel_label", wxT("Format"));
	addEntry("DFFPanel_geometryFlagsDescLabel_label", wxT("Flags"));
	addEntry("DFFPanel_geometryUVCountDescLabel_label", wxT("Anzahl UV sets"));
	addEntry("DFFPanel_geometryVerticesDescLabel_label", wxT("Vertices"));
	addEntry("DFFPanel_geometryBoundsDescLabel_label", wxT("Bounding Sphere"));
	addEntry("DFFPanel_geometryMaterialsDescLabel_label", wxT("Materialien"));
	addEntry("DFFPanel_geometryPartsDescLabel_label", wxT("Einzelteile"));
	addEntry("DFFPanel_geometryFrameDescLabel_label", wxT("Frame"));
	addEntry("DFFPanel_geometryMaterialColorDescLabel_label", wxT("Farbe"));
	addEntry("DFFPanel_geometryMaterialTexturesSizer_label", wxT("Texturen"));
	addEntry("DFFPanel_geometryMaterialTextureDiffnameDescLabel_label", wxT("Diffuse name"));
	addEntry("DFFPanel_geometryMaterialTextureAlphaDescLabel_label", wxT("Alpha name"));
	addEntry("DFFPanel_geometryMaterialTextureFilterDescLabel_label", wxT("Filter flags"));
	addEntry("DFFPanel_geometryPartIndicesDescLabel_label", wxT("Indizes"));
	addEntry("DFFPanel_geometryPartMaterialDescLabel_label", wxT("Material"));

	addEntry("MainFrame_title", wxT("${ProgramName}"));
	addEntry("MainFrame_fileMenu_label", wxT("Datei"));
	addEntry("MainFrame_helpMenu_label", wxT("Hilfe"));
	addEntry("MainFrame_txdMenu_label", wxT("TXD-Aktionen"));
	addEntry("MainFrame_imgMenu_label", wxT("IMG-Aktionen"));
	addEntry("MainFrame_openItem_label", wxT("\u00D6ffnen..."));
	addEntry("MainFrame_dlgOpenFile_title", wxT("Bitte die zu \u00F6ffnende Datei ausw\u00E4hlen"));
	addEntry("MainFrame_closeItem_label", wxT("Schlie\u00DFen"));
	addEntry("MainFrame_txdExtractItem_label", wxT("Entpacken..."));
	addEntry("MainFrame_aboutItem_label", wxT("\u00DCber..."));

	addEntry("AboutFrame_title", wxT("\u00DCber"));
	addEntry("AboutFrame_titleLabel_value", wxT("${ProgramName}"));
	addEntry("AboutFrame_internalTitleLabel_value", wxT("(${ProgramInternalName})"));
	addEntry("AboutFrame_versionLabel_value", wxT("Version: %s"));
	addEntry("AboutFrame_authorLabel_value", wxT("Entwickelt von: ${Author}"));
	addEntry("AboutFrame_licenseLabel_value", wxT("Freigegeben unter den Bedingungen der ${License}"));
}
