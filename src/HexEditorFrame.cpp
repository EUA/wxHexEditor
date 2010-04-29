/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
*   Copyright (C) 2010  Erdem U. Altinyurt                              *
*                                                                       *
*   This program is free software; you can redistribute it and/or       *
*   modify it under the terms of the GNU General Public License         *
*   as published by the Free Software Foundation; either version 2      *
*   of the License.                                                     *
*                                                                       *
*   This program is distributed in the hope that it will be useful,     *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*   GNU General Public License for more details.                        *
*                                                                       *
*   You should have received a copy of the GNU General Public License   *
*   along with this program;                                            *
*   if not, write to the Free Software Foundation, Inc.,                *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : death_knight at gamebox.net                     *
*************************************************************************/

#include "HexEditorFrame.h"
#define idDiskDevice 10000

HexEditorFrame::HexEditorFrame( wxWindow* parent,int id ):
				HexEditorGui( parent, id, wxString(_T("wxHexEditor ")) << _T(_VERSION_STR_ )){
	wxIcon wxHexEditor_ICON ( wxhex_xpm );
	this->SetIcon(wxHexEditor_ICON);

#if defined __WXMAC__ || defined __WXMSW__
	wxArtProvider::Push(new HexEditorArtProvider); //Using similar MacOSX icons. Much more better than wx ones...
#endif
	PrepareAUI();

	MyAUI->Update();
	this->Connect( SELECT_EVENT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ) );
	this->Connect( UNREDO_EVENT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ) );
	this->Connect( wxEVT_CHAR,	wxKeyEventHandler(HexEditorFrame::OnKeyDown),NULL, this);
	this->Connect( wxEVT_ACTIVATE, wxActivateEventHandler(HexEditorFrame::OnActivate),NULL, this );

	this->Connect( idInjection, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorFrame::OnMenuEvent ) );

	MyNotebook->Connect( wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabSelection ), NULL,this );
	MyNotebook->Connect( wxEVT_COMMAND_AUINOTEBOOK_TAB_MIDDLE_UP, wxAuiNotebookEventHandler(  HexEditorFrame::OnNotebookTabClose ), NULL,this );

	bool update_enable = true;
	if ( ! wxConfigBase::Get()->Read(_T("UpdateCheck"), &update_enable )){
		update_enable = true;
		wxConfigBase::Get()->Write( _T("UpdateCheck"), update_enable );
		}
	if( update_enable ){
		double last_chk=0;
		wxConfigBase::Get()->Read(_T("LastUpdateCheckTime"), (&last_chk));
		//if( wxDateTime::Now() - wxDateSpan::Week() > wxDateTime( last_chk ) )	//One check for a week enough
			{
			wxConfigBase::Get()->Write(_T("LastUpdateCheckTime"), static_cast< double >( wxDateTime::Now().GetTicks()) );
			VersionChecker vc( wxT("http://wxhexeditor.sourceforge.net/version.php"), wxT(_VERSION_) );
			}
		}
	}

HexEditorFrame::~HexEditorFrame(){
	this->Disconnect( SELECT_EVENT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ) );
	this->Disconnect( UNREDO_EVENT, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ) );
   this->Disconnect( wxEVT_CHAR,	wxKeyEventHandler(HexEditorFrame::OnKeyDown),NULL, this);
	this->Disconnect( wxEVT_ACTIVATE, wxActivateEventHandler(HexEditorFrame::OnActivate),NULL, this );

	this->Disconnect( idInjection, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorFrame::OnMenuEvent ) );

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
			Center().Layer(1) );

//    wxAuiToolBarItemArray prepend_items;
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
#ifdef _WX_AUIBAR_H_
	Toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE );// wxAUI_TB_OVERFLOW);
#else
	Toolbar = new wxToolBar( this,  wxID_ANY, wxDefaultPosition, wxDefaultSize );
#endif
//	Toolbar->SetToolBitmapSize(wxSize(48,48));

	Toolbar->AddTool(wxID_NEW, _T("New File"), wxArtProvider::GetBitmap(wxART_NEW, wxART_TOOLBAR), _T("New file"));
	Toolbar->AddTool(wxID_OPEN, _T("Open File"), wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_TOOLBAR), _T("Open"));
	Toolbar->AddTool(wxID_SAVE, _T("Save File"), wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_TOOLBAR), _T("Save"));
	Toolbar->AddTool(wxID_SAVEAS, _T("SaveAs File"), wxArtProvider::GetBitmap(wxART_FILE_SAVE_AS, wxART_TOOLBAR), _T("Save as"));
	Toolbar->AddTool(idClose, _T("Close File"), wxArtProvider::GetBitmap(wxART_CROSS_MARK, wxART_TOOLBAR), _T("Close"));
	Toolbar->AddSeparator();
	Toolbar->AddTool(wxID_FIND, _T("Find"), wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR), _T("Find"));
	Toolbar->AddTool(wxID_REPLACE, _T("Replace"), wxArtProvider::GetBitmap(wxART_FIND_AND_REPLACE, wxART_TOOLBAR), _T("Find and replace"));
	Toolbar->AddTool(idGotoOffset, _T("GoTo"), wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_TOOLBAR), _T("Goto offset"));
	Toolbar->AddSeparator();
	Toolbar->AddTool(wxID_UNDO, _T("Undo"), wxArtProvider::GetBitmap(wxART_UNDO, wxART_TOOLBAR), _T("Undo"));
	Toolbar->AddTool(wxID_REDO, _T("Redo"), wxArtProvider::GetBitmap(wxART_REDO, wxART_TOOLBAR), _T("Redo"));
	Toolbar->AddSeparator();
	Toolbar->AddTool(wxID_COPY, _T("Copy Block"), wxArtProvider::GetBitmap(wxART_COPY, wxART_TOOLBAR), _T("Copy"));
	Toolbar->AddTool(wxID_PASTE, _T("Paste Block"), wxArtProvider::GetBitmap(wxART_PASTE, wxART_TOOLBAR), _T("Paste"));
	Toolbar->AddSeparator();
	Toolbar->AddTool(wxID_CUT, _T("Cut Block"), wxArtProvider::GetBitmap(wxART_CUT, wxART_TOOLBAR), _T("Cuts selected block and copies to clipboard"));
	Toolbar->AddTool(wxID_DELETE, _T("Delete Block"), wxArtProvider::GetBitmap(wxART_DELETE, wxART_TOOLBAR), _T("Deletes selected block"));
//	Toolbar->AddTool(idInjection, _T("Insert Block"), wxArtProvider::GetBitmap(wxART_GO_DOWN, wxART_TOOLBAR), _T("Insert"));

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
	int arr[] = { idFileRO, idFileRW, idFileDW, wxID_SAVE, wxID_SAVEAS, idClose, wxID_FIND, wxID_REPLACE, idGotoOffset, wxID_PASTE };
	for( int i=0 ; i<10 ; i++ ){
		mbar->Enable( arr[i],true );
		Toolbar->EnableTool( arr[i], true );
		}
	MyInterpreter->Enable();
	Toolbar->Refresh();
	}

void HexEditorFrame::ActionDisabler( void ){
	int arr[] = { idFileRO, idFileRW,idFileDW, wxID_SAVE, wxID_SAVEAS, idClose, wxID_FIND, wxID_REPLACE, idGotoOffset, wxID_PASTE, wxID_CUT, wxID_DELETE, wxID_COPY, wxID_UNDO, wxID_REDO, };
	for( int i=0 ; i<15 ; i++ ){
		mbar->Enable( arr[i],false );
		Toolbar->EnableTool( arr[i], false );
		}
	MyInterpreter->Clear();
	MyInterpreter->Disable();
	Toolbar->Refresh();
	}

void HexEditorFrame::OpenFile(wxFileName flname){
	HexEditor *x = new HexEditor(MyNotebook, -1, statusBar,	MyInterpreter,	MyInfoPanel );
	if(x->FileOpen( flname )){
		MyNotebook->AddPage( x, flname.GetFullName(), true );
		ActionEnabler();
		}
	else{
		x->Destroy();
		wxMessageBox( _("File cannot open!"),_T("Error!"), wxICON_ERROR );
		}
	}

void HexEditorFrame::OnMenuEvent( wxCommandEvent& event ){
	if( event.GetId() == wxID_NEW ){	//GetFile Lenght, Save file as, Create file, Open file as RW
		wxString lngt;
		long long size=0;
		while(1){
			lngt = wxGetTextFromUser( _("Please indicate file size in decimal."),
                                    _("Enter File Size:"));
			if(lngt.IsEmpty()){
				return;
			}
			else if( lngt.ToLongLong( &size, 10 ) and (size > 0) )//1 Exabyte is enought for everyone for now, 2010
				break;
			wxMessageBox( _("Wrong input, please retry...") ,_T("Error!"), wxICON_ERROR );
			}
		//Save file
		wxFileDialog* filediag = new wxFileDialog(this,
									_("Choose a file for save as"),
									_(""),
									_(""),
									_("*"),
									wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR,
									wxDefaultPosition);

		if(wxID_OK == filediag->ShowModal()){
			wxFileName flname(filediag->GetPath());
			//create file
			wxFile crt;
			if( not crt.Create( flname.GetFullPath(), true ) ){
				wxMessageBox( _("File cannot open!") ,_T("Error!"), wxICON_ERROR );
				return;
				}
			if( not crt.Open( flname.GetFullPath(), wxFile::read_write ) ){
				wxMessageBox( _("File cannot open!") ,_T("Error!"), wxICON_ERROR );
				return;
				}
			crt.Seek( size-1 );
			crt.Write("\0x00", 1);
			crt.Close();
			//Openning the file with text editor.
			HexEditor *x = new HexEditor(MyNotebook, -1, statusBar, MyInterpreter, MyInfoPanel );
			if(x->FileOpen( flname )){
				MyNotebook->AddPage( x, flname.GetFullName(), true );
				ActionEnabler();
				}
			else
				x->Destroy();
			filediag->Destroy();
			}
		}
	else if( event.GetId() == wxID_OPEN ){
		wxFileDialog* filediag = new wxFileDialog(this,
									_("Choose a file for editing"),
									_(""),
									_(""),
									_("*"),
									wxFD_FILE_MUST_EXIST|wxFD_OPEN|wxFD_CHANGE_DIR,
									wxDefaultPosition);
		if(wxID_OK == filediag->ShowModal()){
			wxFileName flname(filediag->GetPath());
			OpenFile( flname );
			filediag->Destroy();
			}
		}
	else{
		if( MyNotebook->GetPageCount() ){
			HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
			if( MyHexEditor != NULL ){
				switch( event.GetId() ){
					//case wxID_OPEN: not handled here!
					case wxID_SAVE:		MyHexEditor->FileSave( false );		break;
					case wxID_SAVEAS:{
						wxFileDialog* filediag = new wxFileDialog(this,
															_("Choose a file for save as"),
															_(""),
															_(""),
															_("*"),
															wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR,
															wxDefaultPosition);
						if(wxID_OK == filediag->ShowModal()){
							if( !MyHexEditor->FileSave( filediag->GetPath() )){
								wxMessageDialog *dlg = new wxMessageDialog(NULL,wxString(_("File cannot save as ")).Append( filediag->GetPath() ),_("Error"), wxOK|wxICON_ERROR, wxDefaultPosition);
								dlg->ShowModal();dlg->Destroy();
								}
							}
						break;
						}
					case idClose:{
						if( MyHexEditor->FileClose() ){
							MyNotebook->DeletePage( MyNotebook->GetSelection() );// delete MyHexEditor; not neccessery, DeletePage also delete this
							if( MyNotebook->GetPageCount() == 0 )
								ActionDisabler();
							return;
							}
						break;
						}
					case wxID_UNDO:		MyHexEditor->DoUndo();					break;
					case wxID_REDO:		MyHexEditor->DoRedo();					break;
					case wxID_COPY:		MyHexEditor->CopySelection();			break;
					case wxID_CUT:			MyHexEditor->CutSelection();			break;
					case wxID_PASTE:		MyHexEditor->PasteFromClipboard();	break;
					case wxID_DELETE:		MyHexEditor->DeleteSelection();		break;
					case idInjection:		MyHexEditor->InsertBytes();			break;
					case wxID_FIND:		MyHexEditor->FindDialog();				break;
					case wxID_REPLACE:	MyHexEditor->ReplaceDialog();			break;
					case idGotoOffset:	MyHexEditor->GotoDialog();				break;
					case idFileRO:{
						MyHexEditor->SetFileAccessMode( FileDifference::ReadOnly );
						MyInfoPanel->Set( MyHexEditor->GetFileName(), MyHexEditor->FileLength(), MyHexEditor->GetFileAccessModeString(), MyHexEditor->GetFD() );
						break;
						}
					case idFileRW:{
						MyHexEditor->SetFileAccessMode( FileDifference::ReadWrite );
						MyInfoPanel->Set( MyHexEditor->GetFileName(), MyHexEditor->FileLength(), MyHexEditor->GetFileAccessModeString(), MyHexEditor->GetFD() );
						break;
						}
					case idFileDW:
						if( wxOK == wxMessageBox( _("This mode will write changes every change to file DIRECTLY directly."),_("Warning!"), wxOK|wxCANCEL|wxICON_WARNING, this, wxCenter ) )
							MyHexEditor->SetFileAccessMode( FileDifference::DirectWrite );
						MyInfoPanel->Set( MyHexEditor->GetFileName(), MyHexEditor->FileLength(), MyHexEditor->GetFileAccessModeString(), MyHexEditor->GetFD() );
						break;
					default: wxBell();
					}
				}
			}
		}
	event.Skip();
	}

void HexEditorFrame::OnDeviceMenu( wxCommandEvent& event ){
	if( event.GetId() >= idDiskDevice ){
		int i=event.GetId() - idDiskDevice;
		wxArrayString disks;
		wxDir::GetAllFiles(wxT("/dev/disk/by-id"), &disks );
		disks.Sort();
		OpenFile( wxFileName(disks.Item(i)) );
		}
	else if( event.GetId() == idDeviceRam ){
		//OpenFile( wxFileName( wxT("\\\\.\\PhysicalDrive0") ) );//Just testing
// TODO (death#1#): RAM access with mmap and need DirectWrite Mode
		}
	}

void HexEditorFrame::OnClose( wxCloseEvent& event ){
	wxCommandEvent evt;
	OnQuit( evt );
	event.Skip(false);
	}

void HexEditorFrame::OnQuit( wxCommandEvent& event ){
	HexEditor *MyHexEditor;
	for(;;){
		if( MyNotebook->GetPageCount() ){
			MyHexEditor = static_cast<HexEditor*>(MyNotebook->GetPage( 0 ) );
			if( MyHexEditor != NULL ){
				if( MyHexEditor->FileClose() ){
					MyNotebook->DeletePage( 0 );
					MyHexEditor = NULL;
					}
				else
					break;
				}
			else
				wxLogError(_("Error on quit!"));
			}
		else{
			Destroy();
			event.Skip();
			return;
			}
		}
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
	AllAbout.SetVersion( _T(_VERSION_STR_) );
	AllAbout.SetDescription(_("wxHexEditor is a hex editor for HUGE files and devices on Linux mainland."));
	AllAbout.SetCopyright(_T("(C) 2006-2010 Erdem U. Altinyurt"));
	AllAbout.AddDeveloper( _T("Erdem U. Altinyurt") );
	AllAbout.AddArtist( _T("Vlad Adrian") );
	AllAbout.SetWebSite( _T("http://wxhexeditor.sourceforge.net"));

	AllAbout.SetLicense( _T("wxHexEditor is a hex editor for HUGE files and devices on Linux mainland.\n"
			 "Copyright (C) 2010  Erdem U. Altinyurt\n"
			 "\n"
			 "This program is free software; you can redistribute it and/or\n"
			 "modify it under the terms of the GNU General Public License\n"
			 "as published by the Free Software Foundation version 2 "
			 "of the License."
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

void HexEditorFrame::OnUpdateUI(wxUpdateUIEvent& event){
#ifdef _DEBUG_
	std::cout << "HexEditorFrame::OnUpdateUI(wxUpdateUIEvent& event) ID " << event.GetId() << "\n" ;
#endif
	mbar->Check(idInterpreter, MyInterpreter->IsShown());
	mbar->Check(idInfoPanel, MyInfoPanel->IsShown());
	mbar->Check(idToolbar, Toolbar->IsShown());
	if(event.GetId() == idDeviceRam){
		//when updateUI received by Ram Device open event is came, thna needed to update Device List.
		#ifndef __WXMSW__
		wxMenuItemList devMen = menuDeviceDisk->GetMenuItems();
		#ifdef _DEBUG_
			std::cout << "HexEditorFrame::Ram event :" << event.GetString().ToAscii() << std::endl ;
		#endif
		int i=0;
		for( wxMenuItemList::iterator it = devMen.begin(); it != devMen.end() ; it++ ){
			menuDeviceDisk->Remove( *it );
		//	this->Disconnect( idDiskDevice+i++, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorFrame::OnDeviceMenu ) );
			}

		this->Disconnect( wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorFrame::OnDeviceMenu ) );
		///ls -l /dev/disk/by-id
		wxArrayString disks;
		wxDir::GetAllFiles(wxT("/dev/disk/by-id"), &disks );
		disks.Sort();
		for( unsigned i =0 ; i < disks.Count() ; i++){
			#ifdef _DEBUG_
			std::cout << "Disk: " << disks.Item(i).ToAscii() << std::endl;
			#endif
			menuDeviceDisk->Append( idDiskDevice+i, disks.Item(i).AfterLast('/'), wxT(""), wxITEM_NORMAL );
			this->Connect( idDiskDevice+i, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorFrame::OnDeviceMenu ) );
			}
		#endif
		}

	if( MyNotebook->GetPageCount() ){
		HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
		if( MyHexEditor != NULL ){
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

		if(event.GetId() == SELECT_EVENT ){
			#ifdef _DEBUG_SELECT_
				std::cout << "HexEditorFrame::Select_Event :" << event.GetString().ToAscii() << std::endl ;
			#endif
			Toolbar->EnableTool( wxID_COPY, event.GetString() == wxT("Selected") );
			mbar->Enable( wxID_COPY, event.GetString() == wxT("Selected") );
	#ifdef Enable_Injections
			Toolbar->EnableTool( wxID_CUT, event.GetString() == wxT("Selected") );
			mbar->Enable( wxID_CUT, event.GetString() == wxT("Selected") );
			Toolbar->EnableTool( wxID_DELETE, event.GetString() == wxT("Selected") );
			mbar->Enable( wxID_DELETE, event.GetString() == wxT("Selected") );
	//		Toolbar->EnableTool( idInjection, event.GetString() == wxT("notselected") );
	//		mbar->Enable( idInjection, event.GetString() == wxT("notselected") );
			Toolbar->Refresh();
	#endif
			}

		if(event.GetId() == UNREDO_EVENT ){
			#ifdef _DEBUG_FILE_
				std::cout << "HexEditorFrame::UNREDO event :" << event.GetString().ToAscii() << std::endl ;
			#endif
			Toolbar->EnableTool( wxID_UNDO, MyHexEditor->IsAvailable_Undo() );
			Toolbar->EnableTool( wxID_REDO, MyHexEditor->IsAvailable_Redo() );
			mbar->Enable( wxID_UNDO, MyHexEditor->IsAvailable_Undo() );
			mbar->Enable( wxID_REDO, MyHexEditor->IsAvailable_Redo() );
			Toolbar->Refresh();
			}
		}
	event.Skip();
	}

void HexEditorFrame::OnNotebookTabSelection( wxAuiNotebookEvent& event ){
#ifdef _DEBUG_
	std::cout << "HexEditorFrame::OnNotebookTabSelection( wxAuiNotebookEvent& event ) \n" ;
#endif
	if( MyNotebook->GetPageCount() ){
		HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage(  event.GetSelection() ) );
			if( MyHexEditor != NULL ){
				MyHexEditor->UpdateCursorLocation();
				MyInfoPanel->Set( MyHexEditor->GetFileName(), MyHexEditor->FileLength(), MyHexEditor->GetFileAccessModeString(), MyHexEditor->GetFD() );

				Toolbar->EnableTool( wxID_COPY, not MyHexEditor->select->IsState( Select::SELECT_FALSE ) );
				mbar->Enable( wxID_COPY, not MyHexEditor->select->IsState( Select::SELECT_FALSE ) );

				Toolbar->EnableTool( wxID_UNDO, MyHexEditor->IsAvailable_Undo() );
				Toolbar->EnableTool( wxID_REDO, MyHexEditor->IsAvailable_Redo() );
				mbar->Enable( wxID_UNDO, MyHexEditor->IsAvailable_Undo() );
				mbar->Enable( wxID_REDO, MyHexEditor->IsAvailable_Redo() );

				Toolbar->Refresh();
				}
		}
	event.Skip();
	}

void HexEditorFrame::OnNotebookTabClose( wxAuiNotebookEvent& event ){
#ifdef _DEBUG_
	std::cout << "HexEditorFrame::OnNotebookTabClose( wxAuiNotebookEvent& event ) \n" ;
#endif
	if( MyNotebook->GetPageCount() ){
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
	}

void HexEditorFrame::OnActivate( wxActivateEvent& event ){
#ifdef _DEBUG_MOUSE_
	std::cout << "HexEditorFrame::OnActivate( wxActivateEvent& event ) \n" ;
#endif
	TagHideAll();
	}

void HexEditorFrame::TagHideAll( void ){
	if( MyNotebook->GetPageCount() ){
		HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
		if( MyHexEditor != NULL )
			MyHexEditor->TagHideAll();
		}
	}

wxBitmap HexEditorArtProvider::CreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& WXUNUSED(size)){
	if ( client == wxART_TOOLBAR ){
#if defined __WXMAC__
		if ( id == wxART_NEW )					return wxGetBitmapFromMemory(osx_new);
		if ( id == wxART_FILE_OPEN )			return wxGetBitmapFromMemory(osx_open);
		if ( id == wxART_FILE_SAVE )			return wxGetBitmapFromMemory(osx_save);
		if ( id == wxART_FILE_SAVE_AS )		return wxGetBitmapFromMemory(osx_save);
		if ( id == wxART_CROSS_MARK )			return wxGetBitmapFromMemory(osx_cancel);
		if ( id == wxART_FIND )					return wxGetBitmapFromMemory(osx_find);
		if ( id == wxART_FIND_AND_REPLACE )	return wxGetBitmapFromMemory(osx_find_and_replace);
		if ( id == wxART_GO_FORWARD )			return wxGetBitmapFromMemory(osx_jump_to);
		if ( id == wxART_UNDO )					return wxGetBitmapFromMemory(osx_undo);
		if ( id == wxART_REDO )					return wxGetBitmapFromMemory(osx_redo);
		if ( id == wxART_COPY )					return wxGetBitmapFromMemory(osx_copy);
		if ( id == wxART_PASTE )				return wxGetBitmapFromMemory(osx_paste);
		if ( id == wxART_CUT )					return wxGetBitmapFromMemory(osx_cut);
		if ( id == wxART_DELETE )				return wxGetBitmapFromMemory(osx_delete);
#elif defined __WXMSW__
		if ( id == wxART_NEW )					return wxGetBitmapFromMemory(win_new);
		if ( id == wxART_FILE_OPEN )			return wxGetBitmapFromMemory(win_open);
		if ( id == wxART_FILE_SAVE )			return wxGetBitmapFromMemory(win_save);
		if ( id == wxART_FILE_SAVE_AS )		return wxGetBitmapFromMemory(win_saveas);
		if ( id == wxART_CROSS_MARK )			return wxGetBitmapFromMemory(win_cancel);
		if ( id == wxART_FIND )					return wxGetBitmapFromMemory(win_find);
		if ( id == wxART_FIND_AND_REPLACE )	return wxGetBitmapFromMemory(win_find_and_replace);
		if ( id == wxART_GO_FORWARD )			return wxGetBitmapFromMemory(win_jump_to);
		if ( id == wxART_UNDO )					return wxGetBitmapFromMemory(win_undo);
		if ( id == wxART_REDO )					return wxGetBitmapFromMemory(win_redo);
		if ( id == wxART_COPY )					return wxGetBitmapFromMemory(win_copy);
		if ( id == wxART_PASTE )				return wxGetBitmapFromMemory(win_paste);
		if ( id == wxART_CUT )					return wxGetBitmapFromMemory(win_cut);
		if ( id == wxART_DELETE )				return wxGetBitmapFromMemory(win_delete);
#endif
		}
	return wxNullBitmap;
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

VersionChecker::VersionChecker( wxString _url, wxString _version, wxWindow *parent, wxWindowID id )
:UpdateDialog_Gui( parent, id ){
	wxURL url( _url );
	if (url.IsOk()){
		url.GetProtocol().SetTimeout(3);
		wxInputStream *in_stream = url.GetInputStream();
		if( in_stream == NULL || in_stream->GetSize() > 10 ){
			return;	//need for keep safe
			}
		char *bfr = new char[in_stream->GetSize()+1];
		for(unsigned i = 0 ; i < in_stream->GetSize()+1 ; i++ )
			bfr[i]=0;
		in_stream->Read(bfr, in_stream->GetSize());
		if( strncmp( bfr, _version.To8BitData(),  _version.Len() ) > 0 ){
			wxString newver = wxString::FromAscii( bfr );
			version_text->SetLabel(wxString::Format( _("New wxHexEditor version %s is available!"), newver.c_str() ));
			wxbtmp_icon->SetBitmap(  wxArtProvider::GetBitmap( wxART_TIP, wxART_MESSAGE_BOX ) );
			Centre();
			Fit();
			wxBell();
			ShowModal();
			}
		}
	}

void VersionChecker::OnChkDisplay( wxCommandEvent& event ){
	wxConfigBase::Get()->Write( _T("UpdateCheck"), !wxchk_display->GetValue());
	}
