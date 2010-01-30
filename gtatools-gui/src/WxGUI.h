///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __WxGUI__
#define __WxGUI__

class FileDisplayer;

#include "FileDisplayer.h"
#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include "TXDTextureDisplayer.h"
#include <wx/splitter.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/statline.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_TXD_EXTRACT 1000

///////////////////////////////////////////////////////////////////////////////
/// Class TXDPanelPrototype
///////////////////////////////////////////////////////////////////////////////
class TXDPanelPrototype : public FileDisplayer
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter1;
		wxPanel* m_panel5;
		wxListBox* textureList;
		wxPanel* m_panel1;
		wxPanel* m_panel2;
		wxStaticText* nameLabel;
		wxStaticText* formatDescLabel;
		wxStaticText* formatLabel;
		wxStaticText* bppDescLabel;
		wxStaticText* bppLabel;
		wxStaticText* widthDescLabel;
		wxStaticText* widthLabel;
		wxStaticText* heightDescLabel;
		wxStaticText* heightLabel;
		wxStaticText* alphaTextureDescLabel;
		wxStaticText* alphaTextureLabel;
		wxStaticText* compressionDescLabel;
		wxStaticText* compressionLabel;
		wxStaticText* alphaUsedDescLabel;
		wxStaticText* alphaUsedLabel;
		wxButton* extractButton;
		TXDTextureDisplayer* image;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onTextureSelectionChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void onExtract( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		TXDPanelPrototype( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 528,361 ), long style = wxTAB_TRAVERSAL );
		~TXDPanelPrototype();
		void m_splitter1OnIdle( wxIdleEvent& )
		{
		m_splitter1->SetSashPosition( 0 );
		m_splitter1->Disconnect( wxEVT_IDLE, wxIdleEventHandler( TXDPanelPrototype::m_splitter1OnIdle ), NULL, this );
		}
		
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class MainFramePrototype
///////////////////////////////////////////////////////////////////////////////
class MainFramePrototype : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* menuBar;
		wxMenu* fileMenu;
		wxMenuItem* openItem;
		wxMenuItem* closeItem;
		wxMenu* txdMenu;
		wxMenuItem* txdExtractItem;
		wxMenu* imgMenu;
		wxBoxSizer* sizer;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onOpen( wxCommandEvent& event ){ event.Skip(); }
		virtual void onClose( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		MainFramePrototype( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("GTA File Viewer"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 646,625 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~MainFramePrototype();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class IMGPanelPrototype
///////////////////////////////////////////////////////////////////////////////
class IMGPanelPrototype : public FileDisplayer
{
	private:
	
	protected:
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel7;
		wxListBox* fileList;
		wxPanel* infoPanel;
		wxBoxSizer* infoSizer;
		wxStaticText* fileLabel;
		wxStaticText* typeDescLabel;
		wxStaticText* typeLabel;
		wxStaticText* offsetDescLabel;
		wxStaticText* offsetLabel;
		wxStaticText* sizeDescLabel;
		wxStaticText* sizeLabel;
		wxStaticLine* m_staticline1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onSelectionChanged( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		IMGPanelPrototype( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 642,501 ), long style = wxTAB_TRAVERSAL );
		~IMGPanelPrototype();
		void m_splitter2OnIdle( wxIdleEvent& )
		{
		m_splitter2->SetSashPosition( 310 );
		m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( IMGPanelPrototype::m_splitter2OnIdle ), NULL, this );
		}
		
	
};

#endif //__WxGUI__
