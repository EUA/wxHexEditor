/***********************************(GPL)********************************
*	wxHexEditor is a hex edit tool for editing massive files in Linux   *
*	Copyright (C) 2006  Erdem U. Altinyurt                              *
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

#include "Tag.h"
TagElement::TagElement(){
	visible = false;
	start = end = 0;
	tag.Clear();
	FontClrData.SetColour( *wxBLACK );//Default wxBlack
	NoteClrData.SetColour( *wxRED );
	wxP = NULL;
	}

TagElement::TagElement( int64_t _start, int64_t _end, wxString _tag, wxColourData fntclr, wxColourData noteclr):
			start(_start), end(_end), tag(_tag), FontClrData(fntclr), NoteClrData(noteclr){
	visible = false;
	wxP = NULL;
	}

TagElement::~TagElement(){
	if(wxP != NULL )
		Hide();
	tag.Clear();
	}

void TagElement::Show( const wxPoint& pos, wxWindow *parent ){
	if( !visible ){
		visible = true;
		wxP = new wxPopupWindow( parent );
		wxP->SetBackgroundColour( NoteClrData.GetColour() );
		wxStaticText *text = new wxStaticText( wxP, wxID_ANY, tag );
		wxBoxSizer *topSizer = new wxBoxSizer( wxVERTICAL );
		topSizer->Add( text, 0, wxALL, 5 );
		wxP->SetAutoLayout( true );
		wxP->SetSizer( topSizer );
		topSizer->Fit(wxP);
		wxP->Position( pos, topSizer->GetSize() );
		wxP->Show();
		}
	}

void TagElement::Hide( void ){
		visible=false;
		wxP->Destroy();
		}

//----------TAG DIALOG-----------//
TagDialog::TagDialog(TagElement& TagE, wxWindow* parent):TagDialogGui( parent ),Tag(TagE){
	TmpTag = Tag;
	TagTextCtrl->SetValue( TmpTag.tag );
//	TagTextCtrl->SetBinValue( TmpTag.tag );
//	wxTextAttr attr;
//	attr.SetTextColour( TmpTag.FontClrData.GetColour() );
//	attr.SetBackgroundColour( TmpTag.NoteClrData.GetColour() );
//	attr.SetBackgroundColour( *wxRED );
//	TagTextCtrl->SetDefaultStyle( attr );

	wxBitmap bitmap( 100, 25);
    wxMemoryDC dc;
    dc.SelectObject( bitmap );
    dc.SetBackground( TmpTag.FontClrData.GetColour() );
    dc.Clear();
    dc.SelectObject( wxNullBitmap );
    FontBitmapButton->SetBitmapLabel( bitmap );

	dc.SelectObject( bitmap );
    dc.SetBackground( TmpTag.NoteClrData.GetColour() );
    dc.Clear();
    dc.SelectObject( wxNullBitmap );
    NoteBitmapButton->SetBitmapLabel( bitmap );
	}

void TagDialog::OnFontColor( wxCommandEvent& event ){
	ChooseColor( TmpTag.FontClrData );
	}
void TagDialog::OnNoteColor( wxCommandEvent& event ){
	ChooseColor( TmpTag.NoteClrData );
	}
void TagDialog::ChooseColor( wxColourData& tmpClrData ){
#if wxUSE_COLOURDLG
	wxColourDialog dialog(this, &tmpClrData);
#elif USE_COLOURDLG_GENERIC
	wxGenericColourDialog *dialog = new wxGenericColourDialog(this, &tmpClrData);
#endif
	if( &tmpClrData == &TmpTag.FontClrData )	//Cannot compare wxColourData, so I am comparing it's memory adresses :)
		dialog.SetTitle(_T("Choose the font colour"));
	else
		dialog.SetTitle(_T("Choose the background colour"));

    if (dialog.ShowModal() == wxID_OK)
        tmpClrData = dialog.GetColourData();

	wxBitmap bitmap( 100, 25);
    wxMemoryDC dc;
    dc.SelectObject( bitmap );
    dc.SetBackground( tmpClrData.GetColour() );
    dc.Clear();
    dc.SelectObject( wxNullBitmap );

	if( &tmpClrData == &TmpTag.FontClrData)
		FontBitmapButton->SetBitmapLabel( bitmap );
	else
		NoteBitmapButton->SetBitmapLabel( bitmap );

//	wxTextAttr attr;
//	attr.SetTextColour( TmpTag.FontClrData.GetColour() );
//	attr.SetBackgroundColour( TmpTag.NoteClrData.GetColour() );
//	TagTextCtrl->SetDefaultStyle( attr );
	}

void TagDialog::OnSave( wxCommandEvent& event ){
	TmpTag.tag = TagTextCtrl->GetValue();
	Tag = TmpTag;
	EndModal(wxID_SAVE);
	}

void TagDialog::OnDelete( wxCommandEvent& event ){
	EndModal(wxID_DELETE);
	}

