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
