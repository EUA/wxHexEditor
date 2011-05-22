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
	menuEditUndo = new wxMenuItem( editMenu, wxID_UNDO, wxString( wxT("&Undo") ) + wxT('\t') + wxT("CTRL+Z"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditUndo );
	menuEditUndo->Enable( false );
	
	wxMenuItem* menuEditRedo;
	menuEditRedo = new wxMenuItem( editMenu, wxID_REDO, wxString( wxT("&Redo") ) + wxT('\t') + wxT("CTRL+Y"), wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditRedo );
	menuEditRedo->Enable( false );
	
	wxMenuItem* m_separator2;
	m_separator2 = editMenu->AppendSeparator();
	
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
	
	wxMenuItem* menuEditDelete;
	menuEditDelete = new wxMenuItem( editMenu, wxID_DELETE, wxString( wxT("Delete") ) , wxEmptyString, wxITEM_NORMAL );
	editMenu->Append( menuEditDelete );
	menuEditDelete->Enable( false );
	
	wxMenuItem* menuEditInsert;
	menuEditInsert = new wxMenuItem( editMenu, idInsert, wxString( wxT("Insert") ) , wxEmptyString, wxITEM_NORMAL );
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
	
	wxMenuItem* menuViewTagPanel;
	menuViewTagPanel = new wxMenuItem( viewMenu, idTagPanel, wxString( wxT("TAG Panel") ) , wxEmptyString, wxITEM_CHECK );
	viewMenu->Append( menuViewTagPanel );
	
	mbar->Append( viewMenu, wxT("&View") ); 
	
	toolsMenu = new wxMenu();
	wxMenuItem* menuToolChecksum;
	menuToolChecksum = new wxMenuItem( toolsMenu, wxID_ANY, wxString( wxT("Calculate Checksum") ) , wxEmptyString, wxITEM_NORMAL );
	toolsMenu->Append( menuToolChecksum );
	menuToolChecksum->Enable( false );
	
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
	
	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( HexEditorGui::OnClose ) );
	this->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( HexEditorGui::OnKeyDown ) );
	this->Connect( menuFileNew->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileOpen->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileSave->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileSaveAs->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileClose->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuFileQuit->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnQuit ) );
	this->Connect( menuEditUndo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditRedo->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditCopy->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditCut->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditPaste->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditDelete->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditInsert->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditFind->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditReplace->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuEditGotooffset->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Connect( menuViewInterprater->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewInterprater->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuViewToolbar->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewToolbar->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuViewInfopanel->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewInfopanel->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuViewTagPanel->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Connect( menuViewTagPanel->GetId(), wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Connect( menuToolChecksum->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolMenu ) );
	this->Connect( menuToolCompare->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
	this->Connect( menuToolsXORView->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
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
	this->Disconnect( wxID_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_SAVEAS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idClose, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_CLOSE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnQuit ) );
	this->Disconnect( wxID_UNDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_REDO, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_COPY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_CUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_PASTE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_DELETE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idInsert, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_FIND, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( wxID_REPLACE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idGotoOffset, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnMenuEvent ) );
	this->Disconnect( idInterpreter, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( idInterpreter, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( idToolbar, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( idToolbar, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( idInfoPanel, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( idInfoPanel, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( idTagPanel, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnViewMenu ) );
	this->Disconnect( idTagPanel, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorGui::OnUpdateUI ) );
	this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolMenu ) );
	this->Disconnect( idCompare, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
	this->Disconnect( idXORView, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorGui::OnToolsMenu ) );
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
	fgSizerTop->Add( m_comboBoxSearch, 0, wxALL|wxEXPAND, 5 );
	
	m_static_replace = new wxStaticText( this, wxID_ANY, wxT("Replace: "), wxDefaultPosition, wxDefaultSize, 0 );
	m_static_replace->Wrap( -1 );
	m_static_replace->Hide();
	
	fgSizerTop->Add( m_static_replace, 0, wxALIGN_CENTER, 5 );
	
	m_comboBoxReplace = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN ); 
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
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizerButtons->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
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
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer5;
	fgSizer5 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer5->AddGrowableCol( 1 );
	fgSizer5->SetFlexibleDirection( wxBOTH );
	fgSizer5->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_TextFile1 = new wxStaticText( this, wxID_ANY, wxT("File #1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextFile1->Wrap( -1 );
	fgSizer5->Add( m_TextFile1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	filePick1 = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	fgSizer5->Add( filePick1, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_TextFile2 = new wxStaticText( this, wxID_ANY, wxT("File #2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextFile2->Wrap( -1 );
	fgSizer5->Add( m_TextFile2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	filePick2 = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.*"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_OPEN );
	fgSizer5->Add( filePick2, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer16->Add( fgSizer5, 0, wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Search For") ), wxHORIZONTAL );
	
	m_radioDifferent = new wxRadioButton( this, wxID_ANY, wxT("Different bytes"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_radioDifferent, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	m_radioSame = new wxRadioButton( this, wxID_ANY, wxT("Same bytes"), wxDefaultPosition, wxDefaultSize, 0 );
	sbSizer2->Add( m_radioSame, 1, wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer16->Add( sbSizer2, 0, wxALIGN_CENTER|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer25;
	bSizer25 = new wxBoxSizer( wxHORIZONTAL );
	
	checkStopCompare = new wxCheckBox( this, wxID_ANY, wxT("Stop comparison after"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer25->Add( checkStopCompare, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	spinStopCompare = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10000, 0 );
	spinStopCompare->Enable( false );
	
	bSizer25->Add( spinStopCompare, 0, wxALL, 5 );
	
	m_staticText20 = new wxStaticText( this, wxID_ANY, wxT("hits."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText20->Wrap( -1 );
	bSizer25->Add( m_staticText20, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer16->Add( bSizer25, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );
	
	checkSaveResults = new wxCheckBox( this, wxID_ANY, wxT("Save Results to File"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer17->Add( checkSaveResults, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	filePickSave = new wxFilePickerCtrl( this, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.txt"), wxDefaultPosition, wxDefaultSize, wxFLP_OVERWRITE_PROMPT|wxFLP_SAVE );
	filePickSave->Enable( false );
	
	bSizer17->Add( filePickSave, 0, wxALL, 5 );
	
	bSizer16->Add( bSizer17, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );
	
	btnCancel = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer19->Add( btnCancel, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	btnCompare = new wxButton( this, wxID_ANY, wxT("Compare"), wxDefaultPosition, wxDefaultSize, 0 );
	btnCompare->SetDefault(); 
	bSizer19->Add( btnCompare, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	bSizer16->Add( bSizer19, 0, wxEXPAND, 5 );
	
	this->SetSizer( bSizer16 );
	this->Layout();
	bSizer16->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	checkStopCompare->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	checkSaveResults->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	btnCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	btnCompare->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
}

CompareDialogGui::~CompareDialogGui()
{
	// Disconnect Events
	checkStopCompare->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	checkSaveResults->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	btnCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	btnCompare->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CompareDialogGui::EventHandler ), NULL, this );
	
}
