/*
 * DFFFormatProvider.cpp
 *
 *  Created on: 31.01.2010
 *      Author: alemariusnexus
 */

#include "DFFFormatProvider.h"
#include "DFFPanel.h"
#include "../guiconfig.h"

bool DFFFormatProvider::canDisplay(const wxString& filename) const
{
	return filename.Lower().EndsWith(wxT(".dff"));
}


FileDisplayer* DFFFormatProvider::openDisplayer(wxWindow* parent, DataSource* source)
{
	DFFPanel* panel = new DFFPanel(parent);
	panel->display(source);
	return panel;
}


wxString DFFFormatProvider::getFileWildcard() const
{
	return LangGet("Format_DFF_fileWildcard");
}


wxString DFFFormatProvider::getDescription(const wxString& filename) const
{
	return LangGet("Format_DFF_description");
}
