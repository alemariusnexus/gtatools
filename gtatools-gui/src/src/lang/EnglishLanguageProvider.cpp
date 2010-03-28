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

#include "EnglishLanguageProvider.h"

EnglishLanguageProvider::EnglishLanguageProvider()
{
	addEntry("ProgramName", wxT("GTATools GUI Viewer"));
	addEntry("ProgramInternalName", wxT("gtatools-gui"));
	addEntry("Author", wxT("Alemarius Nexus"));
	addEntry("License", wxT("GNU General Public License"));

	addEntry("Misc_Yes", wxT("Yes"));
	addEntry("Misc_No", wxT("No"));

	addEntry("ILError_FileAlreadyExists", wxT("File already exists"));
	addEntry("ILError_Unknown", wxT("Unknown error"));

	addEntry("Format_TXD_description", wxT("GTA Texture Dictionary (TXD)"));
	addEntry("Format_TXD_fileWildcard", wxT("TXD file (*.txd)|*.txd"));
	addEntry("Format_IMG_description", wxT("IMG/DIR archive"));
	addEntry("Format_IMG_fileWildcard", wxT("IMG/DIR archive (*.img ; *.dir)|*.img;*.dir"));
	addEntry("Format_IMG_VER1", wxT("Version 1 (GTA III / GTA VC)"));
	addEntry("Format_IMG_VER2", wxT("Version 2 (GTA SA)"));
	addEntry("Format_Unknown_description", wxT("(Unknown)"));
	addEntry("Format_DFF_description", wxT("DFF Mesh file"));
	addEntry("Format_DFF_fileWildcard", wxT("DFF file (*.dff)|*.dff"));

	addEntry("Dialog_ErrorTitle", wxT("Error"));
	addEntry("Dialog_ErrorUnknownFileFormat", wxT("The file has an unknown format!"));
	addEntry("Dialog_ErrorOpeningIMG", wxT("Error opening IMG file: %s"));
	addEntry("Dialog_ErrorSavingTexture", wxT("Error saving texture! Error Code: %d (%s)"));
	addEntry("Dialog_ErrorOpeningTXD", wxT("Error opening TXD file: %s"));

	addEntry("TXDPanel_nameLabel_emptyValue", wxT("(No texture opened)"));
	addEntry("TXDPanel_formatDescLabel_value", wxT("Format"));
	addEntry("TXDPanel_bppDescLabel_value", wxT("Bytes per pixel"));
	addEntry("TXDPanel_widthDescLabel_value", wxT("Width"));
	addEntry("TXDPanel_heightDescLabel_value", wxT("Height"));
	addEntry("TXDPanel_alphaTextureDescLabel_value", wxT("Alpha texture"));
	addEntry("TXDPanel_compressionDescLabel_value", wxT("Compression"));
	addEntry("TXDPanel_alphaUsedDescLabel_value", wxT("Alpha used"));
	addEntry("TXDPanel_numMipmapsDescLabel_value", wxT("Mipmap count"));
	addEntry("TXDPanel_paletteDescLabel_value", wxT("Palette type"));
	addEntry("TXDPanel_dlgExtractItem_title", wxT("Choose the destination file"));
	addEntry("TXDPanel_dlgExtractItems_title", wxT("Choose the destination directory"));
	addEntry("TXDPanel_dlgExtractFormat_title", wxT("Choose output format"));
	addEntry("TXDPanel_dlgExtractFormat_text", wxT("Choose the output file format"));
	addEntry("TXDPanel_compression_DXT1", wxT("DXT1"));
	addEntry("TXDPanel_compression_DXT3", wxT("DXT2"));
	addEntry("TXDPanel_compression_None", wxT("None"));
	addEntry("TXDPanel_extractButton_label", wxT("Extract..."));
	addEntry("TXDPanel_infoSizer_label", wxT("Information"));
	addEntry("TXDPanel_nameLabel_label", wxT("%s"));
	addEntry("TXDPanel_infoPanel_label", wxT("Information"));
	addEntry("TXDPanel_imagePanel_label", wxT("Image View"));

	addEntry("ImageFormat_PNG_description", wxT("PNG file"));
	addEntry("ImageFormat_PCX_description", wxT("PCX file"));
	addEntry("ImageFormat_JPEG_description", wxT("JPEG file"));
	addEntry("ImageFormat_PNM_description", wxT("PNM file"));
	addEntry("ImageFormat_TIFF_description", wxT("TIFF file"));
	addEntry("ImageFormat_ICO_description", wxT("ICO file"));
	addEntry("ImageFormat_CUR_description", wxT("CUR file"));
	addEntry("ImageFormat_XPM_description", wxT("X Pixmap XPM file"));
	addEntry("ImageFormat_BMP_description", wxT("Windows Bitmap BMP file"));
	addEntry("ImageFormat_PCX_description", wxT("PCX file"));

	addEntry("IMGPanel_imgVersionDescLabel_value", wxT("Version"));
	addEntry("IMGPanel_imgNumEntriesDescLabel_value", wxT("Number of entries"));
	addEntry("IMGPanel_typeDescLabel_value", wxT("File type"));
	addEntry("IMGPanel_offsetDescLabel_value", wxT("Offset"));
	addEntry("IMGPanel_sizeDescLabel_value", wxT("Size"));
	addEntry("IMGPanel_infoSizer_label", wxT("Information"));
	addEntry("IMGPanel_entriesSizer_label", wxT("Entries"));
	addEntry("IMGPanel_entryInfoSizer_label", wxT("Information"));
	addEntry("IMGPanel_entryContentSizer_label", wxT("Content"));
	addEntry("IMGPanel_imgFileLabel_label", wxT("%s"));
	addEntry("IMGPanel_fileLabel_label", wxT("%s"));
	addEntry("IMGPanel_extractButton_label", wxT("Extract..."));
	addEntry("IMGPanel_dlgExtractItem_title", wxT("Choose the destination file"));
	addEntry("IMGPanel_dlgExtractItems_title", wxT("Choose the destination directory"));
	addEntry("IMGPanel_entryInfoPanel_label", wxT("Information"));
	addEntry("IMGPanel_entryContentPanel_label", wxT("Content"));
	addEntry("IMGPanel_infoPanel_label", wxT("Information"));
	addEntry("IMGPanel_entriesPanel_label", wxT("Entries"));

	addEntry("DFFPanel_framePanel_label", wxT("Frames"));
	addEntry("DFFPanel_frameOffsetDescLabel_label", wxT("Offset"));
	addEntry("DFFPanel_frameRotationDescLabel_label", wxT("Rotation Matrix"));
	addEntry("DFFPanel_frameParentDescLabel_label", wxT("Parent"));
	addEntry("DFFPanel_frameParentLabel_noParent_label", wxT("(None)"));
	addEntry("DFFPanel_geometriesPanel_label", wxT("Geometries"));
	addEntry("DFFPanel_geometryInfoPanel_label", wxT("Information"));
	addEntry("DFFPanel_geometryMaterialsPanel_label", wxT("Materials"));
	addEntry("DFFPanel_geometryPartsPanel_label", wxT("Parts"));
	addEntry("DFFPanel_geometryFormatDescLabel_label", wxT("Format"));
	addEntry("DFFPanel_geometryFlagsDescLabel_label", wxT("Flags"));
	addEntry("DFFPanel_geometryUVCountDescLabel_label", wxT("UV set count"));
	addEntry("DFFPanel_geometryVerticesDescLabel_label", wxT("Vertices"));
	addEntry("DFFPanel_geometryBoundsDescLabel_label", wxT("Bounding Sphere"));
	addEntry("DFFPanel_geometryMaterialsDescLabel_label", wxT("Materials"));
	addEntry("DFFPanel_geometryPartsDescLabel_label", wxT("Parts"));
	addEntry("DFFPanel_geometryFrameDescLabel_label", wxT("Frame"));
	addEntry("DFFPanel_geometryMaterialColorDescLabel_label", wxT("Color"));
	addEntry("DFFPanel_geometryMaterialTexturesSizer_label", wxT("Textures"));
	addEntry("DFFPanel_geometryMaterialTextureDiffnameDescLabel_label", wxT("Diffuse name"));
	addEntry("DFFPanel_geometryMaterialTextureAlphaDescLabel_label", wxT("Alpha name"));
	addEntry("DFFPanel_geometryMaterialTextureFilterDescLabel_label", wxT("Filter flags"));
	addEntry("DFFPanel_geometryPartIndicesDescLabel_label", wxT("Indices"));
	addEntry("DFFPanel_geometryPartMaterialDescLabel_label", wxT("Material"));

	addEntry("MainFrame_title", wxT("${ProgramName}"));
	addEntry("MainFrame_fileMenu_label", wxT("File"));
	addEntry("MainFrame_helpMenu_label", wxT("Help"));
	addEntry("MainFrame_txdMenu_label", wxT("TXD Actions"));
	addEntry("MainFrame_imgMenu_label", wxT("IMG Actions"));
	addEntry("MainFrame_openItem_label", wxT("Open..."));
	addEntry("MainFrame_dlgOpenFile_title", wxT("Choose a file to open"));
	addEntry("MainFrame_closeItem_label", wxT("Close"));
	addEntry("MainFrame_txdExtractItem_label", wxT("Extract..."));
	addEntry("MainFrame_aboutItem_label", wxT("About..."));

	addEntry("AboutFrame_title", wxT("About"));
	addEntry("AboutFrame_titleLabel_value", wxT("${ProgramName}"));
	addEntry("AboutFrame_internalTitleLabel_value", wxT("(${ProgramInternalName})"));
	addEntry("AboutFrame_versionLabel_value", wxT("Version: %s"));
	addEntry("AboutFrame_authorLabel_value", wxT("Developed by: ${Author}"));
	addEntry("AboutFrame_licenseLabel_value", wxT("Released under the terms of the ${License}"));
}
