/*
 * FileDisplayer.h
 *
 *  Created on: 30.01.2010
 *      Author: alemariusnexus
 */

#ifndef FILEDISPLAYER_H_
#define FILEDISPLAYER_H_

#include <wx/wx.h>
#include <istream>

using std::istream;

class FileDisplayer : public wxPanel {
public:
	FileDisplayer(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL,
			const wxString& name = "panel");
	virtual ~FileDisplayer() {};
	bool display(istream* stream);
	void close();
	bool displayFile(const wxString& filename);

public:
	virtual bool doDisplay(istream* stream) = 0;
	virtual void doClose() = 0;

private:
	istream* stream;
	bool open;
};

#endif /* FILEDISPLAYER_H_ */
