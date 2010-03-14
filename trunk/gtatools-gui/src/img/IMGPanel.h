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

#ifndef __IMGPanel__
#define __IMGPanel__

#include "../WxGUI.h"
#include <wx/sizer.h>
#include <fstream>
#include <gtaformats/img/IMGArchive.h>
#include "../FileDisplayer.h"
#include <vector>

using std::ifstream;
using std::vector;

class IMGPanel : public IMGPanelPrototype
{
protected:
	virtual void onSelectionChanged(wxCommandEvent& evt);
	void onExtract(wxCommandEvent& evt);
	void onApplyFilter(wxCommandEvent& evt);

public:
	static bool canDisplay(const wxString& filename);

public:
	IMGPanel(wxWindow* parent);
	virtual ~IMGPanel();
	virtual bool doDisplay(DataSource* source);
	virtual void doClose();

private:
	void displayFiltered(const wxString& filter);

private:
	//wxMenu* menu;
	//wxMenu* txdMenu;
	//ifstream* stream;
	IMGArchive* archive;
	FileDisplayer* entryDisplayer;
	wxStaticBoxSizer* entryContentSizer;
	vector<wxString*> displayStrings;
	//wxWindow* pluginWindow;
};

#endif // __IMGPanel__
