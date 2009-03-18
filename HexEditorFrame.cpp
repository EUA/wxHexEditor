/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
*   Copyright (C) 2006  Erdem U. Altinyurt                              *
*                                                                       *
*   This program is free software; you can redistribute it and/or       *
*   modify it under the terms of the GNU General Public License         *
*   as published by the Free Software Foundation; either version 2      *
*   of the License, or any later version.                               *
*                                                                       *
*   This program is distributed in the hope that it will be useful,     *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*   GNU General Public License for more details.                        *
*                                                                       *
*   You should have received a copy of the GNU General Public License   *
*   along with this program;                                            *
*   if not, write to the Free Software	Foundation, Inc.,                *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : death_knight at gamebox.net                     *
*************************************************************************/
#include "HexEditorFrame.h"

HexEditorFrame::HexEditorFrame(	wxWindow* parent,int id ):
				HexEditorGui( parent, id, wxString(_T("wxHexEditor ")) << _T(_VERSION_ ) ){
	wxIcon wxHexEditor_ICON ( mondrian_xpm );
	this->SetIcon(wxHexEditor_ICON);

	PrepareAUI();

#ifdef _DEBUG_
	wxFileName myname(_("./testfile.swp"));
	MyNotebook->AddPage( new HexEditor(MyNotebook, -1, statusBar, MyInterpreter, MyInfoPanel, &myname ), myname.GetFullName() );

	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( 0 ) );
	if( MyHexEditor != NULL )
		MyInfoPanel->Set( MyHexEditor->GetFileName(), MyHexEditor->FileLength(), MyHexEditor->GetFileAccessModeString(), MyHexEditor->GetFD() );

	ActionEnabler();
	MyNotebook->SetSelection( 0 );
#endif

	MyAUI->Update();

    this->Connect( wxEVT_CHAR,	wxKeyEventHandler(HexEditorFrame::OnKeyDown),NULL, this);
	this->Connect( idInterpreter, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);
	this->Connect( idFileRO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);
	this->Connect( idGotoOffset, wxCommandEventHandler(  HexEditorFrame::OnEditGoto ),NULL,this);
	MyNotebook->Connect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabSelection ), NULL,this );
	MyNotebook->Connect( wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabClose ), NULL,this );
	}

HexEditorFrame::~HexEditorFrame(){
	this->Disconnect( idInterpreter, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);
	this->Disconnect( idFileRO, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);
	this->Disconnect( idGotoOffset, wxCommandEventHandler(  HexEditorFrame::OnEditGoto ),NULL,this);
	MyNotebook->Disconnect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabSelection ), NULL,this );
	MyNotebook->Disconnect( wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabClose ), NULL,this );
	}

void HexEditorFrame::PrepareAUI( void ){
		MyAUI = new wxAuiManager( this );

	MyNotebook = new wxAuiNotebook(this,-1);
	MyNotebook->SetArtProvider(new wxAuiSimpleTabArt);
	MyNotebook->SetWindowStyleFlag(0);
//	MyAUI->AddPane( MyNotebook, wxCENTER);

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
	MyNotebook->SetDropTarget( new DnDFile( this ) );
	MyInfoPanel->SetDropTarget( new DnDFile( this ) );
	MyInterpreter->SetDropTarget( new DnDFile( this ) );
	Toolbar->SetDropTarget( new DnDFile( this ) );
	}

void HexEditorFrame::ActionEnabler( void ){
	int arr[] = { idFileRO, idFileRW, wxID_SAVE, wxID_SAVEAS, idClose, wxID_FIND, wxID_REPLACE, idGotoOffset, wxID_UNDO, wxID_REDO, wxID_COPY, wxID_PASTE };
//	idFileDW, wxID_CUT, wxID_DELETE
	for( int i=0 ; i<12 ; i++ ){
		mbar->Enable( arr[i],true );
		Toolbar->EnableTool( arr[i], true );
		}
	MyInterpreter->Enable();
	}

void HexEditorFrame::ActionDisabler( void ){
	int arr[] = { idFileRO, idFileRW, wxID_SAVE, wxID_SAVEAS, idClose, wxID_FIND, idGotoOffset, wxID_UNDO, wxID_REDO, wxID_COPY, wxID_PASTE , idFileDW, wxID_REPLACE, wxID_CUT, wxID_DELETE };
	for( int i=0 ; i<15 ; i++ ){
		mbar->Enable( arr[i],false );
		Toolbar->EnableTool( arr[i], false );
		}
	MyInterpreter->Clear();
	MyInterpreter->Disable();

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

		HexEditor *x = new HexEditor(MyNotebook, -1, statusBar, MyInterpreter, MyInfoPanel );
		if(x->FileOpen( myname )){
			MyNotebook->AddPage( x, myname.GetFullName(), true );
			ActionEnabler();
			}
		else
			x->Destroy();
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

void HexEditorFrame::OnFileSaveAs( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL ){
		wxFileDialog* filediag = new wxFileDialog(this,
											_("Choose a file for save as"),
											_(""),
											_(""),
											_("*"),
											wxFD_SAVE,
											wxDefaultPosition);
		if(wxID_OK == filediag->ShowModal()){
			if( !MyHexEditor->FileSave( filediag->GetPath() )){
				wxMessageDialog *dlg = new wxMessageDialog(NULL,wxString(_("File cannot save as ")).Append( filediag->GetPath() ),_("Error"), wxOK|wxICON_ERROR, wxDefaultPosition);
				dlg->ShowModal();dlg->Destroy();
				}
			}
		}
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
		MyHexEditor->Undo();
	event.Skip();
	}

void HexEditorFrame::OnEditRedo( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		MyHexEditor->Redo();
	event.Skip();
	}

void HexEditorFrame::OnEditCopy( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		MyHexEditor->CopySelection();
	event.Skip();
	}

void HexEditorFrame::OnEditCut( wxCommandEvent& event ){
//	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
//	if( MyHexEditor != NULL )
//		//MyHexEditor->Redo();
//	event.Skip();
	}

void HexEditorFrame::OnEditPaste( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		MyHexEditor->PasteFromClipboard();
	event.Skip();
	}

void HexEditorFrame::OnEditFind( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		MyHexEditor->FindDialog();
	event.Skip();
	}

void HexEditorFrame::OnEditReplace( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		MyHexEditor->ReplaceDialog();
	event.Skip();
	}

void HexEditorFrame::OnEditGoto( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL ){
		uint64_t newoffset;
		GotoDialog *mydialog = new GotoDialog( this, newoffset, MyHexEditor->CursorOffset(), MyHexEditor->FileLength() );
		if( mydialog->ShowModal() == wxID_OK ){
			MyHexEditor->Goto( newoffset );
			}
		}
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

void HexEditorFrame::OnAbout( wxCommandEvent& event ){
	wxAboutDialogInfo AllAbout;
    AllAbout.SetName(_T("wxHexEditor"));
    AllAbout.SetVersion( _T(_VERSION_) );
    AllAbout.SetDescription(_("wxHexEditor is a hex editor for HUGE files and devices on Linux mainland."));
    AllAbout.SetCopyright(_T("(C) 2006 Erdem U. Altinyurt"));

    AllAbout.SetWebSite( _T("http://wxhexeditor.sourceforge.net"));

	AllAbout.SetLicense( _T("wxHexEditor is a hex editor for HUGE files and devices on Linux mainland.\n"
             "Copyright (C) 2006  Erdem U. Altinyurt\n"
             "\n"
             "This program is free software; you can redistribute it and/or\n"
             "modify it under the terms of the GNU General Public License\n"
             "as published by the Free Software Foundation; either version 2"
             "of the License, or any later version."
             "\n"
             "This program is distributed in the hope that it will be useful,\n"
             "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
             "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
             "GNU General Public License for more details.\n"
             "\n"
             "You should have received a copy of the GNU General Public License\n"
             "along with this program; if not, write to the Free Software\n"
             "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\n"
             "\n"
             "home:  wxhexeditor.sourceforge.net\n"
             "email: death_knight@gamebox.net\n")
             );
    wxAboutBox(AllAbout);
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
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage(  event.GetSelection() ) );
		if( MyHexEditor != NULL )
			MyInfoPanel->Set( MyHexEditor->GetFileName(), MyHexEditor->FileLength(), MyHexEditor->GetFileAccessModeString(), MyHexEditor->GetFD() );
	event.Skip();
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
		if ( myfl.FileExists() ){
			HexFramework->MyNotebook->AddPage( new HexEditor( HexFramework->MyNotebook, 1, HexFramework->statusBar, HexFramework->MyInterpreter, HexFramework->MyInfoPanel, &myfl), myfl.GetFullName(), true);
			HexFramework->ActionEnabler();
			}
		else{
			wxMessageDialog dlg(NULL,wxString(_("Dropped file:\n")).Append( myfl.GetPath() ).Append(_("\ncannot open!")),_("Error"), wxOK|wxICON_ERROR, wxDefaultPosition);
			dlg.ShowModal();dlg.Destroy();
			}

		}
	return TRUE;
	}
