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

#include "IMGFormatProvider.h"
#include <gtaformats/img/IMGArchive.h>
#include <string>
#include "IMGPanel.h"
#include <wx/wx.h>
#include "../guiconfig.h"


bool IMGFormatProvider::canDisplay(const wxString& filename) const
{
	std::string str = std::string(filename.mb_str());
	return IMGArchive::isValidIMGFilename(str)
			||  IMGArchive::isValidDIRFilename(str);
}


FileDisplayer* IMGFormatProvider::openDisplayer(wxWindow* parent, DataSource* source)
{
	IMGPanel* panel = new IMGPanel(parent);
	panel->display(source);
	return panel;
}


/*FileDisplayer* IMGFormatProvider::openDisplayer(wxWindow* parent, const wxString& filename)
{
	IMGPanel* panel = new IMGPanel(parent);
	panel->displayFile(filename);
	return panel;
}*/


wxString IMGFormatProvider::getFileWildcard() const
{
	return LangGet("Format_IMG_fileWildcard");
}


wxString IMGFormatProvider::getDescription(const wxString& filename) const
{
	return LangGet("Format_IMG_description");
}
