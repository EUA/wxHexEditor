/***********************************(GPL)********************************
*	wxHexEditor is a hex edit tool for editing massive files in Linux   *
*	Copyright (C) 2007  Erdem U. Altinyurt                              *
*                                                                       *
*	This program is free software; you can redistribute it and/or       *
*	modify it under the terms of the GNU General Public License         *
*	as published by the Free Software Foundation; either version 2      *
*	of the License, or any later version.                               *
*                                                                       *
*	This program is distributed in the hope that it will be useful,     *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*	GNU General Public License for more details.                        *
*                                                                       *
*	You should have received a copy of the GNU General Public License   *
*	along with this program;                                            *
*   if not, write to the Free Software	Foundation, Inc.,               *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : death_knight at gamebox.net                     *
*************************************************************************/
#include "HexEditorFrame.h"

#include "InfoPanel.h"

HexEditorFrame::HexEditorFrame(	wxWindow* parent,int id ):
				HexEditorGui( parent, id, wxT("wxHexEditor v0.05 Alpha") ){

	PrepareAUI();

#if defined( _DEBUG_ )
	wxFileName myname(_("./testfile.swp"));
	MyNotebook->AddPage( new HexEditor(MyNotebook, -1, statusBar, MyInterpreter, MyInfoPanel, &myname ), myname.GetFullName() );
	ActionEnabler();
	MyNotebook->SetSelection( 0 );
#endif

	MyAUI->Update();

	this->Connect( idInterpreter, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);
	this->Connect( idFileRO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);

	MyNotebook->Connect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabSelection ), NULL,this );
	MyNotebook->Connect( wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabClose ), NULL,this );

	// wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_DOWN
	// wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED
	// wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGING
	// wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE

	}

HexEditorFrame::~HexEditorFrame(){
	this->Disconnect( idInterpreter, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);
	this->Disconnect( idFileRO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);
	MyNotebook->Disconnect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabSelection ), NULL,this );
	MyNotebook->Disconnect( wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabClose ), NULL,this );
	}

void HexEditorFrame::PrepareAUI( void ){
		MyAUI = new wxAuiManager( this );

	MyNotebook = new wxAuiNotebook(this,-1);
	MyNotebook->SetArtProvider(new wxAuiSimpleTabArt);
	MyNotebook->SetWindowStyleFlag(0);
//				MyAUI->AddPane( MyNotebook, wxCENTER);

	MyAUI -> AddPane( MyNotebook, wxAuiPaneInfo().
			CaptionVisible(false).
			MinSize(wxSize(400,100)).
			CloseButton(false).
			Center().Layer(1)	);

    wxAuiToolBarItemArray prepend_items;
/*
    wxAuiToolBarItemArray append_items;
    wxAuiToolBarItem item;
    item.SetKind(wxITEM_SEPARATOR);
    append_items.Add(item);
    item.SetKind(wxITEM_NORMAL);
    item.SetId(wxID_ANY);
    item.SetLabel(_("Customize..."));
    append_items.Add(item);
*/
	Toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                         wxAUI_TB_DEFAULT_STYLE );// wxAUI_TB_OVERFLOW);
	Toolbar->SetToolBitmapSize(wxSize(48,48));
    Toolbar->AddTool(wxID_NEW, _T("New File"), wxArtProvider::GetBitmap(wxART_NEW));
    Toolbar->AddTool(wxID_OPEN, _T("Open File"), wxArtProvider::GetBitmap(wxART_FILE_OPEN));
    Toolbar->AddTool(wxID_SAVE, _T("Save File"), wxArtProvider::GetBitmap(wxART_FILE_SAVE));
    Toolbar->AddTool(wxID_SAVEAS, _T("Save File"), wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS));
    Toolbar->AddTool(idClose, _T("Close File"), wxArtProvider::GetBitmap(wxART_CROSS_MARK));
	Toolbar->AddSeparator();
	Toolbar->AddTool(wxID_FIND, _T("Find"), wxArtProvider::GetBitmap(wxART_FIND));
	Toolbar->AddTool(wxID_REPLACE, _T("Replace"), wxArtProvider::GetBitmap(wxART_FIND_AND_REPLACE));
	Toolbar->AddTool(idGotoOffset, _T("GoTo"), wxArtProvider::GetBitmap(wxART_GO_FORWARD));
	Toolbar->AddSeparator();
	Toolbar->AddTool(wxID_UNDO, _T("Undo"), wxArtProvider::GetBitmap(wxART_UNDO));
	Toolbar->AddTool(wxID_REDO, _T("Redo"), wxArtProvider::GetBitmap(wxART_REDO));
	Toolbar->AddSeparator();
	Toolbar->AddTool(wxID_COPY, _T("Copy Block"), wxArtProvider::GetBitmap(wxART_COPY));
	Toolbar->AddTool(wxID_PASTE, _T("Paste Block"), wxArtProvider::GetBitmap(wxART_PASTE));
	Toolbar->AddSeparator();
	Toolbar->AddTool(wxID_CUT, _T("Cut Block"), wxArtProvider::GetBitmap(wxART_CUT));
	Toolbar->AddTool(wxID_DELETE, _T("Delete Block"), wxArtProvider::GetBitmap(wxART_DELETE));

//  Toolbar->SetCustomOverflowItems(prepend_items, append_items);
    Toolbar->Realize();
	mbar->Check( idToolbar, true );

    MyAUI -> AddPane(Toolbar, wxAuiPaneInfo().
                  Name(wxT("ToolBar")).Caption(wxT("Big Toolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));

	MyInfoPanel = new InfoPanel( this, -1 );
	MyAUI -> AddPane( MyInfoPanel, wxAuiPaneInfo().
					Caption(wxT("InfoPanel")).
					TopDockable(false).
					BottomDockable(false).
					MinSize(wxSize(170,111)).
					Left().Layer(1) );
	mbar->Check( idInfoPanel, true );

	MyInterpreter = new DataInterpreter( this, -1 );

	MyAUI -> AddPane( MyInterpreter, wxAuiPaneInfo().
					Caption(wxT("DataInterpreter")).
					TopDockable(false).
					BottomDockable(false).
					MinSize(wxSize(170,190)).
					Left().Layer(1) );
	mbar->Check( idInterpreter, true );

	ActionDisabler();
	MyNotebook->SetDropTarget( new DnDFile( MyNotebook, statusBar, MyInterpreter, MyInfoPanel) );
	MyInfoPanel->SetDropTarget( new DnDFile( MyNotebook, statusBar, MyInterpreter, MyInfoPanel) );
	MyInterpreter->SetDropTarget( new DnDFile( MyNotebook, statusBar, MyInterpreter, MyInfoPanel) );
	Toolbar->SetDropTarget( new DnDFile( MyNotebook, statusBar, MyInterpreter, MyInfoPanel) );
	}

void HexEditorFrame::ActionEnabler( void ){
	mbar->Enable( idFileRO, true );
	mbar->Enable( idFileRW, true );
//	mbar->Enable( idFileDW, true );
	mbar->Enable( wxID_SAVE, true );
//	mbar->Enable( wxID_SAVEAS, true );
	mbar->Enable( idClose, true );
//	mbar->Enable( wxID_FIND, true );
//	mbar->Enable( wxID_REPLACE, true );
//	mbar->Enable( idGotoOffset, true );
	mbar->Enable( wxID_UNDO, true );
	mbar->Enable( wxID_REDO, true );
//	mbar->Enable( wxID_COPY, true );
//	mbar->Enable( wxID_PASTE, true );
//	mbar->Enable( wxID_CUT, true );
//	mbar->Enable( wxID_DELETE, true );

	Toolbar->EnableTool( wxID_SAVE, true);
//	Toolbar->EnableTool( wxID_SAVEAS, true);
	Toolbar->EnableTool( idClose, true);
//	Toolbar->EnableTool( wxID_FIND, false);
//	Toolbar->EnableTool( wxID_REPLACE, false);
//	Toolbar->EnableTool( idGotoOffset, true);
	Toolbar->EnableTool( wxID_UNDO, true);
	Toolbar->EnableTool( wxID_REDO, true);
//	Toolbar->EnableTool( wxID_COPY, true);
//	Toolbar->EnableTool( wxID_PASTE, true);
//	Toolbar->EnableTool( wxID_CUT, true);
//	Toolbar->EnableTool( wxID_DELETE, true);
	}

void HexEditorFrame::ActionDisabler( void ){
	mbar->Enable( idFileRO, false );
	mbar->Enable( idFileRW, false );
	mbar->Enable( idFileDW, false );
	mbar->Enable( wxID_SAVE, false );
	mbar->Enable( wxID_SAVEAS, false );
	mbar->Enable( idClose, false );
	mbar->Enable( wxID_FIND, false );
	mbar->Enable( wxID_REPLACE, false );
	mbar->Enable( idGotoOffset, false );
	mbar->Enable( wxID_UNDO, false );
	mbar->Enable( wxID_REDO, false );
	mbar->Enable( wxID_COPY, false );
	mbar->Enable( wxID_PASTE, false );
	mbar->Enable( wxID_CUT, false );
	mbar->Enable( wxID_DELETE, false );

	Toolbar->EnableTool( wxID_SAVE, false);
	Toolbar->EnableTool( wxID_SAVEAS, false);
	Toolbar->EnableTool( idClose, false);
	Toolbar->EnableTool( wxID_FIND, false);
	Toolbar->EnableTool( wxID_REPLACE, false);
	Toolbar->EnableTool( idGotoOffset, false);
	Toolbar->EnableTool( wxID_UNDO, false);
	Toolbar->EnableTool( wxID_REDO, false);
	Toolbar->EnableTool( wxID_COPY, false);
	Toolbar->EnableTool( wxID_PASTE, false);
	Toolbar->EnableTool( wxID_CUT, false);
	Toolbar->EnableTool( wxID_DELETE, false);

	MyInterpreter->Clear();
	}

void HexEditorFrame::OnFileOpen( wxCommandEvent& event ){
	wxFileDialog* filediag = new wxFileDialog(this,
											_("Choose a file for editing"),
											_(""),
											_(""),
											_("*"),
											wxFD_FILE_MUST_EXIST|wxFD_OPEN,
											wxDefaultPosition);
	if(wxID_OK == filediag->ShowModal()){
		wxFileName myname(filediag->GetPath());
		MyNotebook->AddPage( new HexEditor(MyNotebook, -1, statusBar, MyInterpreter, MyInfoPanel, &myname ), myname.GetFullName(), true);
		ActionEnabler();
		filediag->Destroy();
		}
	event.Skip();
	}

void HexEditorFrame::OnFileSave( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		MyHexEditor->FileSave( false );
	event.Skip();
	}

void HexEditorFrame::OnFileClose( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		if( MyHexEditor->FileClose() ){
			MyNotebook->DeletePage( MyNotebook->GetSelection() );
			// delete MyHexEditor; not neccessery, DeletePage also delete this
			}
	 if( MyNotebook->GetPageCount() == 0 )
		ActionDisabler();
	event.Skip();
	}

void HexEditorFrame::OnQuit( wxCommandEvent& event ){
	HexEditor *MyHexEditor;
	for(;;){
		MyHexEditor = static_cast<HexEditor*>(MyNotebook->GetPage( 0 ) );
		if( MyHexEditor == NULL ){
			Destroy();
			event.Skip();
			return;
			}
		else if( MyHexEditor->FileClose() ){
			MyNotebook->DeletePage( 0 );
			MyHexEditor = NULL;
			}
		else break;
		}
	}

void HexEditorFrame::OnEditUndo( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		MyHexEditor->FileUndo();
	event.Skip();
	}

void HexEditorFrame::OnEditRedo( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		MyHexEditor->FileRedo();
	event.Skip();
	}

void HexEditorFrame::OnViewMenu( wxCommandEvent& event ){
	switch( event.GetId() ){
		case idInterpreter:
			MyAUI->GetPane(MyInterpreter).Show(event.IsChecked());
			MyAUI->Update();
			break;
		case idInfoPanel:
			MyAUI->GetPane(MyInfoPanel).Show(event.IsChecked());
			MyAUI->Update();
			break;
		case idToolbar:
			MyAUI->GetPane(Toolbar).Show(event.IsChecked());
			MyAUI->Update();
			break;
		default:
			wxBell();
		}
	}

void HexEditorFrame::OnOptionsFileMode( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
		if( MyHexEditor != NULL ){
			switch( event.GetId() ){
				case idFileRO:
					if( MyHexEditor->GetFileAccessMode() != FileDifference::ReadOnly )
						MyHexEditor->SetFileAccessMode( FileDifference::ReadOnly );
					break;

				case idFileRW:
					if( MyHexEditor->GetFileAccessMode() == FileDifference::ReadOnly )	//if its not RW or DW
						MyHexEditor->SetFileAccessMode( FileDifference::ReadWrite );
					break;

				case idFileDW:
					wxBell();
					break;

				default:
					wxBell();
				}

			}
	}

void HexEditorFrame::OnUpdateUI(wxUpdateUIEvent& event){
#ifdef _DEBUG_
	std::cout << "HexEditorFrame::OnUpdateUI(wxUpdateUIEvent& event) \n" ;
#endif
	mbar->Check(idInterpreter, MyInterpreter->IsShown());
	mbar->Check(idInfoPanel, MyInfoPanel->IsShown());
	mbar->Check(idToolbar, Toolbar->IsShown());
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
		if( MyHexEditor != NULL )
			switch( MyHexEditor->GetFileAccessMode() ){
				case FileDifference::ReadOnly :
					mbar->Check(idFileRO, true);
					break;
				case FileDifference::ReadWrite :
					mbar->Check(idFileRW, true);
					break;
				case FileDifference::DirectWrite :
					mbar->Check(idFileDW, true);
					break;
				}
	}

void HexEditorFrame::OnNotebookTabSelection( wxAuiNotebookEvent& event ){
#ifdef _DEBUG_
	std::cout << "HexEditorFrame::OnNotebookTabSelection( wxAuiNotebookEvent& event ) \n" ;
#endif
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
		if( MyHexEditor != NULL )
			MyInfoPanel->Set( MyHexEditor->GetFileName(), MyHexEditor->GetFileAccessModeString() );
	}

void HexEditorFrame::OnNotebookTabClose( wxAuiNotebookEvent& event ){
#ifdef _DEBUG_
	std::cout << "HexEditorFrame::OnNotebookTabClose( wxAuiNotebookEvent& event ) \n" ;
#endif
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( event.GetSelection() ) );
		if( MyHexEditor != NULL ){
			if( MyHexEditor->FileClose() ){
				MyNotebook->DeletePage( event.GetSelection() );
				// delete MyHexEditor; not neccessery, DeletePage also delete this
				}
			if( MyNotebook->GetPageCount() == 0 )
				ActionDisabler();
			}
	}


bool DnDFile::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames){
	size_t nFiles = filenames.GetCount();
	for ( size_t n = 0; n < nFiles; n++ ) {
		wxFileName myfl( filenames[n] );
		if ( myfl.FileExists() )
			m_pOwner->AddPage( new HexEditor( m_pOwner, 1, statusbar, myinterpreter, myinfopanel, &myfl), myfl.GetFullName(), true);
		}
	return TRUE;
	}
