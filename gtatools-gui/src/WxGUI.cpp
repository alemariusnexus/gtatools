///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "WxGUI.h"

///////////////////////////////////////////////////////////////////////////

TXDPanelPrototype::TXDPanelPrototype( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : FileDisplayer( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_splitter1 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter1->Connect( wxEVT_IDLE, wxIdleEventHandler( TXDPanelPrototype::m_splitter1OnIdle ), NULL, this );
	m_panel5 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );
	
	textureList = new wxListBox( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_HSCROLL|wxLB_MULTIPLE|wxLB_SORT ); 
	bSizer11->Add( textureList, 1, wxALL|wxEXPAND, 5 );
	
	m_panel5->SetSizer( bSizer11 );
	m_panel5->Layout();
	bSizer11->Fit( m_panel5 );
	m_panel1 = new wxPanel( m_splitter1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	m_panel2 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	nameLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("(No texture opened)"), wxDefaultPosition, wxDefaultSize, 0 );
	nameLabel->Wrap( -1 );
	nameLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer3->Add( nameLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );
	
	formatDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("bla"), wxDefaultPosition, wxDefaultSize, 0 );
	formatDescLabel->Wrap( -1 );
	bSizer4->Add( formatDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	formatLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	formatLabel->Wrap( -1 );
	bSizer4->Add( formatLabel, 1, wxALL, 5 );
	
	bSizer3->Add( bSizer4, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );
	
	bppDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Bytes per pixel"), wxDefaultPosition, wxDefaultSize, 0 );
	bppDescLabel->Wrap( -1 );
	bSizer41->Add( bppDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	bppLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	bppLabel->Wrap( -1 );
	bSizer41->Add( bppLabel, 1, wxALL, 5 );
	
	bSizer3->Add( bSizer41, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxHORIZONTAL );
	
	widthDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Width"), wxDefaultPosition, wxDefaultSize, 0 );
	widthDescLabel->Wrap( -1 );
	bSizer42->Add( widthDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	widthLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	widthLabel->Wrap( -1 );
	bSizer42->Add( widthLabel, 1, wxALL, 5 );
	
	bSizer3->Add( bSizer42, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxHORIZONTAL );
	
	heightDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Height"), wxDefaultPosition, wxDefaultSize, 0 );
	heightDescLabel->Wrap( -1 );
	bSizer43->Add( heightDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	heightLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	heightLabel->Wrap( -1 );
	bSizer43->Add( heightLabel, 1, wxALL, 5 );
	
	bSizer3->Add( bSizer43, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxHORIZONTAL );
	
	alphaTextureDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Alpha texture"), wxDefaultPosition, wxDefaultSize, 0 );
	alphaTextureDescLabel->Wrap( -1 );
	bSizer44->Add( alphaTextureDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	alphaTextureLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	alphaTextureLabel->Wrap( -1 );
	bSizer44->Add( alphaTextureLabel, 1, wxALL, 5 );
	
	bSizer3->Add( bSizer44, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxHORIZONTAL );
	
	compressionDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Compression"), wxDefaultPosition, wxDefaultSize, 0 );
	compressionDescLabel->Wrap( -1 );
	bSizer45->Add( compressionDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	compressionLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	compressionLabel->Wrap( -1 );
	bSizer45->Add( compressionLabel, 1, wxALL, 5 );
	
	bSizer3->Add( bSizer45, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer451;
	bSizer451 = new wxBoxSizer( wxHORIZONTAL );
	
	alphaUsedDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Alpha Used"), wxDefaultPosition, wxDefaultSize, 0 );
	alphaUsedDescLabel->Wrap( -1 );
	bSizer451->Add( alphaUsedDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	alphaUsedLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	alphaUsedLabel->Wrap( -1 );
	bSizer451->Add( alphaUsedLabel, 1, wxALL, 5 );
	
	bSizer3->Add( bSizer451, 1, wxEXPAND, 5 );
	
	m_panel2->SetSizer( bSizer3 );
	m_panel2->Layout();
	bSizer3->Fit( m_panel2 );
	bSizer2->Add( m_panel2, 0, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	extractButton = new wxButton( m_panel1, wxID_ANY, wxT("Extract"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( extractButton, 0, wxALL, 5 );
	
	bSizer2->Add( bSizer20, 0, 0, 5 );
	
	image = new TXDTextureDisplayer(m_panel1);
	bSizer2->Add( image, 1, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer2 );
	m_panel1->Layout();
	bSizer2->Fit( m_panel1 );
	m_splitter1->SplitVertically( m_panel5, m_panel1, 0 );
	bSizer1->Add( m_splitter1, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	// Connect Events
	textureList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( TXDPanelPrototype::onTextureSelectionChanged ), NULL, this );
	extractButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TXDPanelPrototype::onExtract ), NULL, this );
}

TXDPanelPrototype::~TXDPanelPrototype()
{
	// Disconnect Events
	textureList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( TXDPanelPrototype::onTextureSelectionChanged ), NULL, this );
	extractButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TXDPanelPrototype::onExtract ), NULL, this );
}

MainFramePrototype::MainFramePrototype( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	menuBar = new wxMenuBar( 0 );
	fileMenu = new wxMenu();
	openItem = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("Open...") ) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL );
	fileMenu->Append( openItem );
	
	closeItem = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("Close") ) + wxT('\t') + wxT("CTRL+C"), wxEmptyString, wxITEM_NORMAL );
	fileMenu->Append( closeItem );
	closeItem->Enable( false );
	
	menuBar->Append( fileMenu, wxT("File") );
	
	txdMenu = new wxMenu();
	txdExtractItem = new wxMenuItem( txdMenu, wxID_TXD_EXTRACT, wxString( wxT("Extract...") ) , wxEmptyString, wxITEM_NORMAL );
	txdMenu->Append( txdExtractItem );
	txdExtractItem->Enable( false );
	
	menuBar->Append( txdMenu, wxT("TXD Actions") );
	
	imgMenu = new wxMenu();
	menuBar->Append( imgMenu, wxT("IMG Actions") );
	
	this->SetMenuBar( menuBar );
	
	sizer = new wxBoxSizer( wxVERTICAL );
	
	this->SetSizer( sizer );
	this->Layout();
	
	// Connect Events
	this->Connect( openItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onOpen ) );
	this->Connect( closeItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onClose ) );
}

MainFramePrototype::~MainFramePrototype()
{
	// Disconnect Events
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onOpen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onClose ) );
}

IMGPanelPrototype::IMGPanelPrototype( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : FileDisplayer( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );
	
	m_splitter2 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( IMGPanelPrototype::m_splitter2OnIdle ), NULL, this );
	m_panel7 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	fileList = new wxListBox( m_panel7, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer18->Add( fileList, 1, wxALL|wxEXPAND, 5 );
	
	m_panel7->SetSizer( bSizer18 );
	m_panel7->Layout();
	bSizer18->Fit( m_panel7 );
	infoPanel = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	infoSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	fileLabel = new wxStaticText( infoPanel, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	fileLabel->Wrap( -1 );
	fileLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer20->Add( fileLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxBoxSizer* bSizer212;
	bSizer212 = new wxBoxSizer( wxHORIZONTAL );
	
	typeDescLabel = new wxStaticText( infoPanel, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	typeDescLabel->Wrap( -1 );
	bSizer212->Add( typeDescLabel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	typeLabel = new wxStaticText( infoPanel, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	typeLabel->Wrap( -1 );
	bSizer212->Add( typeLabel, 1, wxALL, 5 );
	
	bSizer20->Add( bSizer212, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxHORIZONTAL );
	
	offsetDescLabel = new wxStaticText( infoPanel, wxID_ANY, wxT("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
	offsetDescLabel->Wrap( -1 );
	bSizer21->Add( offsetDescLabel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	offsetLabel = new wxStaticText( infoPanel, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	offsetLabel->Wrap( -1 );
	bSizer21->Add( offsetLabel, 1, wxALL, 5 );
	
	bSizer20->Add( bSizer21, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer211;
	bSizer211 = new wxBoxSizer( wxHORIZONTAL );
	
	sizeDescLabel = new wxStaticText( infoPanel, wxID_ANY, wxT("Size"), wxDefaultPosition, wxDefaultSize, 0 );
	sizeDescLabel->Wrap( -1 );
	bSizer211->Add( sizeDescLabel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	sizeLabel = new wxStaticText( infoPanel, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	sizeLabel->Wrap( -1 );
	bSizer211->Add( sizeLabel, 1, wxALL, 5 );
	
	bSizer20->Add( bSizer211, 0, wxEXPAND, 5 );
	
	infoSizer->Add( bSizer20, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( infoPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	infoSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	infoPanel->SetSizer( infoSizer );
	infoPanel->Layout();
	infoSizer->Fit( infoPanel );
	m_splitter2->SplitVertically( m_panel7, infoPanel, 310 );
	bSizer16->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer16 );
	this->Layout();
	
	// Connect Events
	fileList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( IMGPanelPrototype::onSelectionChanged ), NULL, this );
}

IMGPanelPrototype::~IMGPanelPrototype()
{
	// Disconnect Events
	fileList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( IMGPanelPrototype::onSelectionChanged ), NULL, this );
}
