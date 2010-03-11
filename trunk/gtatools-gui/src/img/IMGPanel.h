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
