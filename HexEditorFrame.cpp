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
				HexEditorGui( parent, id )
				{
				MyAUI = new wxAuiManager( this );
				MyNotebook = new wxAuiNotebook(this,-1);
				MyNotebook->SetArtProvider(new wxAuiSimpleTabArt);
				MyNotebook->SetWindowStyleFlag(0);
				MyAUI -> AddPane( MyNotebook, wxAuiPaneInfo().Name(wxT("wxHE")).Caption(wxT("HexEditor")).
						MinSize(wxSize(400,100)).CloseButton(false).
						Center().Layer(1)	);

	MyInterpreter = new DataInterpreter( this, -1 );
//	MyNotebook->SetDropTarget( new DnDFile( MyNotebook, statusbar, myinterpreter) );
	MyAUI -> AddPane( MyInterpreter, wxAuiPaneInfo().
					Name(wxT("wxHEint")).Caption(wxT("DataInterpreter")).
					MinSize(wxSize(207,-1)).
					Left().Layer(1)
					);
	MyAUI->GetPane(MyInterpreter).Float();


#if defined( _DEBUG_ )
   wxFileName myname(_("./testfile"));
   MyNotebook->AddPage( new wxHexEditor(MyNotebook, -1, statusBar, MyInterpreter, &myname ), myname.GetFullName() );
//				MyHexEditor->FileOpen( myfilename );
//				MyHexEditor->Select( 1000, 2001 );
#endif

	//SetDropTarget( new DnDFile( MyNotebook, statusbar, myinterpreter) );
				MyAUI->Update();
				}

HexEditorFrame::~HexEditorFrame(){}

void HexEditorFrame::OnFileOpen( wxCommandEvent& event ){
	wxFileDialog* filediag = new wxFileDialog(this,
											_("Choose a file for editing"),
											_(""),
											_(""),
											_("*"),
											wxFILE_MUST_EXIST|wxOPEN,
											wxDefaultPosition);
	if(wxID_OK == filediag->ShowModal()){
		wxFileName myname(filediag->GetPath());
		MyNotebook->AddPage( new wxHexEditor(MyNotebook, -1, statusBar, MyInterpreter, &myname ), myname.GetFullName(), true);
		filediag->Destroy();
		}
	event.Skip();
	}
void HexEditorFrame::OnFileClose( wxCommandEvent& event ){
	wxHexEditor *MyHexEditor = static_cast<wxHexEditor*>( MyNotebook->GetPage( MyNotebook->GetSelection() ) );
	if( MyHexEditor != NULL )
		if( MyHexEditor->FileClose() ){
			MyNotebook->DeletePage( MyNotebook->GetSelection() );
			// delete MyHexEditor; not neccessery, deletepage also delete this
			}
	event.Skip();
	}
