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
	menuFileOpen = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("&Open") ) , wxEmptyString, wxITEM_NORMAL );
	fileMenu->Append( menuFileOpen );
	
	wxMenuItem* menuFileSave;
	menuFileSave = new wxMenuItem( fileMenu, wxID_ANY, wxString( wxT("&Save") ) , wxEmptyString, wxITEM_NORMAL );
	fileMenu->Append( menuFileSave );
	menuFileSave->Enable( false );
	
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
	menuEditUndo = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("&Undo") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditUndo );
	menuEditUndo->Enable( false );
	
	wxMenuItem* menuEditRedo;
	menuEditRedo = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("&Redo") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditRedo );
	menuEditRedo->Enable( false );
	
	editMenu->AppendSeparator();
	
	wxMenuItem* menuEditCopy;
	menuEditCopy = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Copy") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditCopy );
	menuEditCopy->Enable( false );
	
	wxMenuItem* menuEditCut;
	menuEditCut = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Cut") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditCut );
	menuEditCut->Enable( false );
	
	wxMenuItem* menuEditPaste;
	menuEditPaste = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Paste") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditPaste );
	menuEditPaste->Enable( false );
	
	editMenu->AppendSeparator();
	
	wxMenuItem* menuEditFind;
	menuEditFind = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Find") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditFind );
	menuEditFind->Enable( false );
	
	wxMenuItem* menuEditGotobyte;
	menuEditGotobyte = new wxMenuItem( editMenu, wxID_ANY, wxString( wxT("Goto Byte") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditGotobyte );
	menuEditGotobyte->Enable( false );
	
	mbar->Append( editMenu, wxT("&Edit") );
	
	viewMenu = new wxMenu();
	wxMenuItem* menuViewInterprator;
	menuViewInterprator = new wxMenuItem( viewMenu, wxID_ANY, wxString( wxT("Interpretor") ) , wxEmptyString, wxITEM_CHECK );
	viewMenu->Append( menuViewInterprator );
	menuViewInterprator->Enable( false );
	
	mbar->Append( viewMenu, wxT("&View") );
	
	helpMenu = new wxMenu();
	wxMenuItem* menuHelpAbout;
	menuHelpAbout = new wxMenuItem( helpMenu, idMenuAbout, wxString( wxT("&About") ) + wxT('\t') + wxT("F1"), wxT("Show info about this application"), wxITEM_NORMAL );
	helpMenu->Append( menuHelpAbout );
	
	mbar->Append( helpMenu, wxT("&Help") );
	
	this->SetMenuBar( mbar );
	
	statusBar = this->CreateStatusBar( 2, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HexEditorGui::OnClose ) );
	this->Connect( menuFileOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileOpen ) );
	this->Connect( menuFileSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileSave ) );
	this->Connect( menuFileClose->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnFileClose ) );
	this->Connect( menuFileQuit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnQuit ) );
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
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnAbout ) );
}
