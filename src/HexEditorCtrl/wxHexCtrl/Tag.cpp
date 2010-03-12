/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
*   Copyright (C) 2010  Erdem U. Altinyurt                              *
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

#include "Tag.h"
#include "wxHexCtrl.h"
TagElement::TagElement(){
	visible = false;
	start = end = 0;
	tag.Clear();
	FontClrData.SetColour( *wxBLACK );//Default wxBlack
	NoteClrData.SetColour( *wxRED );
#ifndef __WXMAC__
	wxP = NULL;
#endif
	}

TagElement::TagElement( uint64_t _start, uint64_t _end, wxString _tag, wxColourData fntclr, wxColourData noteclr):
			start(_start), end(_end), tag(_tag), FontClrData(fntclr), NoteClrData(noteclr){
	visible = false;
	}

TagElement::~TagElement(){
	if( visible )
		Hide();
#ifndef __WXMAC__
	if(wxP != NULL)
		wxP->Destroy();
#endif
	tag.Clear();
	}

void TagElement::Show( const wxPoint& pos, wxWindow *parent ){
	if( !visible ){
#ifdef _DEBUG_
		std::cout << "Show tag element " << this << std::endl;
#endif
		visible = true;
#ifndef __WXMAC__
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
		//wxP->Connect( wxEVT_MOTION,	wxMouseEventHandler(wxHexCtrl::OnMouseMove),NULL, parent);
		//parent->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler(wxHexCtrl::OnKillFocus),NULL, parent );
// TODO (death#1#): Auto hide on popup menus. Modularized!
#endif
		}
	}

void TagElement::Hide( void ){
	if( visible )
		{
#ifdef _DEBUG_
		std::cout << "Hide tag element " << this << std::endl;
#endif
		visible=false;
#ifndef __WXMAC__
		wxP->Hide();
		wxP->Destroy();
#endif
		}
	}

bool TagElement::isCover( uint64_t position ){
	if(( start <= position && end >= position )
		|| ( (start > end) && ( start >= position ) && ( end <= position )))	// for inversed start - end)
		return true;
	else
		return false;
	}

void TagElement::print( void ){
	std::cout << "TAD start:" << start << " end: "<< end << " Tag:" << tag.ToAscii() << std::endl;
	}

//----------TAG DIALOG-----------//
TagDialog::TagDialog(TagElement& TagE, wxWindow* parent):TagDialogGui( parent ),Tag(TagE){
	TmpTag = Tag;
	TagTextCtrl->SetValue( TmpTag.tag );
	m_FontColourPicker->SetColour( TmpTag.FontClrData.GetColour() );
	m_NoteColourPicker->SetColour( TmpTag.NoteClrData.GetColour() );
//	TagTextCtrl->SetBinValue( TmpTag.tag );
//	wxTextAttr attr;
//	attr.SetTextColour( TmpTag.FontClrData.GetColour() );
//	attr.SetBackgroundColour( TmpTag.NoteClrData.GetColour() );
//	attr.SetBackgroundColour( *wxRED );
//	TagTextCtrl->SetDefaultStyle( attr );
	}

void TagDialog::OnSave( wxCommandEvent& event ){
	TmpTag.tag = TagTextCtrl->GetValue();
	TmpTag.FontClrData.SetColour( m_FontColourPicker->GetColour() );
	TmpTag.NoteClrData.SetColour( m_NoteColourPicker->GetColour() );
	Tag = TmpTag;
	EndModal(wxID_SAVE);
	}

void TagDialog::OnDelete( wxCommandEvent& event ){
	EndModal(wxID_DELETE);
	}
