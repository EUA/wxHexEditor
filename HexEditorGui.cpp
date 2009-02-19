///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif //WX_PRECOMP

#include "HexEditorGui.h"

///////////////////////////////////////////////////////////////////////////

HexEditorGui::HexEditorGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	mbar = new wxMenuBar( 0 );
	fileMenu = new wxMenu();
	wxMenuItem* menuFileOpen;
	menuFileOpen = new wxMenuItem( fileMenu, wxID_OPEN, wxString( wxT("&Open") ) + wxT('\t') + wxT("CTRL+O"), wxT("Open a file"), wxITEM_NORMAL );
	fileMenu->Append( menuFileOpen );
	
	wxMenuItem* menuFileSave;
	menuFileSave = new wxMenuItem( fileMenu, wxID_SAVE, wxString( wxT("&Save") ) + wxT('\t') + wxT("CTRL+S"), wxT("Save modifications"), wxITEM_NORMAL );
	fileMenu->Append( menuFileSave );
	menuFileSave->Enable( false );
	
	wxMenuItem* menuFileSaveAs;
	menuFileSaveAs = new wxMenuItem( fileMenu, wxID_SAVEAS, wxString( wxT("Save &As") ) + wxT('\t') + wxT("SHIFT+CTRL+S"), wxT("Save current file as..."), wxITEM_NORMAL );
	fileMenu->Append( menuFileSaveAs );
	menuFileSaveAs->Enable( false );
	
	wxMenuItem* menuFileClose;
	menuFileClose = new wxMenuItem( fileMenu, idClose, wxString( wxT("&Close") ) + wxT('\t') + wxT("CTRL+W"), wxT("Close current file"), wxITEM_NORMAL );
	fileMenu->Append( menuFileClose );
	menuFileClose->Enable( false );
	
	fileMenu->AppendSeparator();
	
	wxMenuItem* menuFileQuit;
	menuFileQuit = new wxMenuItem( fileMenu, wxID_CLOSE, wxString( wxT("&Quit") ) + wxT('\t') + wxT("CTRL+Q"), wxT("Quit the application"), wxITEM_NORMAL );
	fileMenu->Append( menuFileQuit );
	
	mbar->Append( fileMenu, wxT("&File") );
	
	editMenu = new wxMenu();
	wxMenuItem* menuEditUndo;
	menuEditUndo = new wxMenuItem( editMenu, wxID_UNDO, wxString( wxT("&Undo") ) + wxT('\t') + wxT("CTRL+Z"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditUndo );
	menuEditUndo->Enable( false );
	
	wxMenuItem* menuEditRedo;
	menuEditRedo = new wxMenuItem( editMenu, wxID_REDO, wxString( wxT("&Redo") ) + wxT('\t') + wxT("CTRL+Y"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditRedo );
	menuEditRedo->Enable( false );
	
	editMenu->AppendSeparator();
	
	wxMenuItem* menuEditCopy;
	menuEditCopy = new wxMenuItem( editMenu, wxID_COPY, wxString( wxT("Copy") ) + wxT('\t') + wxT("CTRL+C"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditCopy );
	menuEditCopy->Enable( false );
	
	wxMenuItem* menuEditCut;
	menuEditCut = new wxMenuItem( editMenu, wxID_CUT, wxString( wxT("Cut") ) + wxT('\t') + wxT("CTRL+X"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditCut );
	menuEditCut->Enable( false );
	
	wxMenuItem* menuEditPaste;
	menuEditPaste = new wxMenuItem( editMenu, wxID_PASTE, wxString( wxT("Paste") ) + wxT('\t') + wxT("CTRL+V"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditPaste );
	menuEditPaste->Enable( false );
	
	editMenu->AppendSeparator();
	
	wxMenuItem* menuEditFind;
	menuEditFind = new wxMenuItem( editMenu, wxID_FIND, wxString( wxT("Find") ) + wxT('\t') + wxT("CTRL+F"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditFind );
	menuEditFind->Enable( false );
	
	wxMenuItem* menuEditReplace;
	menuEditReplace = new wxMenuItem( editMenu, wxID_REPLACE, wxString( wxT("Replace") ) + wxT('\t') + wxT("CTRL+R"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditReplace );
	menuEditReplace->Enable( false );
	
	wxMenuItem* menuEditGotooffset;
	menuEditGotooffset = new wxMenuItem( editMenu, idGotoOffset, wxString( wxT("Goto Offset") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditGotooffset );
	menuEditGotooffset->Enable( false );
	
	mbar->Append( editMenu, wxT("&Edit") );
	
	viewMenu = new wxMenu();
	wxMenuItem* menuViewInterprater;
	menuViewInterprater = new wxMenuItem( viewMenu, idInterpreter, wxString( wxT("Data Interpreter") ) , wxEmptyString, wxITEM_CHECK );
	viewMenu->Append( menuViewInterprater );
	
	wxMenuItem* menuViewToolbar;
	menuViewToolbar = new wxMenuItem( viewMenu, idToolbar, wxString( wxT("Toolbar") ) , wxEmptyString, wxITEM_CHECK );
	viewMenu->Append( menuViewToolbar );
	
	wxMenuItem* menuViewInfopanel;
	menuViewInfopanel = new wxMenuItem( viewMenu, idInfoPanel, wxString( wxT("InfoPanel") ) , wxEmptyString, wxITEM_CHECK );
	viewMenu->Append( menuViewInfopanel );
	
	mbar->Append( viewMenu, wxT("&View") );
	
	optionsMenu = new wxMenu();
	menuOptionsFileMode = new wxMenu();
	wxMenuItem* menuOptionsFileModeRO;
	menuOptionsFileModeRO = new wxMenuItem( menuOptionsFileMode, idFileRO, wxString( wxT("Read Only") ) , wxEmptyString, wxITEM_RADIO );
	menuOptionsFileMode->Append( menuOptionsFileModeRO );
	menuOptionsFileModeRO->Enable( false );
	menuOptionsFileModeRO->Check( true );
	
	wxMenuItem* menuOptionsFileModeRW;
	menuOptionsFileModeRW = new wxMenuItem( menuOptionsFileMode, idFileRW, wxString( wxT("Writeable") ) , wxEmptyString, wxITEM_RADIO );
	menuOptionsFileMode->Append( menuOptionsFileModeRW );
	menuOptionsFileModeRW->Enable( false );
	
	wxMenuItem* menuOptionsFileModeDW;
	menuOptionsFileModeDW = new wxMenuItem( menuOptionsFileMode, idFileDW, wxString( wxT("Direct Write") ) , wxEmptyString, wxITEM_RADIO );
	menuOptionsFileMode->Append( menuOptionsFileModeDW );
	menuOptionsFileModeDW->Enable( false );
	
	optionsMenu->Append( -1, wxT("File Mode"), menuOptionsFileMode );
	
	wxMenuItem* menuOptionsPreferences;
	menuOptionsPreferences = new wxMenuItem( optionsMenu, wxID_PREFERENCES, wxString( wxT("Preferences") ) , wxEmptyString, wxITEM_NORMAL );
	optionsMenu->Append( menuOptionsPreferences );
	menuOptionsPreferences->Enable( false );
	
	mbar->Append( optionsMenu, wxT("Options") );
	
	helpMenu = new wxMenu();
	wxMenuItem* menuHelpAbout;
	menuHelpAbout = new wxMenuItem( helpMenu, wxID_ABOUT, wxString( wxT("&About") ) + wxT('\t') + wxT("F1"), wxT("Show info about this application"), wxITEM_NORMAL );
	helpMenu->Append( menuHelpAbout );
	
	mbar->Append( helpMenu, wxT("&Help") );
	
	this->SetMenuBar( mbar );
	
	statusBar = this->CreateStatusBar( 5, wxST_SIZEGRIP, wxID_ANY );
	statusBar->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HexEditorGui::OnClose ) );
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( HexEditorGui::OnKeyDown ) );
	this->Connect( menuFileOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileOpen ) );
	this->Connect( menuFileSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileSave ) );
	this->Connect( menuFileSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileSaveAs ) );
	this->Connect( menuFileClose->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileClose ) );
	this->Connect( menuFileQuit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnQuit ) );
	this->Connect( menuEditUndo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditUndo ) );
	this->Connect( menuEditRedo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditRedo ) );
	this->Connect( menuEditCopy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditCopy ) );
	this->Connect( menuEditCut->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditCut ) );
	this->Connect( menuEditPaste->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditPaste ) );
	this->Connect( menuEditFind->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditFind ) );
	this->Connect( menuEditReplace->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditReplace ) );
	this->Connect( menuEditGotooffset->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditGoto ) );
	this->Connect( menuViewInterprater->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewToolbar->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewInfopanel->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuOptionsFileModeRO->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnOptionsFileMode ) );
	this->Connect( menuOptionsFileModeRW->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnOptionsFileMode ) );
	this->Connect( menuOptionsFileModeDW->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnOptionsFileMode ) );
	this->Connect( menuHelpAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnAbout ) );
}

HexEditorGui::~HexEditorGui()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HexEditorGui::OnClose ) );
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( HexEditorGui::OnKeyDown ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileOpen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileSave ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileSaveAs ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileClose ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnQuit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditUndo ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditRedo ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditCopy ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditCut ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditPaste ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditFind ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditReplace ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnEditGoto ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnOptionsFileMode ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnOptionsFileMode ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnOptionsFileMode ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnAbout ) );
}

InterpreterGui::InterpreterGui( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* optionSizer;
	optionSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_check_unsigned = new wxCheckBox( this, ID_CHK_UNSIGNED, wxT("Unsigned"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_check_unsigned->SetFont( wxFont( 8, 74, 90, 90, false, wxT("Sans") ) );
	
	optionSizer->Add( m_check_unsigned, 0, wxALL, 5 );
	
	m_check_bigendian = new wxCheckBox( this, ID_CHK_BIGENDIAN, wxT("Big Endian"), wxDefaultPosition, wxDefaultSize, 0 );
	
	m_check_bigendian->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	optionSizer->Add( m_check_bigendian, 0, wxALL, 5 );
	
	mainSizer->Add( optionSizer, 0, wxEXPAND, 4 );
	
	wxFlexGridSizer* numSizer;
	numSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	numSizer->AddGrowableCol( 1 );
	numSizer->SetFlexibleDirection( wxHORIZONTAL );
	
	m_static_8bit = new wxStaticText( this, ID_DEFAULT, wxT("8 bit"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_8bit->Wrap( -1 );
	m_static_8bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_8bit, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_textctrl_8bit = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_READONLY );
	m_textctrl_8bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_8bit, 0, wxALL|wxEXPAND, 1 );
	
	m_static_16bit = new wxStaticText( this, ID_DEFAULT, wxT("16 bit"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_16bit->Wrap( -1 );
	m_static_16bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_16bit, 0, wxALIGN_CENTER|wxALL, 0 );
	
	m_textctrl_16bit = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_READONLY );
	m_textctrl_16bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_16bit, 0, wxALL|wxEXPAND, 1 );
	
	m_static_32bit = new wxStaticText( this, ID_DEFAULT, wxT("32 bit"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_32bit->Wrap( -1 );
	m_static_32bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_32bit, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_textctrl_32bit = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_textctrl_32bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_32bit, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 1 );
	
	m_static_64bit = new wxStaticText( this, ID_DEFAULT, wxT("64 bit"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_64bit->Wrap( -1 );
	m_static_64bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_64bit, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_textctrl_64bit = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_textctrl_64bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_64bit, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 1 );
	
	m_static_float = new wxStaticText( this, ID_DEFAULT, wxT("Float"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_float->Wrap( -1 );
	m_static_float->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_float, 0, wxALIGN_CENTER|wxALL, 2 );
	
	m_textctrl_float = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_textctrl_float->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_float, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 1 );
	
	m_static_double = new wxStaticText( this, ID_DEFAULT, wxT("Double"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_double->Wrap( -1 );
	m_static_double->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_double, 0, wxALIGN_CENTER|wxBOTTOM|wxTOP, 3 );
	
	m_textctrl_double = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_textctrl_double->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_double, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 1 );
	
	mainSizer->Add( numSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_check_unsigned->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( InterpreterGui::OnUpdate ), NULL, this );
	m_check_bigendian->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( InterpreterGui::OnUpdate ), NULL, this );
}

InterpreterGui::~InterpreterGui()
{
	// Disconnect Events
	m_check_unsigned->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( InterpreterGui::OnUpdate ), NULL, this );
	m_check_bigendian->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( InterpreterGui::OnUpdate ), NULL, this );
}

InfoPanelGui::InfoPanelGui( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* nameSizer;
	nameSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_static_name = new wxStaticText( this, wxID_ANY, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_name->Wrap( -1 );
	nameSizer->Add( m_static_name, 0, wxALL, 2 );
	
	m_name = new wxStaticText( this, wxID_ANY, wxT("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
	m_name->Wrap( -1 );
	nameSizer->Add( m_name, 1, wxBOTTOM|wxTOP, 2 );
	
	mainSizer->Add( nameSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* pathSizer;
	pathSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_static_path = new wxStaticText( this, wxID_ANY, wxT("Path:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_path->Wrap( -1 );
	pathSizer->Add( m_static_path, 0, wxALL, 2 );
	
	m_path = new wxStaticText( this, wxID_ANY, wxT("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
	m_path->Wrap( -1 );
	pathSizer->Add( m_path, 1, wxALL|wxEXPAND, 2 );
	
	mainSizer->Add( pathSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* sizeSizer;
	sizeSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_static_size = new wxStaticText( this, wxID_ANY, wxT("Size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_size->Wrap( -1 );
	sizeSizer->Add( m_static_size, 0, wxALL, 2 );
	
	m_size = new wxStaticText( this, wxID_ANY, wxT("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
	m_size->Wrap( -1 );
	sizeSizer->Add( m_size, 0, wxALL, 2 );
	
	mainSizer->Add( sizeSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* accessSizer;
	accessSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_static_access = new wxStaticText( this, wxID_ANY, wxT("Access:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_access->Wrap( -1 );
	accessSizer->Add( m_static_access, 0, wxALL, 2 );
	
	m_access = new wxStaticText( this, wxID_ANY, wxT("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
	m_access->Wrap( -1 );
	accessSizer->Add( m_access, 0, wxALL, 2 );
	
	mainSizer->Add( accessSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* deviceSizer;
	deviceSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_static_device = new wxStaticText( this, wxID_ANY, wxT("Device:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_device->Wrap( -1 );
	deviceSizer->Add( m_static_device, 0, wxALL, 2 );
	
	m_device = new wxStaticText( this, wxID_ANY, wxT("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
	m_device->Wrap( -1 );
	deviceSizer->Add( m_device, 0, wxALL, 2 );
	
	mainSizer->Add( deviceSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

InfoPanelGui::~InfoPanelGui()
{
}

GotoDialogGui::GotoDialogGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerTop;
	bSizerTop = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticTextOffset = new wxStaticText( this, wxID_ANY, wxT("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticTextOffset->Wrap( -1 );
	bSizerTop->Add( m_staticTextOffset, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textCtrlOffset = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RIGHT );
	bSizerTop->Add( m_textCtrlOffset, 1, wxALIGN_CENTER|wxALL, 5 );
	
	m_dec = new wxRadioButton( this, wxID_ANY, wxT("Decimal"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTop->Add( m_dec, 0, wxALL, 5 );
	
	m_hex = new wxRadioButton( this, wxID_ANY, wxT("Hex"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTop->Add( m_hex, 0, wxALL, 5 );
	
	mainSizer->Add( bSizerTop, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerBottom;
	bSizerBottom = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_branchChoices[] = { wxT("From beginning"), wxT("From here"), wxT("From end") };
	int m_branchNChoices = sizeof( m_branchChoices ) / sizeof( wxString );
	m_branch = new wxRadioBox( this, wxID_ANY, wxT("Type of branch"), wxDefaultPosition, wxDefaultSize, m_branchNChoices, m_branchChoices, 1, wxRA_SPECIFY_ROWS );
	m_branch->SetSelection( 0 );
	bSizerBottom->Add( m_branch, 0, wxALL, 5 );
	
	m_button_go = new wxButton( this, wxID_ANY, wxT("Go!"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button_go->SetDefault(); 
	bSizerBottom->Add( m_button_go, 1, wxALIGN_CENTER|wxTOP, 5 );
	
	mainSizer->Add( bSizerBottom, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_textCtrlOffset->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GotoDialogGui::OnInput ), NULL, this );
	m_textCtrlOffset->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( GotoDialogGui::OnGo ), NULL, this );
	m_dec->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GotoDialogGui::OnConvert ), NULL, this );
	m_hex->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GotoDialogGui::OnConvert ), NULL, this );
	m_button_go->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GotoDialogGui::OnGo ), NULL, this );
}

GotoDialogGui::~GotoDialogGui()
{
	// Disconnect Events
	m_textCtrlOffset->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GotoDialogGui::OnInput ), NULL, this );
	m_textCtrlOffset->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( GotoDialogGui::OnGo ), NULL, this );
	m_dec->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GotoDialogGui::OnConvert ), NULL, this );
	m_hex->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GotoDialogGui::OnConvert ), NULL, this );
	m_button_go->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GotoDialogGui::OnGo ), NULL, this );
}

FindDialogGui::FindDialogGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizerTop;
	fgSizerTop = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerTop->AddGrowableCol( 1 );
	fgSizerTop->SetFlexibleDirection( wxBOTH );
	fgSizerTop->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_static_search = new wxStaticText( this, wxID_ANY, wxT("Search: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_search->Wrap( -1 );
	fgSizerTop->Add( m_static_search, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textSearch = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RIGHT );
	fgSizerTop->Add( m_textSearch, 0, wxALIGN_CENTER|wxBOTTOM|wxEXPAND|wxTOP, 5 );
	
	m_static_replace = new wxStaticText( this, wxID_ANY, wxT("Replace: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_replace->Wrap( -1 );
	m_static_replace->Hide();
	
	fgSizerTop->Add( m_static_replace, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_textReplace = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER|wxTE_PROCESS_TAB|wxTE_RIGHT );
	m_textReplace->Hide();
	
	fgSizerTop->Add( m_textReplace, 0, wxALIGN_CENTER|wxEXPAND, 5 );
	
	mainSizer->Add( fgSizerTop, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerBottom;
	bSizerBottom = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_searchtypeChoices[] = { wxT("Text"), wxT("Hex") };
	int m_searchtypeNChoices = sizeof( m_searchtypeChoices ) / sizeof( wxString );
	m_searchtype = new wxRadioBox( this, wxID_ANY, wxT("Search as"), wxDefaultPosition, wxDefaultSize, m_searchtypeNChoices, m_searchtypeChoices, 1, wxRA_SPECIFY_COLS );
	m_searchtype->SetSelection( 0 );
	bSizerBottom->Add( m_searchtype, 0, wxALL, 5 );
	
	wxString m_fromChoices[] = { wxT("Beginning"), wxT("Cursor") };
	int m_fromNChoices = sizeof( m_fromChoices ) / sizeof( wxString );
	m_from = new wxRadioBox( this, wxID_ANY, wxT("Search from"), wxDefaultPosition, wxDefaultSize, m_fromNChoices, m_fromChoices, 1, wxRA_SPECIFY_COLS );
	m_from->SetSelection( 1 );
	bSizerBottom->Add( m_from, 0, wxALL, 5 );
	
	m_button = new wxButton( this, wxID_ANY, wxT("Find"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button->SetDefault(); 
	bSizerBottom->Add( m_button, 1, wxALIGN_CENTER|wxTOP, 5 );
	
	mainSizer->Add( bSizerBottom, 0, wxEXPAND, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	mainSizer->Fit( this );
	
	// Connect Events
	m_textSearch->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindDialogGui::OnGo ), NULL, this );
	m_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::OnButton ), NULL, this );
}

FindDialogGui::~FindDialogGui()
{
	// Disconnect Events
	m_textSearch->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindDialogGui::OnGo ), NULL, this );
	m_button->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::OnButton ), NULL, this );
}
