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

#ifndef __TXDPanel__
#define __TXDPanel__

#include "../WxGUI.h"
#include <gtaformats/txd/TXDArchive.h>
#include <fstream>
#include <istream>
#include "../guiconfig.h"
#include "../FileDisplayer.h"
#include <gtaformats/util/stream/InputStream.h>

using std::ifstream;
using std::istream;


/** Implementing TXDPanelPrototype */
class TXDPanel : public TXDPanelPrototype
{
protected:
	void onTextureSelectionChanged(wxCommandEvent& evt);
	void onExtract(wxCommandEvent& evt);

/*public:
	TXDPanel(wxWindow* parent, wxWindow* window, wxMenu* menu);
	virtual ~TXDPanel();
	bool displayArchive(istream* stream, bool autoCloseStream = true);
	void displayArchive(TXDArchive* archive);*/

public:
	static bool canDisplay(const wxString& filename);

public:
	TXDPanel(wxWindow* parent);
	virtual ~TXDPanel();
	virtual bool doDisplay(DataSource* source);
	virtual void doClose();
	void displayArchive(TXDArchive* archive);

private:
	void displayTexture(TXDTexture* texture);

private:
	//wxMenu* menu;
	TXDArchive* archive;
	InputStream* stream;
};

#endif // __TXDPanel__
