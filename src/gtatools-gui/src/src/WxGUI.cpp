///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 29 2010)
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
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	nameLabel = new wxStaticText( m_panel1, wxID_ANY, wxT("(No texture opened)"), wxDefaultPosition, wxDefaultSize, 0 );
	nameLabel->Wrap( -1 );
	nameLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer32->Add( nameLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	entryNotebook = new wxNotebook( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	infoPanel = new wxPanel( entryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );
	
	m_panel2 = new wxPanel( infoPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 9, 2, 0, 25 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	formatDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Format"), wxDefaultPosition, wxDefaultSize, 0 );
	formatDescLabel->Wrap( -1 );
	fgSizer3->Add( formatDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	formatLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	formatLabel->Wrap( -1 );
	fgSizer3->Add( formatLabel, 1, wxALL, 5 );
	
	bppDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Bytes per pixel"), wxDefaultPosition, wxDefaultSize, 0 );
	bppDescLabel->Wrap( -1 );
	fgSizer3->Add( bppDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	bppLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	bppLabel->Wrap( -1 );
	fgSizer3->Add( bppLabel, 1, wxALL, 5 );
	
	widthDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Width"), wxDefaultPosition, wxDefaultSize, 0 );
	widthDescLabel->Wrap( -1 );
	fgSizer3->Add( widthDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	widthLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	widthLabel->Wrap( -1 );
	fgSizer3->Add( widthLabel, 1, wxALL, 5 );
	
	heightDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Height"), wxDefaultPosition, wxDefaultSize, 0 );
	heightDescLabel->Wrap( -1 );
	fgSizer3->Add( heightDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	heightLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	heightLabel->Wrap( -1 );
	fgSizer3->Add( heightLabel, 1, wxALL, 5 );
	
	alphaTextureDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Alpha texture"), wxDefaultPosition, wxDefaultSize, 0 );
	alphaTextureDescLabel->Wrap( -1 );
	fgSizer3->Add( alphaTextureDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	alphaTextureLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	alphaTextureLabel->Wrap( -1 );
	fgSizer3->Add( alphaTextureLabel, 1, wxALL, 5 );
	
	compressionDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Compression"), wxDefaultPosition, wxDefaultSize, 0 );
	compressionDescLabel->Wrap( -1 );
	fgSizer3->Add( compressionDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	compressionLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	compressionLabel->Wrap( -1 );
	fgSizer3->Add( compressionLabel, 1, wxALL, 5 );
	
	alphaUsedDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Alpha Used"), wxDefaultPosition, wxDefaultSize, 0 );
	alphaUsedDescLabel->Wrap( -1 );
	fgSizer3->Add( alphaUsedDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	alphaUsedLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	alphaUsedLabel->Wrap( -1 );
	fgSizer3->Add( alphaUsedLabel, 1, wxALL, 5 );
	
	numMipmapsDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Mipmap count"), wxDefaultPosition, wxDefaultSize, 0 );
	numMipmapsDescLabel->Wrap( -1 );
	fgSizer3->Add( numMipmapsDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	numMipmapsLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	numMipmapsLabel->Wrap( -1 );
	fgSizer3->Add( numMipmapsLabel, 1, wxALL, 5 );
	
	paletteDescLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("Palette type"), wxDefaultPosition, wxDefaultSize, 0 );
	paletteDescLabel->Wrap( -1 );
	fgSizer3->Add( paletteDescLabel, 1, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 5 );
	
	paletteLabel = new wxStaticText( m_panel2, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	paletteLabel->Wrap( -1 );
	fgSizer3->Add( paletteLabel, 1, wxALL, 5 );
	
	m_panel2->SetSizer( fgSizer3 );
	m_panel2->Layout();
	fgSizer3->Fit( m_panel2 );
	bSizer35->Add( m_panel2, 0, wxEXPAND | wxALL, 5 );
	
	infoPanel->SetSizer( bSizer35 );
	infoPanel->Layout();
	bSizer35->Fit( infoPanel );
	entryNotebook->AddPage( infoPanel, wxT("a page"), true );
	imagePanel = new wxPanel( entryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxVERTICAL );
	
	image = new TXDTextureDisplayer(imagePanel);
	bSizer34->Add( image, 1, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );
	
	imagePanel->SetSizer( bSizer34 );
	imagePanel->Layout();
	bSizer34->Fit( imagePanel );
	entryNotebook->AddPage( imagePanel, wxT("a page"), false );
	
	bSizer32->Add( entryNotebook, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	extractButton = new wxButton( m_panel1, wxID_ANY, wxT("Extract"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( extractButton, 0, wxALL, 5 );
	
	bSizer32->Add( bSizer20, 0, wxEXPAND, 5 );
	
	m_panel1->SetSizer( bSizer32 );
	m_panel1->Layout();
	bSizer32->Fit( m_panel1 );
	m_splitter1->SplitVertically( m_panel5, m_panel1, 260 );
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
	
	helpMenu = new wxMenu();
	aboutItem = new wxMenuItem( helpMenu, wxID_ANY, wxString( wxT("About") ) , wxEmptyString, wxITEM_NORMAL );
	helpMenu->Append( aboutItem );
	
	menuBar->Append( helpMenu, wxT("Help") );
	
	this->SetMenuBar( menuBar );
	
	wxBoxSizer* topSizer;
	topSizer = new wxBoxSizer( wxVERTICAL );
	
	sizer = new wxBoxSizer( wxVERTICAL );
	
	topSizer->Add( sizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( topSizer );
	this->Layout();
	
	// Connect Events
	this->Connect( openItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onOpen ) );
	this->Connect( closeItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onClose ) );
	this->Connect( aboutItem->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onAbout ) );
}

MainFramePrototype::~MainFramePrototype()
{
	// Disconnect Events
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onOpen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onClose ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFramePrototype::onAbout ) );
}

IMGPanelPrototype::IMGPanelPrototype( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : FileDisplayer( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer22;
	bSizer22 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	imgFileLabel = new wxStaticText( this, wxID_ANY, wxT("(IMG file name)"), wxDefaultPosition, wxDefaultSize, 0 );
	imgFileLabel->Wrap( -1 );
	imgFileLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer33->Add( imgFileLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	bSizer22->Add( bSizer33, 0, wxEXPAND, 5 );
	
	notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_panel71 = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 25 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	imgVersionDescLabel = new wxStaticText( m_panel71, wxID_ANY, wxT("Version"), wxDefaultPosition, wxDefaultSize, 0 );
	imgVersionDescLabel->Wrap( -1 );
	fgSizer5->Add( imgVersionDescLabel, 1, wxALL, 5 );
	
	imgVersionLabel = new wxStaticText( m_panel71, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	imgVersionLabel->Wrap( -1 );
	fgSizer5->Add( imgVersionLabel, 1, wxALL, 5 );
	
	imgNumEntriesDescLabel = new wxStaticText( m_panel71, wxID_ANY, wxT("Number of entries"), wxDefaultPosition, wxDefaultSize, 0 );
	imgNumEntriesDescLabel->Wrap( -1 );
	fgSizer5->Add( imgNumEntriesDescLabel, 1, wxALL, 5 );
	
	imgNumEntriesLabel = new wxStaticText( m_panel71, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	imgNumEntriesLabel->Wrap( -1 );
	fgSizer5->Add( imgNumEntriesLabel, 1, wxALL, 5 );
	
	m_panel71->SetSizer( fgSizer5 );
	m_panel71->Layout();
	fgSizer5->Fit( m_panel71 );
	notebook->AddPage( m_panel71, wxT("a page"), false );
	m_panel8 = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer331;
	bSizer331 = new wxBoxSizer( wxVERTICAL );
	
	m_panel11 = new wxPanel( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxStaticBoxSizer* sbSizer3;
	sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( m_panel11, wxID_ANY, wxT("Search") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 25 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText37 = new wxStaticText( m_panel11, wxID_ANY, wxT("Filter"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText37->Wrap( -1 );
	fgSizer1->Add( m_staticText37, 1, wxALIGN_CENTER|wxALL, 5 );
	
	filterField = new wxTextCtrl( m_panel11, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 250,-1 ), 0 );
	fgSizer1->Add( filterField, 1, wxALL, 5 );
	
	sbSizer3->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	applyFilterButton = new wxButton( m_panel11, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer3->Add( applyFilterButton, 0, wxALL, 5 );
	
	m_panel11->SetSizer( sbSizer3 );
	m_panel11->Layout();
	sbSizer3->Fit( m_panel11 );
	bSizer331->Add( m_panel11, 0, wxEXPAND | wxALL, 5 );
	
	m_splitter2 = new wxSplitterWindow( m_panel8, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( IMGPanelPrototype::m_splitter2OnIdle ), NULL, this );
	m_panel7 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	fileList = new wxListBox( m_panel7, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_MULTIPLE ); 
	bSizer18->Add( fileList, 1, wxALL|wxEXPAND, 5 );
	
	m_panel7->SetSizer( bSizer18 );
	m_panel7->Layout();
	bSizer18->Fit( m_panel7 );
	entryPanel = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	entrySizer = new wxBoxSizer( wxVERTICAL );
	
	fileLabel = new wxStaticText( entryPanel, wxID_ANY, wxT("(Entry name)"), wxDefaultPosition, wxDefaultSize, 0 );
	fileLabel->Wrap( -1 );
	fileLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	entrySizer->Add( fileLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	entryNotebook = new wxNotebook( entryPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	entryInfoPanel = new wxPanel( entryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 3, 2, 0, 25 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	typeDescLabel = new wxStaticText( entryInfoPanel, wxID_ANY, wxT("Type"), wxDefaultPosition, wxDefaultSize, 0 );
	typeDescLabel->Wrap( -1 );
	fgSizer2->Add( typeDescLabel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	typeLabel = new wxStaticText( entryInfoPanel, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	typeLabel->Wrap( -1 );
	fgSizer2->Add( typeLabel, 1, wxALL, 5 );
	
	offsetDescLabel = new wxStaticText( entryInfoPanel, wxID_ANY, wxT("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
	offsetDescLabel->Wrap( -1 );
	fgSizer2->Add( offsetDescLabel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	offsetLabel = new wxStaticText( entryInfoPanel, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	offsetLabel->Wrap( -1 );
	fgSizer2->Add( offsetLabel, 1, wxALL, 5 );
	
	sizeDescLabel = new wxStaticText( entryInfoPanel, wxID_ANY, wxT("Size"), wxDefaultPosition, wxDefaultSize, 0 );
	sizeDescLabel->Wrap( -1 );
	fgSizer2->Add( sizeDescLabel, 1, wxALIGN_CENTER|wxALL, 5 );
	
	sizeLabel = new wxStaticText( entryInfoPanel, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	sizeLabel->Wrap( -1 );
	fgSizer2->Add( sizeLabel, 1, wxALL, 5 );
	
	entryInfoPanel->SetSizer( fgSizer2 );
	entryInfoPanel->Layout();
	fgSizer2->Fit( entryInfoPanel );
	entryNotebook->AddPage( entryInfoPanel, wxT("a page"), false );
	
	entrySizer->Add( entryNotebook, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* bSizer271;
	bSizer271 = new wxBoxSizer( wxHORIZONTAL );
	
	extractButton = new wxButton( entryPanel, wxID_ANY, wxT("Extract..."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer271->Add( extractButton, 0, wxALL, 5 );
	
	entrySizer->Add( bSizer271, 0, wxEXPAND, 5 );
	
	entryPanel->SetSizer( entrySizer );
	entryPanel->Layout();
	entrySizer->Fit( entryPanel );
	m_splitter2->SplitVertically( m_panel7, entryPanel, 273 );
	bSizer331->Add( m_splitter2, 1, wxEXPAND, 5 );
	
	m_panel8->SetSizer( bSizer331 );
	m_panel8->Layout();
	bSizer331->Fit( m_panel8 );
	notebook->AddPage( m_panel8, wxT("a page"), true );
	
	bSizer22->Add( notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( bSizer22 );
	this->Layout();
	
	// Connect Events
	applyFilterButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IMGPanelPrototype::onApplyFilter ), NULL, this );
	fileList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( IMGPanelPrototype::onSelectionChanged ), NULL, this );
	extractButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IMGPanelPrototype::onExtract ), NULL, this );
}

IMGPanelPrototype::~IMGPanelPrototype()
{
	// Disconnect Events
	applyFilterButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IMGPanelPrototype::onApplyFilter ), NULL, this );
	fileList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( IMGPanelPrototype::onSelectionChanged ), NULL, this );
	extractButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( IMGPanelPrototype::onExtract ), NULL, this );
}

DFFPanelPrototype::DFFPanelPrototype( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : FileDisplayer( parent, id, pos, size, style )
{
	wxBoxSizer* sizer;
	sizer = new wxBoxSizer( wxVERTICAL );
	
	notebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	framePanel = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter4 = new wxSplitterWindow( framePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter4->Connect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter4OnIdle ), NULL, this );
	m_panel17 = new wxPanel( m_splitter4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxVERTICAL );
	
	frameList = new wxListBox( m_panel17, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer26->Add( frameList, 1, wxALL|wxEXPAND, 5 );
	
	m_panel17->SetSizer( bSizer26 );
	m_panel17->Layout();
	bSizer26->Fit( m_panel17 );
	m_panel18 = new wxPanel( m_splitter4, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxVERTICAL );
	
	frameNameLabel = new wxStaticText( m_panel18, wxID_ANY, wxT("(None selected)"), wxDefaultPosition, wxDefaultSize, 0 );
	frameNameLabel->Wrap( -1 );
	frameNameLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	bSizer27->Add( frameNameLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 25 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	frameOffsetDescLabel = new wxStaticText( m_panel18, wxID_ANY, wxT("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
	frameOffsetDescLabel->Wrap( -1 );
	fgSizer5->Add( frameOffsetDescLabel, 0, wxALL, 5 );
	
	frameOffsetLabel = new wxStaticText( m_panel18, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	frameOffsetLabel->Wrap( -1 );
	fgSizer5->Add( frameOffsetLabel, 0, wxALL, 5 );
	
	frameRotationDescLabel = new wxStaticText( m_panel18, wxID_ANY, wxT("Rotation Matrix"), wxDefaultPosition, wxDefaultSize, 0 );
	frameRotationDescLabel->Wrap( -1 );
	fgSizer5->Add( frameRotationDescLabel, 0, wxALL, 5 );
	
	frameRotationLabel = new wxStaticText( m_panel18, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	frameRotationLabel->Wrap( -1 );
	fgSizer5->Add( frameRotationLabel, 0, wxALL, 5 );
	
	frameParentDescLabel = new wxStaticText( m_panel18, wxID_ANY, wxT("Parent"), wxDefaultPosition, wxDefaultSize, 0 );
	frameParentDescLabel->Wrap( -1 );
	fgSizer5->Add( frameParentDescLabel, 0, wxALL, 5 );
	
	frameParentLabel = new wxStaticText( m_panel18, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	frameParentLabel->Wrap( -1 );
	fgSizer5->Add( frameParentLabel, 0, wxALL, 5 );
	
	bSizer27->Add( fgSizer5, 1, wxEXPAND, 5 );
	
	m_panel18->SetSizer( bSizer27 );
	m_panel18->Layout();
	bSizer27->Fit( m_panel18 );
	m_splitter4->SplitVertically( m_panel17, m_panel18, 169 );
	bSizer25->Add( m_splitter4, 1, wxEXPAND, 5 );
	
	framePanel->SetSizer( bSizer25 );
	framePanel->Layout();
	bSizer25->Fit( framePanel );
	notebook->AddPage( framePanel, wxT("Frames"), false );
	geometriesPanel = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxHORIZONTAL );
	
	m_splitter5 = new wxSplitterWindow( geometriesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter5->Connect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter5OnIdle ), NULL, this );
	m_panel22 = new wxPanel( m_splitter5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxVERTICAL );
	
	geometryList = new wxListBox( m_panel22, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer30->Add( geometryList, 1, wxALL|wxEXPAND, 5 );
	
	m_panel22->SetSizer( bSizer30 );
	m_panel22->Layout();
	bSizer30->Fit( m_panel22 );
	m_panel23 = new wxPanel( m_splitter5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxVERTICAL );
	
	geometryNotebook = new wxNotebook( m_panel23, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	geometryInfoPanel = new wxPanel( geometryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer6;
	fgSizer6 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer6->SetFlexibleDirection( wxBOTH );
	fgSizer6->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	geometryFormatDescLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("Format"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryFormatDescLabel->Wrap( -1 );
	fgSizer6->Add( geometryFormatDescLabel, 0, wxALL, 5 );
	
	geometryFormatLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryFormatLabel->Wrap( -1 );
	fgSizer6->Add( geometryFormatLabel, 0, wxALL, 5 );
	
	geometryFlagsDescLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("Flags"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryFlagsDescLabel->Wrap( -1 );
	fgSizer6->Add( geometryFlagsDescLabel, 0, wxALL, 5 );
	
	geometryFlagsLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryFlagsLabel->Wrap( -1 );
	fgSizer6->Add( geometryFlagsLabel, 0, wxALL, 5 );
	
	geometryUVCountDescLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("UV coordinate sets"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryUVCountDescLabel->Wrap( -1 );
	fgSizer6->Add( geometryUVCountDescLabel, 0, wxALL, 5 );
	
	geometryUVCountLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryUVCountLabel->Wrap( -1 );
	fgSizer6->Add( geometryUVCountLabel, 0, wxALL, 5 );
	
	geometryVerticesDescLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("Vertex count"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryVerticesDescLabel->Wrap( -1 );
	fgSizer6->Add( geometryVerticesDescLabel, 0, wxALL, 5 );
	
	geometryVerticesLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryVerticesLabel->Wrap( -1 );
	fgSizer6->Add( geometryVerticesLabel, 0, wxALL, 5 );
	
	geometryBoundsDescLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("Bounding Sphere"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryBoundsDescLabel->Wrap( -1 );
	fgSizer6->Add( geometryBoundsDescLabel, 0, wxALL, 5 );
	
	geometryBoundsLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryBoundsLabel->Wrap( -1 );
	fgSizer6->Add( geometryBoundsLabel, 0, wxALL, 5 );
	
	geometryMaterialsDescLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("Material count"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialsDescLabel->Wrap( -1 );
	fgSizer6->Add( geometryMaterialsDescLabel, 0, wxALL, 5 );
	
	geometryMaterialsLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialsLabel->Wrap( -1 );
	fgSizer6->Add( geometryMaterialsLabel, 0, wxALL, 5 );
	
	geometryPartsDescLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("Part count"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryPartsDescLabel->Wrap( -1 );
	fgSizer6->Add( geometryPartsDescLabel, 0, wxALL, 5 );
	
	geometryPartsLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryPartsLabel->Wrap( -1 );
	fgSizer6->Add( geometryPartsLabel, 0, wxALL, 5 );
	
	geometryFrameDescLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("Frame"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryFrameDescLabel->Wrap( -1 );
	fgSizer6->Add( geometryFrameDescLabel, 0, wxALL, 5 );
	
	geometryFrameLabel = new wxStaticText( geometryInfoPanel, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryFrameLabel->Wrap( -1 );
	fgSizer6->Add( geometryFrameLabel, 0, wxALL, 5 );
	
	geometryInfoPanel->SetSizer( fgSizer6 );
	geometryInfoPanel->Layout();
	fgSizer6->Fit( geometryInfoPanel );
	geometryNotebook->AddPage( geometryInfoPanel, wxT("Information"), true );
	geometryMaterialsPanel = new wxPanel( geometryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer32;
	bSizer32 = new wxBoxSizer( wxVERTICAL );
	
	m_splitter6 = new wxSplitterWindow( geometryMaterialsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter6->Connect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter6OnIdle ), NULL, this );
	m_panel26 = new wxPanel( m_splitter6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxVERTICAL );
	
	geometryMaterialsList = new wxListBox( m_panel26, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer33->Add( geometryMaterialsList, 1, wxALL|wxEXPAND, 5 );
	
	m_panel26->SetSizer( bSizer33 );
	m_panel26->Layout();
	bSizer33->Fit( m_panel26 );
	m_panel27 = new wxPanel( m_splitter6, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	geometryMaterialColorDescLabel = new wxStaticText( m_panel27, wxID_ANY, wxT("Color"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialColorDescLabel->Wrap( -1 );
	fgSizer8->Add( geometryMaterialColorDescLabel, 0, wxALL, 5 );
	
	geometryMaterialColorLabel = new wxStaticText( m_panel27, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialColorLabel->Wrap( -1 );
	fgSizer8->Add( geometryMaterialColorLabel, 0, wxALL, 5 );
	
	bSizer36->Add( fgSizer8, 0, wxEXPAND, 5 );
	
	geometryMaterialTexturesSizer = new wxStaticBoxSizer( new wxStaticBox( m_panel27, wxID_ANY, wxT("Textures") ), wxVERTICAL );
	
	m_splitter8 = new wxSplitterWindow( m_panel27, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter8->Connect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter8OnIdle ), NULL, this );
	m_panel31 = new wxPanel( m_splitter8, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxVERTICAL );
	
	geometryMaterialTextureList = new wxListBox( m_panel31, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer37->Add( geometryMaterialTextureList, 1, wxALL|wxEXPAND, 5 );
	
	m_panel31->SetSizer( bSizer37 );
	m_panel31->Layout();
	bSizer37->Fit( m_panel31 );
	m_panel32 = new wxPanel( m_splitter8, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer9;
	fgSizer9 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer9->SetFlexibleDirection( wxBOTH );
	fgSizer9->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	geometryMaterialTextureDiffnameDescLabel = new wxStaticText( m_panel32, wxID_ANY, wxT("Diffuse name"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialTextureDiffnameDescLabel->Wrap( -1 );
	fgSizer9->Add( geometryMaterialTextureDiffnameDescLabel, 0, wxALL, 5 );
	
	geometryMaterialTextureDiffnameLabel = new wxStaticText( m_panel32, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialTextureDiffnameLabel->Wrap( -1 );
	fgSizer9->Add( geometryMaterialTextureDiffnameLabel, 0, wxALL, 5 );
	
	geometryMaterialTextureAlphaDescLabel = new wxStaticText( m_panel32, wxID_ANY, wxT("Alpha name"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialTextureAlphaDescLabel->Wrap( -1 );
	fgSizer9->Add( geometryMaterialTextureAlphaDescLabel, 0, wxALL, 5 );
	
	geometryMaterialTextureAlphaLabel = new wxStaticText( m_panel32, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialTextureAlphaLabel->Wrap( -1 );
	fgSizer9->Add( geometryMaterialTextureAlphaLabel, 0, wxALL, 5 );
	
	geometryMaterialTextureFilterDescLabel = new wxStaticText( m_panel32, wxID_ANY, wxT("Filter flags"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialTextureFilterDescLabel->Wrap( -1 );
	fgSizer9->Add( geometryMaterialTextureFilterDescLabel, 0, wxALL, 5 );
	
	geometryMaterialTextureFilterLabel = new wxStaticText( m_panel32, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryMaterialTextureFilterLabel->Wrap( -1 );
	fgSizer9->Add( geometryMaterialTextureFilterLabel, 0, wxALL, 5 );
	
	m_panel32->SetSizer( fgSizer9 );
	m_panel32->Layout();
	fgSizer9->Fit( m_panel32 );
	m_splitter8->SplitVertically( m_panel31, m_panel32, 129 );
	geometryMaterialTexturesSizer->Add( m_splitter8, 1, wxEXPAND, 5 );
	
	bSizer36->Add( geometryMaterialTexturesSizer, 1, wxEXPAND|wxTOP, 10 );
	
	m_panel27->SetSizer( bSizer36 );
	m_panel27->Layout();
	bSizer36->Fit( m_panel27 );
	m_splitter6->SplitVertically( m_panel26, m_panel27, 124 );
	bSizer32->Add( m_splitter6, 1, wxEXPAND, 5 );
	
	geometryMaterialsPanel->SetSizer( bSizer32 );
	geometryMaterialsPanel->Layout();
	bSizer32->Fit( geometryMaterialsPanel );
	geometryNotebook->AddPage( geometryMaterialsPanel, wxT("Materials"), false );
	geometryPartsPanel = new wxPanel( geometryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxHORIZONTAL );
	
	m_splitter7 = new wxSplitterWindow( geometryPartsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter7->Connect( wxEVT_IDLE, wxIdleEventHandler( DFFPanelPrototype::m_splitter7OnIdle ), NULL, this );
	m_panel29 = new wxPanel( m_splitter7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );
	
	geometryPartsList = new wxListBox( m_panel29, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	bSizer35->Add( geometryPartsList, 1, wxALL|wxEXPAND, 5 );
	
	m_panel29->SetSizer( bSizer35 );
	m_panel29->Layout();
	bSizer35->Fit( m_panel29 );
	m_panel30 = new wxPanel( m_splitter7, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* fgSizer7;
	fgSizer7 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer7->SetFlexibleDirection( wxBOTH );
	fgSizer7->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	geometryPartIndicesDescLabel = new wxStaticText( m_panel30, wxID_ANY, wxT("Indices"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryPartIndicesDescLabel->Wrap( -1 );
	fgSizer7->Add( geometryPartIndicesDescLabel, 0, wxALL, 5 );
	
	geometryPartIndicesLabel = new wxStaticText( m_panel30, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryPartIndicesLabel->Wrap( -1 );
	fgSizer7->Add( geometryPartIndicesLabel, 0, wxALL, 5 );
	
	geometryPartMaterialDescLabel = new wxStaticText( m_panel30, wxID_ANY, wxT("Material"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryPartMaterialDescLabel->Wrap( -1 );
	fgSizer7->Add( geometryPartMaterialDescLabel, 0, wxALL, 5 );
	
	geometryPartMaterialLabel = new wxStaticText( m_panel30, wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, 0 );
	geometryPartMaterialLabel->Wrap( -1 );
	fgSizer7->Add( geometryPartMaterialLabel, 0, wxALL, 5 );
	
	m_panel30->SetSizer( fgSizer7 );
	m_panel30->Layout();
	fgSizer7->Fit( m_panel30 );
	m_splitter7->SplitVertically( m_panel29, m_panel30, 176 );
	bSizer34->Add( m_splitter7, 1, wxEXPAND, 5 );
	
	geometryPartsPanel->SetSizer( bSizer34 );
	geometryPartsPanel->Layout();
	bSizer34->Fit( geometryPartsPanel );
	geometryNotebook->AddPage( geometryPartsPanel, wxT("Parts"), false );
	geometryRenderPanel = new wxPanel( geometryNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer291;
	bSizer291 = new wxBoxSizer( wxVERTICAL );
	
	int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 16, 0};
	renderer = new DFFRenderer(geometryRenderPanel, args);
	bSizer291->Add( renderer, 1, wxALL|wxEXPAND, 5 );
	
	geometryRenderPanel->SetSizer( bSizer291 );
	geometryRenderPanel->Layout();
	bSizer291->Fit( geometryRenderPanel );
	geometryNotebook->AddPage( geometryRenderPanel, wxT("Rendering"), false );
	
	bSizer31->Add( geometryNotebook, 5, wxEXPAND | wxALL, 5 );
	
	m_panel23->SetSizer( bSizer31 );
	m_panel23->Layout();
	bSizer31->Fit( m_panel23 );
	m_splitter5->SplitVertically( m_panel22, m_panel23, 167 );
	bSizer29->Add( m_splitter5, 1, wxEXPAND, 5 );
	
	geometriesPanel->SetSizer( bSizer29 );
	geometriesPanel->Layout();
	bSizer29->Fit( geometriesPanel );
	notebook->AddPage( geometriesPanel, wxT("Geometries"), true );
	renderPanel = new wxPanel( notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	notebook->AddPage( renderPanel, wxT("Rendering"), false );
	
	sizer->Add( notebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( sizer );
	this->Layout();
	
	// Connect Events
	frameList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onFrameSelectionChanged ), NULL, this );
	geometryList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onGeometrySelectionChanged ), NULL, this );
	geometryMaterialsList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onGeometryMaterialSelectionChanged ), NULL, this );
	geometryMaterialTextureList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onGeometryMaterialTextureSelectionChanged ), NULL, this );
	geometryPartsList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onGeometryPartSelectionChanged ), NULL, this );
}

DFFPanelPrototype::~DFFPanelPrototype()
{
	// Disconnect Events
	frameList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onFrameSelectionChanged ), NULL, this );
	geometryList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onGeometrySelectionChanged ), NULL, this );
	geometryMaterialsList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onGeometryMaterialSelectionChanged ), NULL, this );
	geometryMaterialTextureList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onGeometryMaterialTextureSelectionChanged ), NULL, this );
	geometryPartsList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( DFFPanelPrototype::onGeometryPartSelectionChanged ), NULL, this );
}

AboutFramePrototype::AboutFramePrototype( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	sizer = new wxBoxSizer( wxVERTICAL );
	
	titleLabel = new wxStaticText( this, wxID_ANY, wxT("GTATools GUI Viewer"), wxDefaultPosition, wxDefaultSize, 0 );
	titleLabel->Wrap( -1 );
	titleLabel->SetFont( wxFont( 20, 70, 90, 92, false, wxEmptyString ) );
	
	sizer->Add( titleLabel, 0, wxALIGN_CENTER|wxALL, 5 );
	
	internalTitleLabel = new wxStaticText( this, wxID_ANY, wxT("(gtatools-gui)"), wxDefaultPosition, wxDefaultSize, 0 );
	internalTitleLabel->Wrap( -1 );
	internalTitleLabel->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	sizer->Add( internalTitleLabel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	versionLabel = new wxStaticText( this, wxID_ANY, wxT("Version X.X"), wxDefaultPosition, wxDefaultSize, 0 );
	versionLabel->Wrap( -1 );
	versionLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	sizer->Add( versionLabel, 0, wxALL, 5 );
	
	authorLabel = new wxStaticText( this, wxID_ANY, wxT("Developed by Alemarius Nexus"), wxDefaultPosition, wxDefaultSize, 0 );
	authorLabel->Wrap( -1 );
	authorLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	sizer->Add( authorLabel, 0, wxALL, 5 );
	
	licenseLabel = new wxStaticText( this, wxID_ANY, wxT("Released under the terms of the GNU General Public License"), wxDefaultPosition, wxDefaultSize, 0 );
	licenseLabel->Wrap( -1 );
	licenseLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );
	
	sizer->Add( licenseLabel, 0, wxALL, 5 );
	
	this->SetSizer( sizer );
	this->Layout();
	sizer->Fit( this );
}

AboutFramePrototype::~AboutFramePrototype()
{
}
