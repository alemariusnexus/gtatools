#ifndef __MainFrame__
#define __MainFrame__

/**
@file
Subclass of MainFramePrototype, which is generated by wxFormBuilder.
*/

#include "WxGUI.h"

/** Implementing MainFramePrototype */
class MainFrame : public MainFramePrototype
{
protected:
	// Handlers for MainFramePrototype events.
	void onOpen(wxCommandEvent& event);
	void onClose(wxCommandEvent& evt);
	void onAbout(wxCommandEvent& evt);
	
public:
	static MainFrame* getInstance();

public:
	void displayFile(const wxString& filename);

private:
	MainFrame( wxWindow* parent );

private:
	FileDisplayer* displayer;
};

#endif // __MainFrame__