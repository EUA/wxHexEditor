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

HexEditorFrame::HexEditorFrame(	wxWindow* parent,int id ):
				HexEditorGui( parent, id ){
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
//	Toolbar->AddSeparator();
//  Toolbar->SetCustomOverflowItems(prepend_items, append_items);
    Toolbar->Realize();
	mbar->Check( idToolbar, true );

    MyAUI -> AddPane(Toolbar, wxAuiPaneInfo().
                  Name(wxT("ToolBar")).Caption(wxT("Big Toolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));

	MyInterpreter = new DataInterpreter( this, -1 );

	MyAUI -> AddPane( MyInterpreter, wxAuiPaneInfo().
					Caption(wxT("DataInterpreter")).
					TopDockable(false).
					BottomDockable(false).
					MinSize(wxSize(170,-1)).
					Left().Layer(1) );
	mbar->Check( idInterpreter, true );

	MyNotebook->SetDropTarget( new DnDFile( MyNotebook, statusBar, MyInterpreter) );

#if defined( _DEBUG_ )
	wxFileName myname(_("./testfile"));
	MyNotebook->AddPage( new HexEditor(MyNotebook, -1, statusBar, MyInterpreter, &myname ), myname.GetFullName() );
#endif
	MyAUI->Update();

	this->Connect( idInterpreter, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);
	}

HexEditorFrame::~HexEditorFrame(){
	this->Disconnect( idInterpreter, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorFrame::OnUpdateUI ),NULL,this);
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
		MyNotebook->AddPage( new HexEditor(MyNotebook, -1, statusBar, MyInterpreter, &myname ), myname.GetFullName(), true);
		filediag->Destroy();
		}
	event.Skip();
	}

void HexEditorFrame::OnFileClose( wxCommandEvent& event ){
	HexEditor *MyHexEditor = static_cast<HexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		if( MyHexEditor->FileClose() ){
			MyNotebook->DeletePage( MyNotebook->GetSelection() );
			// delete MyHexEditor; not neccessery, deletepage also delete this
			}
	event.Skip();
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
		case idToolbar:
			MyAUI->GetPane(Toolbar).Show(event.IsChecked());
			MyAUI->Update();
			break;
		default:
			wxBell();
		}
	}

void HexEditorFrame::OnUpdateUI(wxUpdateUIEvent& event){
	mbar->Check(idInterpreter, MyInterpreter->IsShown());
	mbar->Check(idToolbar, Toolbar->IsShown());
	}

bool DnDFile::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames){
	size_t nFiles = filenames.GetCount();
	for ( size_t n = 0; n < nFiles; n++ ) {
		wxFileName myfl( filenames[n] );
		if ( myfl.FileExists() ){
			m_pOwner->AddPage( new HexEditor( m_pOwner, 1, statusbar, myinterpreter, &myfl), myfl.GetFullName(), true);
			}
		}
	return TRUE;
	}
