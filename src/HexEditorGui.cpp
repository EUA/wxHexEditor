///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
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
	wxMenuItem* menuFileNew;
	menuFileNew = new wxMenuItem( fileMenu, wxID_NEW, wxString( wxT("&New") ) + wxT('\t') + wxT("CTRL+N"), wxT("Creates a new file"), wxITEM_NORMAL );
	fileMenu->Append( menuFileNew );
	
	wxMenuItem* menuFileOpen;
	menuFileOpen = new wxMenuItem( fileMenu, wxID_OPEN, wxString( wxT("&Open") ) + wxT('\t') + wxT("CTRL+O"), wxT("Open a file"), wxITEM_NORMAL );
	fileMenu->Append( menuFileOpen );
	
	menuFileOpenRecent = new wxMenu();
	wxMenuItem* menuFileOpenRecentDummy;
	menuFileOpenRecentDummy = new wxMenuItem( menuFileOpenRecent, wxID_ANY, wxString( wxT("No Recent File") ) , wxEmptyString, wxITEM_NORMAL );
	menuFileOpenRecent->Append( menuFileOpenRecentDummy );
	menuFileOpenRecentDummy->Enable( false );
	
	fileMenu->Append( -1, wxT("Open &Recent"), menuFileOpenRecent );
	
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
	
	wxMenuItem* m_separator1;
	m_separator1 = fileMenu->AppendSeparator();
	
	wxMenuItem* menuFileQuit;
	menuFileQuit = new wxMenuItem( fileMenu, wxID_CLOSE, wxString( wxT("&Quit") ) + wxT('\t') + wxT("CTRL+Q"), wxT("Quit the application"), wxITEM_NORMAL );
	fileMenu->Append( menuFileQuit );
	
	mbar->Append( fileMenu, wxT("&File") ); 
	
	editMenu = new wxMenu();
	wxMenuItem* menuEditUndo;
	menuEditUndo = new wxMenuItem( editMenu, wxID_UNDO, wxString( wxT("&Undo") ) + wxT('\t') + wxT("CTRL+Z"), wxT("Undo last operation"), wxITEM_NORMAL );
	editMenu->Append( menuEditUndo );
	menuEditUndo->Enable( false );
	
	wxMenuItem* menuEditRedo;
	menuEditRedo = new wxMenuItem( editMenu, wxID_REDO, wxString( wxT("&Redo") ) + wxT('\t') + wxT("CTRL+Y"), wxT("Reverts last undo"), wxITEM_NORMAL );
	editMenu->Append( menuEditRedo );
	menuEditRedo->Enable( false );
	
	wxMenuItem* m_separator2;
	m_separator2 = editMenu->AppendSeparator();
	
	wxMenuItem* menuEditCopy;
	menuEditCopy = new wxMenuItem( editMenu, wxID_COPY, wxString( wxT("Copy") ) + wxT('\t') + wxT("CTRL+C"), wxT("Copies selection"), wxITEM_NORMAL );
	editMenu->Append( menuEditCopy );
	menuEditCopy->Enable( false );
	
	wxMenuItem* menuEditCopyAs;
	menuEditCopyAs = new wxMenuItem( editMenu, idCopyAs, wxString( wxT("Copy As") ) + wxT('\t') + wxT("CTRL+SHIFT+C"), wxT("Copies selection in special format"), wxITEM_NORMAL );
	editMenu->Append( menuEditCopyAs );
	menuEditCopyAs->Enable( false );
	
	wxMenuItem* menuEditSaveAsDump;
	menuEditSaveAsDump = new wxMenuItem( editMenu, idSaveAsDump, wxString( wxT("Save As Dump") ) + wxT('\t') + wxT("CTRL+ALT+S"), wxT("Save selections as a file"), wxITEM_NORMAL );
	editMenu->Append( menuEditSaveAsDump );
	menuEditSaveAsDump->Enable( false );
	
	wxMenuItem* menuEditCut;
	menuEditCut = new wxMenuItem( editMenu, wxID_CUT, wxString( wxT("Cut") ) + wxT('\t') + wxT("CTRL+X"), wxT("Cuts a selection"), wxITEM_NORMAL );
	editMenu->Append( menuEditCut );
	menuEditCut->Enable( false );
	
	wxMenuItem* menuEditPaste;
	menuEditPaste = new wxMenuItem( editMenu, wxID_PASTE, wxString( wxT("Paste") ) + wxT('\t') + wxT("CTRL+V"), wxT("Paste clipboard to cursor location"), wxITEM_NORMAL );
	editMenu->Append( menuEditPaste );
	menuEditPaste->Enable( false );
	
	wxMenuItem* menuEditFill;
	menuEditFill = new wxMenuItem( editMenu, idFillSelection, wxString( wxT("Fill Selection") ) , wxT("Fill selection blocks"), wxITEM_NORMAL );
	editMenu->Append( menuEditFill );
	menuEditFill->Enable( false );
	
	wxMenuItem* menuEditDelete;
	menuEditDelete = new wxMenuItem( editMenu, wxID_DELETE, wxString( wxT("Delete") ) , wxT("Deletes selection"), wxITEM_NORMAL );
	editMenu->Append( menuEditDelete );
	menuEditDelete->Enable( false );
	
	wxMenuItem* menuEditInsert;
	menuEditInsert = new wxMenuItem( editMenu, idInsert, wxString( wxT("Insert") ) , wxT("Inserts new bytes to cursor location"), wxITEM_NORMAL );
	editMenu->Append( menuEditInsert );
	menuEditInsert->Enable( false );
	
	wxMenuItem* m_separator3;
	m_separator3 = editMenu->AppendSeparator();
	
	wxMenuItem* menuEditFind;
	menuEditFind = new wxMenuItem( editMenu, wxID_FIND, wxString( wxT("Find") ) + wxT('\t') + wxT("CTRL+F"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditFind );
	menuEditFind->Enable( false );
	
	wxMenuItem* menuEditReplace;
	menuEditReplace = new wxMenuItem( editMenu, wxID_REPLACE, wxString( wxT("Replace") ) + wxT('\t') + wxT("CTRL+R"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditReplace );
	menuEditReplace->Enable( false );
	
	wxMenuItem* menuEditGotooffset;
	menuEditGotooffset = new wxMenuItem( editMenu, idGotoOffset, wxString( wxT("Go to Offset") ) + wxT('\t') + wxT("CTRL+G"), wxT("Branhes the given location"), wxITEM_NORMAL );
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
	
	wxMenuItem* menuViewTagPanel;
	menuViewTagPanel = new wxMenuItem( viewMenu, idTagPanel, wxString( wxT("TAG Panel") ) , wxEmptyString, wxITEM_CHECK );
	viewMenu->Append( menuViewTagPanel );
	
	wxMenuItem* menuViewDisassemblerPanel;
	menuViewDisassemblerPanel = new wxMenuItem( viewMenu, idDisassemblerPanel, wxString( wxT("Disassembler Panel") ) , wxEmptyString, wxITEM_CHECK );
	viewMenu->Append( menuViewDisassemblerPanel );
	
	mbar->Append( viewMenu, wxT("&View") ); 
	
	toolsMenu = new wxMenu();
	wxMenuItem* menuToolChecksum;
	menuToolChecksum = new wxMenuItem( toolsMenu, idChecksum, wxString( wxT("Calculate Checksum") ) , wxEmptyString, wxITEM_NORMAL );
	toolsMenu->Append( menuToolChecksum );
	
	wxMenuItem* menuToolCompare;
	menuToolCompare = new wxMenuItem( toolsMenu, idCompare, wxString( wxT("Compare Files") ) , wxEmptyString, wxITEM_NORMAL );
	toolsMenu->Append( menuToolCompare );
	
	wxMenuItem* menuToolsXORView;
	menuToolsXORView = new wxMenuItem( toolsMenu, idXORView, wxString( wxT("XORView Thru") ) , wxT("View your XOR encrypted file without decrypting!"), wxITEM_CHECK );
	toolsMenu->Append( menuToolsXORView );
	
	mbar->Append( toolsMenu, wxT("Tools") ); 
	
	devicesMenu = new wxMenu();
	wxMenuItem* menuDevicesRam;
	menuDevicesRam = new wxMenuItem( devicesMenu, idDeviceRam, wxString( wxT("Open RAM Device") ) , wxEmptyString, wxITEM_NORMAL );
	devicesMenu->Append( menuDevicesRam );
	menuDevicesRam->Enable( false );
	
	menuDeviceDisk = new wxMenu();
	wxMenuItem* menuDevicesDiskItem1;
	menuDevicesDiskItem1 = new wxMenuItem( menuDeviceDisk, wxID_ANY, wxString( wxT("N/A on this OS (yet)") ) , wxEmptyString, wxITEM_NORMAL );
	menuDeviceDisk->Append( menuDevicesDiskItem1 );
	
	devicesMenu->Append( -1, wxT("Open Disk Device"), menuDeviceDisk );
	
	mbar->Append( devicesMenu, wxT("Devices") ); 
	
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
	
	wxBoxSizer* FrameSizer;
	FrameSizer = new wxBoxSizer( wxVERTICAL );
	
	MyNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_SCROLL_BUTTONS|wxAUI_NB_TAB_MOVE|wxAUI_NB_TAB_SPLIT|wxAUI_NB_WINDOWLIST_BUTTON );
	
	FrameSizer->Add( MyNotebook, 1, wxEXPAND | wxALL, 5 );
	
	this->SetSizer( FrameSizer );
	this->Layout();
	FrameSizer->Fit( this );
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HexEditorGui::OnClose ) );
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( HexEditorGui::OnKeyDown ) );
	this->Connect( menuFileNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileOpenRecentDummy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileClose->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileQuit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnQuit ) );
	this->Connect( menuEditUndo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditRedo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditCopy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditCopyAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditSaveAsDump->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditCut->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditPaste->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditFill->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditDelete->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditInsert->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditFind->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditReplace->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditGotooffset->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuViewInterprater->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewInterprater->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuViewToolbar->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewInfopanel->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewTagPanel->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewDisassemblerPanel->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewDisassemblerPanel->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuToolChecksum->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
	this->Connect( menuToolCompare->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
	this->Connect( menuToolsXORView->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
	this->Connect( menuToolsXORView->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuDevicesRam->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnDevicesMenu ) );
	this->Connect( menuDevicesRam->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuDevicesDiskItem1->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnDevicesMenu ) );
	this->Connect( menuOptionsFileModeRO->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuOptionsFileModeRO->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuOptionsFileModeRW->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuOptionsFileModeRW->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuOptionsFileModeDW->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuOptionsFileModeDW->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuHelpAbout->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnAbout ) );
}

HexEditorGui::~HexEditorGui()
{
	// Disconnect Events
	this->Disconnect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HexEditorGui::OnClose ) );
	this->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( HexEditorGui::OnKeyDown ) );
	this->Disconnect( wxID_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_SAVEAS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idClose, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_CLOSE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnQuit ) );
	this->Disconnect( wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idCopyAs, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idSaveAsDump, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idFillSelection, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idInsert, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_FIND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_REPLACE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idGotoOffset, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idInterpreter, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( idInterpreter, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( idToolbar, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( idInfoPanel, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( idTagPanel, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( idDisassemblerPanel, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( idDisassemblerPanel, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( idChecksum, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
	this->Disconnect( idCompare, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
	this->Disconnect( idXORView, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
	this->Disconnect( idXORView, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( idDeviceRam, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnDevicesMenu ) );
	this->Disconnect( idDeviceRam, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnDevicesMenu ) );
	this->Disconnect( idFileRO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idFileRO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( idFileRW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idFileRW, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( idFileDW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idFileDW, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( wxID_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnAbout ) );
	
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
	
	m_static_bin = new wxStaticText( this, ID_DEFAULT, wxT("Binary"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_bin->Wrap( -1 );
	m_static_bin->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_bin, 0, wxALIGN_CENTER, 2 );
	
	wxBoxSizer* binSizer;
	binSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_textctrl_binary = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	m_textctrl_binary->SetMaxLength( 8 ); 
	m_textctrl_binary->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	m_textctrl_binary->SetToolTip( wxT("Press enter to make changes!") );
	
	binSizer->Add( m_textctrl_binary, 0, wxALL|wxEXPAND, 1 );
	
	m_check_edit = new wxCheckBox( this, wxID_ANY, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	m_check_edit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	m_check_edit->SetToolTip( wxT("Allow editing by Data Interpreter Panel") );
	
	binSizer->Add( m_check_edit, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT|wxALL, 1 );
	
	numSizer->Add( binSizer, 1, wxEXPAND, 5 );
	
	m_static_8bit = new wxStaticText( this, ID_DEFAULT, wxT("8 bit"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_8bit->Wrap( -1 );
	m_static_8bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_8bit, 0, wxALIGN_CENTER, 0 );
	
	m_textctrl_8bit = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_READONLY );
	m_textctrl_8bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_8bit, 0, wxALL|wxEXPAND, 1 );
	
	m_static_16bit = new wxStaticText( this, ID_DEFAULT, wxT("16 bit"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_16bit->Wrap( -1 );
	m_static_16bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_16bit, 0, wxALIGN_CENTER, 0 );
	
	m_textctrl_16bit = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxSize( -1,-1 ), wxTE_READONLY );
	m_textctrl_16bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_16bit, 0, wxALL|wxEXPAND, 1 );
	
	m_static_32bit = new wxStaticText( this, ID_DEFAULT, wxT("32 bit"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_32bit->Wrap( -1 );
	m_static_32bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_32bit, 0, wxALIGN_CENTER, 2 );
	
	m_textctrl_32bit = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_textctrl_32bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_32bit, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 1 );
	
	m_static_64bit = new wxStaticText( this, ID_DEFAULT, wxT("64 bit"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_64bit->Wrap( -1 );
	m_static_64bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_64bit, 0, wxALIGN_CENTER, 2 );
	
	m_textctrl_64bit = new wxTextCtrl( this, ID_DEFAULT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_textctrl_64bit->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_textctrl_64bit, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL|wxEXPAND, 1 );
	
	m_static_float = new wxStaticText( this, ID_DEFAULT, wxT("Float"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
	m_static_float->Wrap( -1 );
	m_static_float->SetFont( wxFont( 8, 70, 90, 90, false, wxEmptyString ) );
	
	numSizer->Add( m_static_float, 0, wxALIGN_CENTER, 2 );
	
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
	m_textctrl_binary->Connect( wxEVT_CHAR, wxKeyEventHandler( InterpreterGui::OnTextEdit ), NULL, this );
	m_textctrl_binary->Connect( wxEVT_MIDDLE_DCLICK, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Connect( wxEVT_MIDDLE_DOWN, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Connect( wxEVT_MIDDLE_UP, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Connect( wxEVT_RIGHT_DCLICK, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Connect( wxEVT_RIGHT_UP, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_check_edit->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( InterpreterGui::OnCheckEdit ), NULL, this );
}

InterpreterGui::~InterpreterGui()
{
	// Disconnect Events
	m_check_unsigned->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( InterpreterGui::OnUpdate ), NULL, this );
	m_check_bigendian->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( InterpreterGui::OnUpdate ), NULL, this );
	m_textctrl_binary->Disconnect( wxEVT_CHAR, wxKeyEventHandler( InterpreterGui::OnTextEdit ), NULL, this );
	m_textctrl_binary->Disconnect( wxEVT_MIDDLE_DCLICK, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Disconnect( wxEVT_MIDDLE_DOWN, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Disconnect( wxEVT_MIDDLE_UP, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Disconnect( wxEVT_RIGHT_DCLICK, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_textctrl_binary->Disconnect( wxEVT_RIGHT_UP, wxMouseEventHandler( InterpreterGui::OnTextMouse ), NULL, this );
	m_check_edit->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( InterpreterGui::OnCheckEdit ), NULL, this );
	
}

TagPanelGui::TagPanelGui( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSize;
	mainSize = new wxBoxSizer( wxVERTICAL );
	
	TagPanelList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	mainSize->Add( TagPanelList, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( mainSize );
	this->Layout();
	mainSize->Fit( this );
	
	// Connect Events
	TagPanelList->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( TagPanelGui::OnTagSelect ), NULL, this );
}

TagPanelGui::~TagPanelGui()
{
	// Disconnect Events
	TagPanelList->Disconnect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( TagPanelGui::OnTagSelect ), NULL, this );
	
}

InfoPanelGui::InfoPanelGui( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_InfoPanelText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_InfoPanelText->Wrap( -1 );
	mainSizer->Add( m_InfoPanelText, 0, wxALL, 2 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

InfoPanelGui::~InfoPanelGui()
{
}

DisassemblerPanelGUI::DisassemblerPanelGUI( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerTop;
	bSizerTop = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_choiceVendorChoices[] = { wxT("INTEL"), wxT("AMD") };
	int m_choiceVendorNChoices = sizeof( m_choiceVendorChoices ) / sizeof( wxString );
	m_choiceVendor = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceVendorNChoices, m_choiceVendorChoices, 0 );
	m_choiceVendor->SetSelection( 0 );
	m_choiceVendor->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 90, false, wxEmptyString ) );
	m_choiceVendor->SetToolTip( wxT("CPU Vendor") );
	
	bSizerTop->Add( m_choiceVendor, 1, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxString m_choiceASMTypeChoices[] = { wxT("INTEL"), wxT("AT&T") };
	int m_choiceASMTypeNChoices = sizeof( m_choiceASMTypeChoices ) / sizeof( wxString );
	m_choiceASMType = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceASMTypeNChoices, m_choiceASMTypeChoices, 0 );
	m_choiceASMType->SetSelection( 0 );
	m_choiceASMType->SetToolTip( wxT("Assembly Type") );
	
	bSizerTop->Add( m_choiceASMType, 1, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	wxString m_choiceBitModeChoices[] = { wxT("16Bit"), wxT("32Bit"), wxT("64Bit") };
	int m_choiceBitModeNChoices = sizeof( m_choiceBitModeChoices ) / sizeof( wxString );
	m_choiceBitMode = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choiceBitModeNChoices, m_choiceBitModeChoices, 0 );
	m_choiceBitMode->SetSelection( 1 );
	m_choiceBitMode->SetToolTip( wxT("Disassembly Bit Mode") );
	
	bSizerTop->Add( m_choiceBitMode, 1, wxTOP|wxRIGHT|wxLEFT, 5 );
	
	mainSizer->Add( bSizerTop, 0, wxEXPAND, 5 );
	
	m_dasmCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP|wxTE_MULTILINE|wxTE_READONLY );
	mainSizer->Add( m_dasmCtrl, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	this->SetSizer( mainSizer );
	this->Layout();
	
	// Connect Events
	m_choiceVendor->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DisassemblerPanelGUI::OnUpdate ), NULL, this );
	m_choiceASMType->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DisassemblerPanelGUI::OnUpdate ), NULL, this );
	m_choiceBitMode->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DisassemblerPanelGUI::OnUpdate ), NULL, this );
}

DisassemblerPanelGUI::~DisassemblerPanelGUI()
{
	// Disconnect Events
	m_choiceVendor->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DisassemblerPanelGUI::OnUpdate ), NULL, this );
	m_choiceASMType->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DisassemblerPanelGUI::OnUpdate ), NULL, this );
	m_choiceBitMode->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( DisassemblerPanelGUI::OnUpdate ), NULL, this );
	
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
	
	m_comboBoxOffset = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_comboBoxOffset->Append( wxT(" ") );
	m_comboBoxOffset->Append( wxT(" ") );
	m_comboBoxOffset->Append( wxT(" ") );
	m_comboBoxOffset->Append( wxT(" ") );
	m_comboBoxOffset->Append( wxT(" ") );
	m_comboBoxOffset->Append( wxT(" ") );
	m_comboBoxOffset->Append( wxT(" ") );
	m_comboBoxOffset->Append( wxT(" ") );
	m_comboBoxOffset->Append( wxT(" ") );
	m_comboBoxOffset->Append( wxT(" ") );
	bSizerTop->Add( m_comboBoxOffset, 0, wxALL, 5 );
	
	m_dec = new wxRadioButton( this, wxID_ANY, wxT("Decimal"), wxDefaultPosition, wxDefaultSize, 0 );
	m_dec->SetValue( true ); 
	bSizerTop->Add( m_dec, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_hex = new wxRadioButton( this, wxID_ANY, wxT("Hex"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerTop->Add( m_hex, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
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
	m_comboBoxOffset->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GotoDialogGui::OnInput ), NULL, this );
	m_comboBoxOffset->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( GotoDialogGui::OnGO ), NULL, this );
	m_dec->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GotoDialogGui::OnConvert ), NULL, this );
	m_hex->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GotoDialogGui::OnConvert ), NULL, this );
	m_button_go->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GotoDialogGui::OnGo ), NULL, this );
}

GotoDialogGui::~GotoDialogGui()
{
	// Disconnect Events
	m_comboBoxOffset->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( GotoDialogGui::OnInput ), NULL, this );
	m_comboBoxOffset->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( GotoDialogGui::OnGO ), NULL, this );
	m_dec->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GotoDialogGui::OnConvert ), NULL, this );
	m_hex->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( GotoDialogGui::OnConvert ), NULL, this );
	m_button_go->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( GotoDialogGui::OnGo ), NULL, this );
	
}

FindDialogGui::FindDialogGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 350,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizerLeft;
	fgSizerLeft = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizerLeft->AddGrowableCol( 0 );
	fgSizerLeft->SetFlexibleDirection( wxBOTH );
	fgSizerLeft->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizerBottom;
	bSizerBottom = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizerTop;
	fgSizerTop = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerTop->AddGrowableCol( 1 );
	fgSizerTop->SetFlexibleDirection( wxBOTH );
	fgSizerTop->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_static_search = new wxStaticText( this, wxID_ANY, wxT("Search: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_search->Wrap( -1 );
	fgSizerTop->Add( m_static_search, 0, wxALIGN_CENTER, 5 );
	
	m_comboBoxSearch = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN );
	m_comboBoxSearch->Append( wxT(" ") );
	m_comboBoxSearch->Append( wxT(" ") );
	m_comboBoxSearch->Append( wxT(" ") );
	m_comboBoxSearch->Append( wxT(" ") );
	m_comboBoxSearch->Append( wxT(" ") );
	m_comboBoxSearch->Append( wxT(" ") );
	m_comboBoxSearch->Append( wxT(" ") );
	m_comboBoxSearch->Append( wxT(" ") );
	m_comboBoxSearch->Append( wxT(" ") );
	m_comboBoxSearch->Append( wxT(" ") );
	fgSizerTop->Add( m_comboBoxSearch, 0, wxALL|wxEXPAND, 5 );
	
	m_static_replace = new wxStaticText( this, wxID_ANY, wxT("Replace: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_replace->Wrap( -1 );
	m_static_replace->Hide();
	
	fgSizerTop->Add( m_static_replace, 0, wxALIGN_CENTER, 5 );
	
	m_comboBoxReplace = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Append( wxT(" ") );
	m_comboBoxReplace->Hide();
	
	fgSizerTop->Add( m_comboBoxReplace, 0, wxALL|wxEXPAND, 5 );
	
	bSizerBottom->Add( fgSizerTop, 0, wxALL|wxEXPAND, 5 );
	
	fgSizerLeft->Add( bSizerBottom, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerOptions;
	bSizerOptions = new wxBoxSizer( wxHORIZONTAL );
	
	wxString m_searchtypeChoices[] = { wxT("Text"), wxT("Hex") };
	int m_searchtypeNChoices = sizeof( m_searchtypeChoices ) / sizeof( wxString );
	m_searchtype = new wxRadioBox( this, wxID_ANY, wxT("Search as"), wxDefaultPosition, wxDefaultSize, m_searchtypeNChoices, m_searchtypeChoices, 1, wxRA_SPECIFY_COLS );
	m_searchtype->SetSelection( 0 );
	bSizerOptions->Add( m_searchtype, 1, wxALL, 3 );
	
	wxStaticBoxSizer* sbSizerSearchOptions;
	sbSizerSearchOptions = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Options") ), wxVERTICAL );
	
	chkMatchCase = new wxCheckBox( this, wxID_ANY, wxT("Match Case"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerSearchOptions->Add( chkMatchCase, 0, wxALL, 5 );
	
	chkSearchBackwards = new wxCheckBox( this, wxID_ANY, wxT("Search backwards"), wxDefaultPosition, wxDefaultSize, 0 );
	chkSearchBackwards->Enable( false );
	
	sbSizerSearchOptions->Add( chkSearchBackwards, 0, wxALL, 5 );
	
	chkWrapAround = new wxCheckBox( this, wxID_ANY, wxT("Wrap around"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerSearchOptions->Add( chkWrapAround, 0, wxALL, 5 );
	
	chkUTF8 = new wxCheckBox( this, wxID_ANY, wxT("As UTF-8"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerSearchOptions->Add( chkUTF8, 0, wxALL, 5 );
	
	bSizerOptions->Add( sbSizerSearchOptions, 1, wxEXPAND, 5 );
	
	fgSizerLeft->Add( bSizerOptions, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 2 );
	
	bSizerMain->Add( fgSizerLeft, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxVERTICAL );
	
	btnFind = new wxButton( this, wxID_ANY, wxT("Find"), wxDefaultPosition, wxDefaultSize, 0 );
	btnFind->SetDefault(); 
	bSizerButtons->Add( btnFind, 0, wxALIGN_CENTER|wxEXPAND|wxTOP, 2 );
	
	btnFindAll = new wxButton( this, wxID_ANY, wxT("Find All"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( btnFindAll, 0, wxALIGN_CENTER|wxEXPAND|wxTOP, 2 );
	
	btnFindSomeBytes = new wxButton( this, wxID_ANY, wxT("Find Some Bytes"), wxDefaultPosition, wxDefaultSize, 0 );
	btnFindSomeBytes->SetToolTip( wxT("This function finds some different values at file. Usefull for finding non-null sections at disks.") );
	
	bSizerButtons->Add( btnFindSomeBytes, 0, 0, 5 );
	
	m_staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_staticline->Hide();
	
	bSizerButtons->Add( m_staticline, 0, wxEXPAND | wxALL, 5 );
	
	btnReplace = new wxButton( this, wxID_ANY, wxT("Replace"), wxDefaultPosition, wxDefaultSize, 0 );
	btnReplace->Hide();
	
	bSizerButtons->Add( btnReplace, 0, wxTOP|wxEXPAND, 2 );
	
	btnReplaceAll = new wxButton( this, wxID_ANY, wxT("Replace All"), wxDefaultPosition, wxDefaultSize, 0 );
	btnReplaceAll->Hide();
	
	bSizerButtons->Add( btnReplaceAll, 0, wxTOP|wxEXPAND, 0 );
	
	bSizerMain->Add( bSizerButtons, 0, wxEXPAND|wxRIGHT, 5 );
	
	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
	
	// Connect Events
	m_comboBoxSearch->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	m_comboBoxSearch->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	m_searchtype->Connect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	chkUTF8->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnFind->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnFindAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnFindSomeBytes->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnReplace->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnReplaceAll->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
}

FindDialogGui::~FindDialogGui()
{
	// Disconnect Events
	m_comboBoxSearch->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	m_comboBoxSearch->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	m_searchtype->Disconnect( wxEVT_COMMAND_RADIOBOX_SELECTED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	chkUTF8->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnFind->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnFindAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnFindSomeBytes->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnReplace->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	btnReplaceAll->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FindDialogGui::EventHandler ), NULL, this );
	
}

UpdateDialogGui::UpdateDialogGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* MainBoxSizer;
	MainBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	
	wxbtmp_icon = new wxStaticBitmap( this, wxID_OPEN, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	MainBoxSizer->Add( wxbtmp_icon, 0, wxALIGN_CENTER|wxALL, 10 );
	
	wxBoxSizer* VerticalBoxSizer;
	VerticalBoxSizer = new wxBoxSizer( wxVERTICAL );
	
	version_text = new wxStaticText( this, wxID_ANY, wxT("New wxHexEditor version %s is available!"), wxDefaultPosition, wxDefaultSize, 0 );
	version_text->Wrap( -1 );
	VerticalBoxSizer->Add( version_text, 0, wxALIGN_CENTER|wxALL, 10 );
	
	m_hyperlink = new wxHyperlinkCtrl( this, wxID_ANY, wxT("Download new wxHexEditor"), wxT("http://wxhexeditor.sourceforge.net"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	VerticalBoxSizer->Add( m_hyperlink, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxchk_display = new wxCheckBox( this, wxID_ANY, wxT("Do not show this again."), wxDefaultPosition, wxDefaultSize, 0 );
	VerticalBoxSizer->Add( wxchk_display, 0, wxALIGN_CENTER|wxALL, 10 );
	
	MainBoxSizer->Add( VerticalBoxSizer, 1, wxEXPAND, 5 );
	
	this->SetSizer( MainBoxSizer );
	this->Layout();
	MainBoxSizer->Fit( this );
	
	// Connect Events
	wxchk_display->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( UpdateDialogGui::OnChkDisplay ), NULL, this );
}

UpdateDialogGui::~UpdateDialogGui()
{
	// Disconnect Events
	wxchk_display->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( UpdateDialogGui::OnChkDisplay ), NULL, this );
	
}

CompareDialogGui::CompareDialogGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizerFiles;
	fgSizerFiles = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerFiles->AddGrowableCol( 1 );
	fgSizerFiles->SetFlexibleDirection( wxBOTH );
	fgSizerFiles->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_TextFile1 = new wxStaticText( this, wxID_ANY, wxT("File #1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextFile1->Wrap( -1 );
	fgSizerFiles->Add( m_TextFile1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	filePick1 = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	fgSizerFiles->Add( filePick1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_TextFile2 = new wxStaticText( this, wxID_ANY, wxT("File #2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextFile2->Wrap( -1 );
	fgSizerFiles->Add( m_TextFile2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	filePick2 = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	fgSizerFiles->Add( filePick2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizerMain->Add( fgSizerFiles, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerSearchFor;
	sbSizerSearchFor = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Search For") ), wxHORIZONTAL );
	
	m_radioDifferent = new wxRadioButton( this, wxID_ANY, wxT("Different bytes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_radioDifferent->SetValue( true ); 
	sbSizerSearchFor->Add( m_radioDifferent, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_radioSame = new wxRadioButton( this, wxID_ANY, wxT("Same bytes"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerSearchFor->Add( m_radioSame, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizerMain->Add( sbSizerSearchFor, 0, wxALIGN_CENTER|wxEXPAND, 5 );
	
	wxBoxSizer* bSizerStopAfter;
	bSizerStopAfter = new wxBoxSizer( wxHORIZONTAL );
	
	checkStopCompare = new wxCheckBox( this, wxID_ANY, wxT("Stop comparison after"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerStopAfter->Add( checkStopCompare, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	spinStopCompare = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10000, 0 );
	spinStopCompare->Enable( false );
	
	bSizerStopAfter->Add( spinStopCompare, 0, wxALL, 5 );
	
	m_staticText20 = new wxStaticText( this, wxID_ANY, wxT("hits."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	bSizerStopAfter->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizerMain->Add( bSizerStopAfter, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerMerger;
	bSizerMerger = new wxBoxSizer( wxHORIZONTAL );
	
	checkMerge = new wxCheckBox( this, wxID_ANY, wxT("Merge sections if close as"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMerger->Add( checkMerge, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	spinMerge = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 100000000, 0 );
	spinMerge->Enable( false );
	
	bSizerMerger->Add( spinMerge, 0, wxALL, 5 );
	
	m_staticText201 = new wxStaticText( this, wxID_ANY, wxT("bytes."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText201->Wrap( -1 );
	bSizerMerger->Add( m_staticText201, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizerMain->Add( bSizerMerger, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerSave;
	bSizerSave = new wxBoxSizer( wxHORIZONTAL );
	
	checkSaveResults = new wxCheckBox( this, wxID_ANY, wxT("Save Results to File"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerSave->Add( checkSaveResults, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	filePickSave = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.txt"), wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE );
	filePickSave->Enable( false );
	
	bSizerSave->Add( filePickSave, 0, wxALL, 5 );
	
	bSizerMain->Add( bSizerSave, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	btnCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( btnCancel, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	btnCompare = new wxButton( this, wxID_ANY, wxT("Compare"), wxDefaultPosition, wxDefaultSize, 0 );
	btnCompare->SetDefault(); 
	btnCompare->Enable( false );
	
	bSizerButtons->Add( btnCompare, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizerMain->Add( bSizerButtons, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	filePick1->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( CompareDialogGui::OnFileChange ), NULL, this );
	filePick2->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( CompareDialogGui::OnFileChange ), NULL, this );
	checkStopCompare->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	checkMerge->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	checkSaveResults->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	filePickSave->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( CompareDialogGui::filePickSaveOnFileChanged ), NULL, this );
	btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	btnCompare->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
}

CompareDialogGui::~CompareDialogGui()
{
	// Disconnect Events
	filePick1->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( CompareDialogGui::OnFileChange ), NULL, this );
	filePick2->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( CompareDialogGui::OnFileChange ), NULL, this );
	checkStopCompare->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	checkMerge->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	checkSaveResults->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	filePickSave->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( CompareDialogGui::filePickSaveOnFileChanged ), NULL, this );
	btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	btnCompare->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	
}

ChecksumDialogGui::ChecksumDialogGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizerTop;
	bSizerTop = new wxBoxSizer( wxHORIZONTAL );
	
	txtFileToHash = new wxStaticText( this, wxID_ANY, wxT("File To Hash:"), wxDefaultPosition, wxDefaultSize, 0 );
	txtFileToHash->Wrap( -1 );
	bSizerTop->Add( txtFileToHash, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizerDiler;
	bSizerDiler = new wxBoxSizer( wxVERTICAL );
	
	chkFile = new wxCheckBox( this, wxID_ANY, wxT("Curent File"), wxDefaultPosition, wxDefaultSize, 0 );
	chkFile->Enable( false );
	
	bSizerDiler->Add( chkFile, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	filePick = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE );
	bSizerDiler->Add( filePick, 0, wxALL|wxEXPAND, 5 );
	
	bSizerTop->Add( bSizerDiler, 1, wxEXPAND, 5 );
	
	bSizerMain->Add( bSizerTop, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerHashTypes;
	bSizerHashTypes = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizerMD;
	sbSizerMD = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("MD") ), wxVERTICAL );
	
	chkMD2 = new wxCheckBox( this, wxID_ANY, wxT("MD2 (slow!)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerMD->Add( chkMD2, 0, 0, 5 );
	
	chkMD4 = new wxCheckBox( this, wxID_ANY, wxT("MD4"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerMD->Add( chkMD4, 0, 0, 5 );
	
	chkMD5 = new wxCheckBox( this, wxID_ANY, wxT("MD5"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerMD->Add( chkMD5, 0, 0, 5 );
	
	bSizerHashTypes->Add( sbSizerMD, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerSHA;
	sbSizerSHA = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("SHA") ), wxVERTICAL );
	
	chkSHA1 = new wxCheckBox( this, wxID_ANY, wxT("SHA1"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerSHA->Add( chkSHA1, 0, 0, 5 );
	
	chkSHA224 = new wxCheckBox( this, wxID_ANY, wxT("SHA224"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerSHA->Add( chkSHA224, 0, 0, 5 );
	
	chkSHA256 = new wxCheckBox( this, wxID_ANY, wxT("SHA256"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerSHA->Add( chkSHA256, 0, 0, 5 );
	
	chkSHA384 = new wxCheckBox( this, wxID_ANY, wxT("SHA384"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerSHA->Add( chkSHA384, 0, 0, 5 );
	
	chkSHA512 = new wxCheckBox( this, wxID_ANY, wxT("SHA512"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerSHA->Add( chkSHA512, 0, 0, 5 );
	
	bSizerHashTypes->Add( sbSizerSHA, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerRIPEMD;
	sbSizerRIPEMD = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("RipeMD") ), wxVERTICAL );
	
	chkRIPEMD128 = new wxCheckBox( this, wxID_ANY, wxT("RipeMD128"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerRIPEMD->Add( chkRIPEMD128, 0, 0, 5 );
	
	chkRIPEMD160 = new wxCheckBox( this, wxID_ANY, wxT("RipeMD160"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerRIPEMD->Add( chkRIPEMD160, 0, 0, 5 );
	
	chkRIPEMD256 = new wxCheckBox( this, wxID_ANY, wxT("RipeMD256"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerRIPEMD->Add( chkRIPEMD256, 0, 0, 5 );
	
	chkRIPEMD320 = new wxCheckBox( this, wxID_ANY, wxT("RipeMD320"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerRIPEMD->Add( chkRIPEMD320, 0, 0, 5 );
	
	bSizerHashTypes->Add( sbSizerRIPEMD, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerHAVAL;
	sbSizerHAVAL = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("HAVAL") ), wxVERTICAL );
	
	chkHAVAL128 = new wxCheckBox( this, wxID_ANY, wxT("HAVAL128"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerHAVAL->Add( chkHAVAL128, 0, 0, 5 );
	
	chkHAVAL160 = new wxCheckBox( this, wxID_ANY, wxT("HAVAL160"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerHAVAL->Add( chkHAVAL160, 0, 0, 5 );
	
	chkHAVAL192 = new wxCheckBox( this, wxID_ANY, wxT("HAVAL192"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerHAVAL->Add( chkHAVAL192, 0, 0, 5 );
	
	chkHAVAL224 = new wxCheckBox( this, wxID_ANY, wxT("HAVAL224"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerHAVAL->Add( chkHAVAL224, 0, 0, 5 );
	
	chkHAVAL256 = new wxCheckBox( this, wxID_ANY, wxT("HAVAL256"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerHAVAL->Add( chkHAVAL256, 0, 0, 5 );
	
	bSizerHashTypes->Add( sbSizerHAVAL, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerTIGER;
	sbSizerTIGER = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("TIGER") ), wxVERTICAL );
	
	chkTIGER128 = new wxCheckBox( this, wxID_ANY, wxT("TIGER128"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerTIGER->Add( chkTIGER128, 0, 0, 5 );
	
	chkTIGER160 = new wxCheckBox( this, wxID_ANY, wxT("TIGER160"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerTIGER->Add( chkTIGER160, 0, 0, 5 );
	
	chkTIGER = new wxCheckBox( this, wxID_ANY, wxT("TIGER (192)"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerTIGER->Add( chkTIGER, 0, 0, 5 );
	
	bSizerHashTypes->Add( sbSizerTIGER, 1, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizerOthers;
	sbSizerOthers = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Others") ), wxVERTICAL );
	
	chkADLER32 = new wxCheckBox( this, wxID_ANY, wxT("ADLER32"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerOthers->Add( chkADLER32, 0, 0, 5 );
	
	chkCRC32 = new wxCheckBox( this, wxID_ANY, wxT("CRC32"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerOthers->Add( chkCRC32, 0, 0, 5 );
	
	chkCRC32B = new wxCheckBox( this, wxID_ANY, wxT("CRC32B"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerOthers->Add( chkCRC32B, 0, 0, 5 );
	
	chkWHIRLPOOL = new wxCheckBox( this, wxID_ANY, wxT("WHRILPOOL"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerOthers->Add( chkWHIRLPOOL, 0, 0, 5 );
	
	chkGOST = new wxCheckBox( this, wxID_ANY, wxT("GOST"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerOthers->Add( chkGOST, 0, 0, 5 );
	
	chkSNEFRU128 = new wxCheckBox( this, wxID_ANY, wxT("SNEFRU128"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerOthers->Add( chkSNEFRU128, 0, 0, 5 );
	
	chkSNEFRU256 = new wxCheckBox( this, wxID_ANY, wxT("SNEFRU256"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizerOthers->Add( chkSNEFRU256, 0, 0, 5 );
	
	bSizerHashTypes->Add( sbSizerOthers, 1, wxEXPAND, 5 );
	
	bSizerMain->Add( bSizerHashTypes, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	btnCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( btnCancel, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	btnCalculate = new wxButton( this, wxID_ANY, wxT("Calculate"), wxDefaultPosition, wxDefaultSize, 0 );
	btnCalculate->Enable( false );
	
	bSizerButtons->Add( btnCalculate, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizerMain->Add( bSizerButtons, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	chkFile->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	filePick->Connect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ChecksumDialogGui::filePickOnFileChanged ), NULL, this );
	chkMD2->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkMD4->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkMD5->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA1->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA224->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA256->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA384->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA512->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkRIPEMD128->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkRIPEMD160->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkRIPEMD256->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkRIPEMD320->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL128->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL160->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL192->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL224->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL256->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkTIGER128->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkTIGER160->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkTIGER->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkADLER32->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkCRC32->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkCRC32B->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkWHIRLPOOL->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkGOST->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSNEFRU128->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSNEFRU256->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	btnCalculate->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
}

ChecksumDialogGui::~ChecksumDialogGui()
{
	// Disconnect Events
	chkFile->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	filePick->Disconnect( wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler( ChecksumDialogGui::filePickOnFileChanged ), NULL, this );
	chkMD2->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkMD4->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkMD5->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA1->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA224->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA256->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA384->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSHA512->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkRIPEMD128->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkRIPEMD160->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkRIPEMD256->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkRIPEMD320->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL128->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL160->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL192->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL224->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkHAVAL256->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkTIGER128->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkTIGER160->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkTIGER->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkADLER32->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkCRC32->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkCRC32B->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkWHIRLPOOL->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkGOST->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSNEFRU128->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	chkSNEFRU256->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	btnCalculate->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ChecksumDialogGui::EventHandler ), NULL, this );
	
}

CopyAsDialogGui::CopyAsDialogGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizerSelections;
	fgSizerSelections = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerSelections->AddGrowableCol( 1 );
	fgSizerSelections->SetFlexibleDirection( wxBOTH );
	fgSizerSelections->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	txtCopyAs = new wxStaticText( this, wxID_ANY, wxT("Copy As :"), wxDefaultPosition, wxDefaultSize, 0 );
	txtCopyAs->Wrap( -1 );
	fgSizerSelections->Add( txtCopyAs, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxString chcCopyAsChoices[] = { wxT("Full Text"), wxT("Special Hex"), wxT("Internet"), wxT("C/C++ Source"), wxT("Assembler Source") };
	int chcCopyAsNChoices = sizeof( chcCopyAsChoices ) / sizeof( wxString );
	chcCopyAs = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, chcCopyAsNChoices, chcCopyAsChoices, 0 );
	chcCopyAs->SetSelection( 0 );
	fgSizerSelections->Add( chcCopyAs, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	txtOption = new wxStaticText( this, wxID_ANY, wxT("Option:"), wxDefaultPosition, wxDefaultSize, 0 );
	txtOption->Wrap( -1 );
	fgSizerSelections->Add( txtOption, 0, wxALIGN_CENTER_VERTICAL|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxArrayString chcOptionChoices;
	chcOption = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, chcOptionChoices, 0 );
	chcOption->SetSelection( 0 );
	chcOption->Enable( false );
	
	fgSizerSelections->Add( chcOption, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	bSizerMain->Add( fgSizerSelections, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerMiddle;
	bSizerMiddle = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizerLeft;
	bSizerLeft = new wxBoxSizer( wxVERTICAL );
	
	chkBigEndian = new wxCheckBox( this, wxID_ANY, wxT("As BigEndian"), wxDefaultPosition, wxDefaultSize, 0 );
	chkBigEndian->Enable( false );
	
	bSizerLeft->Add( chkBigEndian, 0, wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	
	wxStaticBoxSizer* sbSizerBPL;
	sbSizerBPL = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Bytes Per Line") ), wxVERTICAL );
	
	spnBytePerLine = new wxSpinCtrl( this, wxID_ANY, wxT("16"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 8, 1024, 0 );
	sbSizerBPL->Add( spnBytePerLine, 0, wxALIGN_RIGHT, 5 );
	
	bSizerLeft->Add( sbSizerBPL, 1, wxEXPAND, 5 );
	
	bSizerMiddle->Add( bSizerLeft, 0, 0, 5 );
	
	wxBoxSizer* bSizerRight;
	bSizerRight = new wxBoxSizer( wxVERTICAL );
	
	chkOffset = new wxCheckBox( this, wxID_ANY, wxT("Include Offset Part"), wxDefaultPosition, wxDefaultSize, 0 );
	chkOffset->SetValue(true); 
	bSizerRight->Add( chkOffset, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	chkHex = new wxCheckBox( this, wxID_ANY, wxT("Include Hex Part"), wxDefaultPosition, wxDefaultSize, 0 );
	chkHex->SetValue(true); 
	bSizerRight->Add( chkHex, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	chkText = new wxCheckBox( this, wxID_ANY, wxT("Include Text Part"), wxDefaultPosition, wxDefaultSize, 0 );
	chkText->SetValue(true); 
	bSizerRight->Add( chkText, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );
	
	bSizerMiddle->Add( bSizerRight, 0, 0, 5 );
	
	bSizerMain->Add( bSizerMiddle, 0, 0, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	btnCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( btnCancel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	btnCopy = new wxButton( this, wxID_OK, wxT("Copy"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( btnCopy, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizerMain->Add( bSizerButtons, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	chcCopyAs->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
	chcOption->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
	chkBigEndian->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
	btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
	btnCopy->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
}

CopyAsDialogGui::~CopyAsDialogGui()
{
	// Disconnect Events
	chcCopyAs->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
	chcOption->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
	chkBigEndian->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
	btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
	btnCopy->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CopyAsDialogGui::EventHandler ), NULL, this );
	
}

XORViewDialogGui::XORViewDialogGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	txtWarning = new wxStaticText( this, wxID_ANY, wxT("For switching XORView Thru mode, all Undo&Redo buffer will reset and non-saved changes will discarded.\nAlso you can't use methods that changes file size (like delete and inject) with XORView Thru mode enabled."), wxDefaultPosition, wxDefaultSize, 0 );
	txtWarning->Wrap( -1 );
	bSizerMain->Add( txtWarning, 0, wxALL, 5 );
	
	wxBoxSizer* bSizerRadio;
	bSizerRadio = new wxBoxSizer( wxHORIZONTAL );
	
	txtSelection = new wxStaticText( this, wxID_ANY, wxT("Please enter XOR key as "), wxDefaultPosition, wxDefaultSize, 0 );
	txtSelection->Wrap( -1 );
	bSizerRadio->Add( txtSelection, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	radioASCII = new wxRadioButton( this, wxID_ANY, wxT("ASCII"), wxDefaultPosition, wxDefaultSize, 0 );
	radioASCII->SetValue( true ); 
	bSizerRadio->Add( radioASCII, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	radioHex = new wxRadioButton( this, wxID_ANY, wxT("Hex"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerRadio->Add( radioHex, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizerMain->Add( bSizerRadio, 1, wxEXPAND, 5 );
	
	XORtext = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_DONTWRAP );
	bSizerMain->Add( XORtext, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	btnCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( btnCancel, 0, wxALL, 5 );
	
	btnOK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( btnOK, 0, wxALL, 5 );
	
	bSizerMain->Add( bSizerButtons, 1, wxALIGN_RIGHT, 5 );
	
	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	btnOK->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( XORViewDialogGui::EventHandler ), NULL, this );
}

XORViewDialogGui::~XORViewDialogGui()
{
	// Disconnect Events
	btnOK->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( XORViewDialogGui::EventHandler ), NULL, this );
	
}
