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
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include "txd/TXDTextureDisplayer.h"
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/splitter.h>
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include "dff/DFFRenderer.h"

///////////////////////////////////////////////////////////////////////////


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
		wxStaticText* nameLabel;
		wxNotebook* entryNotebook;
		wxPanel* infoPanel;
		wxPanel* m_panel2;
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
		wxStaticText* numMipmapsDescLabel;
		wxStaticText* numMipmapsLabel;
		wxStaticText* paletteDescLabel;
		wxStaticText* paletteLabel;
		wxPanel* imagePanel;
		TXDTextureDisplayer* image;
		wxButton* extractButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onTextureSelectionChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void onExtract( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		TXDPanelPrototype( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 615,472 ), long style = wxTAB_TRAVERSAL );
		~TXDPanelPrototype();
		void m_splitter1OnIdle( wxIdleEvent& )
		{
		m_splitter1->SetSashPosition( 260 );
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
		wxMenu* helpMenu;
		wxMenuItem* aboutItem;
		wxBoxSizer* sizer;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onOpen( wxCommandEvent& event ){ event.Skip(); }
		virtual void onClose( wxCommandEvent& event ){ event.Skip(); }
		virtual void onAbout( wxCommandEvent& event ){ event.Skip(); }
		
	
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
		wxStaticText* imgFileLabel;
		wxNotebook* notebook;
		wxPanel* m_panel71;
		wxStaticText* imgVersionDescLabel;
		wxStaticText* imgVersionLabel;
		wxStaticText* imgNumEntriesDescLabel;
		wxStaticText* imgNumEntriesLabel;
		wxPanel* m_panel8;
		wxPanel* m_panel11;
		wxStaticText* m_staticText37;
		wxTextCtrl* filterField;
		wxButton* applyFilterButton;
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel7;
		wxListBox* fileList;
		wxPanel* entryPanel;
		wxBoxSizer* entrySizer;
		wxStaticText* fileLabel;
		wxNotebook* entryNotebook;
		wxPanel* entryInfoPanel;
		wxStaticText* typeDescLabel;
		wxStaticText* typeLabel;
		wxStaticText* offsetDescLabel;
		wxStaticText* offsetLabel;
		wxStaticText* sizeDescLabel;
		wxStaticText* sizeLabel;
		wxButton* extractButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onApplyFilter( wxCommandEvent& event ){ event.Skip(); }
		virtual void onSelectionChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void onExtract( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		IMGPanelPrototype( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 642,501 ), long style = wxTAB_TRAVERSAL );
		~IMGPanelPrototype();
		void m_splitter2OnIdle( wxIdleEvent& )
		{
		m_splitter2->SetSashPosition( 273 );
		m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( IMGPanelPrototype::m_splitter2OnIdle ), NULL, this );
		}
		
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class DFFPanelPrototype
///////////////////////////////////////////////////////////////////////////////
class DFFPanelPrototype : public FileDisplayer
{
	private:
	
	protected:
		wxNotebook* notebook;
		wxPanel* framePanel;
		wxSplitterWindow* m_splitter4;
		wxPanel* m_panel17;
		wxListBox* frameList;
		wxPanel* m_panel18;
		wxStaticText* frameNameLabel;
		wxStaticText* frameOffsetDescLabel;
		wxStaticText* frameOffsetLabel;
		wxStaticText* frameRotationDescLabel;
		wxStaticText* frameRotationLabel;
		wxStaticText* frameParentDescLabel;
		wxStaticText* frameParentLabel;
		wxPanel* geometriesPanel;
		wxSplitterWindow* m_splitter5;
		wxPanel* m_panel22;
		wxListBox* geometryList;
		wxPanel* m_panel23;
		wxNotebook* geometryNotebook;
		wxPanel* geometryInfoPanel;
		wxStaticText* geometryFormatDescLabel;
		wxStaticText* geometryFormatLabel;
		wxStaticText* geometryFlagsDescLabel;
		wxStaticText* geometryFlagsLabel;
		wxStaticText* geometryUVCountDescLabel;
		wxStaticText* geometryUVCountLabel;
		wxStaticText* geometryVerticesDescLabel;
		wxStaticText* geometryVerticesLabel;
		wxStaticText* geometryBoundsDescLabel;
		wxStaticText* geometryBoundsLabel;
		wxStaticText* geometryMaterialsDescLabel;
		wxStaticText* geometryMaterialsLabel;
		wxStaticText* geometryPartsDescLabel;
		wxStaticText* geometryPartsLabel;
		wxStaticText* geometryFrameDescLabel;
		wxStaticText* geometryFrameLabel;
		wxPanel* geometryMaterialsPanel;
		wxSplitterWindow* m_splitter6;
		wxPanel* m_panel26;
		wxListBox* geometryMaterialsList;
		wxPanel* m_panel27;
		wxStaticText* geometryMaterialColorDescLabel;
		wxStaticText* geometryMaterialColorLabel;
		wxStaticBoxSizer* geometryMaterialTexturesSizer;
		wxSplitterWindow* m_splitter8;
		wxPanel* m_panel31;
		wxListBox* geometryMaterialTextureList;
		wxPanel* m_panel32;
		wxStaticText* geometryMaterialTextureDiffnameDescLabel;
		wxStaticText* geometryMaterialTextureDiffnameLabel;
		wxStaticText* geometryMaterialTextureAlphaDescLabel;
		wxStaticText* geometryMaterialTextureAlphaLabel;
		wxStaticText* geometryMaterialTextureFilterDescLabel;
		wxStaticText* geometryMaterialTextureFilterLabel;
		wxPanel* geometryPartsPanel;
		wxSplitterWindow* m_splitter7;
		wxPanel* m_panel29;
		wxListBox* geometryPartsList;
		wxPanel* m_panel30;
		wxStaticText* geometryPartIndicesDescLabel;
		wxStaticText* geometryPartIndicesLabel;
		wxStaticText* geometryPartMaterialDescLabel;
		wxStaticText* geometryPartMaterialLabel;
		wxPanel* geometryRenderPanel;
		DFFRenderer* renderer;
		wxPanel* renderPanel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void onFrameSelectionChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void onGeometrySelectionChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void onGeometryMaterialSelectionChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void onGeometryMaterialTextureSelectionChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void onGeometryPartSelectionChanged( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		DFFPanelPrototype( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 635,480 ), long style = wxTAB_TRAVERSAL );
		~DFFPanelPrototype();
		void m_splitter4OnIdle( wxIdleEvent& )
		{
		m_splitter4->SetSashPosition( 169 );
		m_splitter4->Disconnect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter4OnIdle ), NULL, this );
		}
		
		void m_splitter5OnIdle( wxIdleEvent& )
		{
		m_splitter5->SetSashPosition( 167 );
		m_splitter5->Disconnect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter5OnIdle ), NULL, this );
		}
		
		void m_splitter6OnIdle( wxIdleEvent& )
		{
		m_splitter6->SetSashPosition( 124 );
		m_splitter6->Disconnect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter6OnIdle ), NULL, this );
		}
		
		void m_splitter8OnIdle( wxIdleEvent& )
		{
		m_splitter8->SetSashPosition( 129 );
		m_splitter8->Disconnect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter8OnIdle ), NULL, this );
		}
		
		void m_splitter7OnIdle( wxIdleEvent& )
		{
		m_splitter7->SetSashPosition( 176 );
		m_splitter7->Disconnect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter7OnIdle ), NULL, this );
		}
		
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class AboutFramePrototype
///////////////////////////////////////////////////////////////////////////////
class AboutFramePrototype : public wxFrame 
{
	private:
	
	protected:
		wxBoxSizer* sizer;
		wxStaticText* titleLabel;
		wxStaticText* internalTitleLabel;
		wxStaticText* versionLabel;
		wxStaticText* authorLabel;
		wxStaticText* licenseLabel;
	
	public:
		AboutFramePrototype( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("About"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~AboutFramePrototype();
	
};

#endif //__WxGUI__
