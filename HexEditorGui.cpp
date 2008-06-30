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
	menuFileOpen = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("&Open") ) + wxT('\t') + wxT("CTRL+O"), wxEmptyString, wxITEM_NORMAL );
	fileMenu->Append( menuFileOpen );
	
	wxMenuItem* menuFileSave;
	menuFileSave = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("&Save") ) + wxT('\t') + wxT("CTRL+S"), wxEmptyString, wxITEM_NORMAL );
	fileMenu->Append( menuFileSave );
	
	wxMenuItem* menuFileClose;
	menuFileClose = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("&Close") ) , wxEmptyString, wxITEM_NORMAL );
	fileMenu->Append( menuFileClose );
	
	fileMenu->AppendSeparator();
	
	wxMenuItem* menuFileQuit;
	menuFileQuit = new wxMenuItem( fileMenu, idMenuQuit, wxString( wxT("&Quit") ) + wxT('\t') + wxT("Alt+F4"), wxT("Quit the application"), wxITEM_NORMAL );
	fileMenu->Append( menuFileQuit );
	
	mbar->Append( fileMenu, wxT("&File") );
	
	editMenu = new wxMenu();
	wxMenuItem* menuEditUndo;
	menuEditUndo = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("&Undo") ) + wxT('\t') + wxT("CTRL+Z"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditUndo );
	menuEditUndo->Enable( false );
	
	wxMenuItem* menuEditRedo;
	menuEditRedo = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("&Redo") ) + wxT('\t') + wxT("CTRL+Y"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditRedo );
	menuEditRedo->Enable( false );
	
	editMenu->AppendSeparator();
	
	wxMenuItem* menuEditCopy;
	menuEditCopy = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Copy") ) + wxT('\t') + wxT("CTRL+C"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditCopy );
	menuEditCopy->Enable( false );
	
	wxMenuItem* menuEditCut;
	menuEditCut = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Cut") ) + wxT('\t') + wxT("CTRL+X"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditCut );
	menuEditCut->Enable( false );
	
	wxMenuItem* menuEditPaste;
	menuEditPaste = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Paste") ) + wxT('\t') + wxT("CTRL+V"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditPaste );
	menuEditPaste->Enable( false );
	
	editMenu->AppendSeparator();
	
	wxMenuItem* menuEditFind;
	menuEditFind = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Find") ) + wxT('\t') + wxT("CTRL+F"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditFind );
	menuEditFind->Enable( false );
	
	wxMenuItem* menuEditReplace;
	menuEditReplace = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Replace") ) + wxT('\t') + wxT("CTRL+R"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditReplace );
	
	wxMenuItem* menuEditGotobyte;
	menuEditGotobyte = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Goto Byte") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditGotobyte );
	menuEditGotobyte->Enable( false );
	
	mbar->Append( editMenu, wxT("&Edit") );
	
	viewMenu = new wxMenu();
	wxMenuItem* menuViewInterprator;
	menuViewInterprator = new wxMenuItem( viewMenu, wxID_INTERPRATOR, wxString( wxT("Interpretor") ) , wxEmptyString, wxITEM_CHECK );
	viewMenu->Append( menuViewInterprator );
	
	mbar->Append( viewMenu, wxT("&View") );
	
	helpMenu = new wxMenu();
	wxMenuItem* menuHelpAbout;
	menuHelpAbout = new wxMenuItem( helpMenu, idMenuAbout, wxString( wxT("&About") ) + wxT('\t') + wxT("F1"), wxT("Show info about this application"), wxITEM_NORMAL );
	helpMenu->Append( menuHelpAbout );
	
	mbar->Append( helpMenu, wxT("&Help") );
	
	this->SetMenuBar( mbar );
	
	statusBar = this->CreateStatusBar( 5, wxST_SIZEGRIP, wxID_ANY );
	statusBar->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HexEditorGui::OnClose ) );
	this->Connect( menuFileOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileOpen ) );
	this->Connect( menuFileSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileSave ) );
	this->Connect( menuFileClose->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileClose ) );
	this->Connect( menuFileQuit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnQuit ) );
	this->Connect( menuViewInterprator->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewInterpretor ) );
	this->Connect( menuHelpAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnAbout ) );
}

HexEditorGui::~HexEditorGui()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HexEditorGui::OnClose ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileOpen ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileSave ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileClose ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnQuit ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewInterpretor ) );
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
	mainSizer->Fit( this );
	
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
