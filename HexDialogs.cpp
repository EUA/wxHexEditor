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

#include "HexDialogs.h"
GotoDialog::GotoDialog( wxWindow* parent, uint64_t& _offset, uint64_t _cursor_offset, uint64_t _filesize  ):GotoDialogGui(parent, wxID_ANY){
	offset = &_offset;
	cursor_offset = _cursor_offset;
	is_olddec =true;
	filesize = _filesize;
	m_textCtrlOffset->SetFocus();
	}
wxString GotoDialog::Filter( wxString text ){
	for( unsigned i = 0 ; i < text.Length() ; i++ ){
		if( m_dec->GetValue() ? isdigit( text.GetChar( i ) ) : isxdigit( text.GetChar( i ) ))
			continue;
		else
			text.Remove( i, 1);
		}
	return text;
	}
void GotoDialog::OnInput( wxCommandEvent& event ){
	if(!m_textCtrlOffset->IsModified())
        return;
    long insertionPoint = m_textCtrlOffset->GetInsertionPoint();
// TODO (death#1#): Copy/Paste/Drop & illegal key walking?
    m_textCtrlOffset->ChangeValue( Filter( m_textCtrlOffset->GetValue() ) );
    m_textCtrlOffset->SetInsertionPoint(insertionPoint);
	event.Skip(false);
	}
void GotoDialog::OnGo( wxCommandEvent& event ){
	wxULongLong_t *wxul = new wxULongLong_t(0);
	m_textCtrlOffset->GetValue().ToULongLong( wxul, m_dec->GetValue() ? 10 : 16 );
	*offset = *wxul;
	delete wxul;
	if( m_branch->GetSelection() == 1)
		*offset += cursor_offset;
	else if( m_branch->GetSelection() == 2)
		*offset = filesize - *offset;
	if( *offset < 0 )
		*offset = 0;

	EndModal( wxID_OK );
	}

void GotoDialog::OnConvert( wxCommandEvent& event ){
	if( event.GetId() == m_dec->GetId() && !is_olddec ){	//old value is hex, new value is dec
		wxULongLong_t *wxul = new wxULongLong_t(0);
		m_textCtrlOffset->GetValue().ToULongLong( wxul, 16 );
		*offset = *wxul;
		delete wxul;
		m_textCtrlOffset->SetValue( wxString::Format(_T("%ld"),*offset) );
		is_olddec = true;
		}
	else if( event.GetId() == m_hex->GetId() && is_olddec ){	//old value is hex, new value is dec
		wxULongLong_t *wxul = new wxULongLong_t(0);
		m_textCtrlOffset->GetValue().ToULongLong( wxul, 10 );
		*offset = *wxul;
		delete wxul;
		m_textCtrlOffset->SetValue( wxString::Format(_T("%X"),*offset) );
		is_olddec = false;
		}
	event.Skip(false);
	}


FindDialog::FindDialog( wxWindow* _parent, FileDifference *_findfile ):FindDialogGui(parent, wxID_ANY){
	parent = static_cast< HexEditor* >(_parent);
	findfile = _findfile;
	}

// TODO (death#1#):Paint 4 Find
void FindDialog::OnFind( wxCommandEvent& event ){
	if( m_searchtype->GetSelection() == 0 ){//text search
		parent->Goto( FindText( m_textSearch->GetValue(), m_from->GetSelection() == 0 ? 0 : parent->CursorOffset() ) );
		}
	else{ //hex search
		parent->Goto( FindBinary( wxHexCtrl::HexToChar( m_textSearch->GetValue() ), m_textSearch->GetValue().Length()/2 , m_from->GetSelection() == 0 ? 0 : parent->CursorOffset()+1 ) );
		}
	}

uint64_t FindDialog::FindText( wxString target, uint64_t start_from ){
	wxBell();
// TODO (death#1#): TextFind!
	}

uint64_t FindDialog::FindBinary( const char *target, unsigned size, uint64_t from ){
	if(target == NULL) return -1;
	int64_t offset=from;
	int search_step = 10*MB;
	int readed=0;
	findfile->Seek( offset, wxFromStart );
	// TODO (death#6#): insert error check message here

	char* buffer = new char [search_step];
	if(buffer == NULL) return -1;
	// TODO (death#6#): insert error check message here
	readed = findfile->Read( buffer, search_step );
	int found = SearchAtBuffer( buffer, search_step, target, size );
	if(found >= 0)
		return offset+found;
	else
		offset +=readed;

	while(readed != search_step){
		memmove(buffer, buffer + search_step - size +1, size-1);// moving unprocessed buffer to begining.
		readed = findfile->Read( buffer, search_step - size + 1 );	//refilling buffer with fresh data
		found = SearchAtBuffer( buffer, search_step, target, size );
		if(found >= 0)
			return offset+found;
		else
			offset +=readed;
		}
	return -1;
	}

// TODO (death#9#): Implement better search algorithm. (Like GPGPU one using OpenCL)
uint64_t FindDialog::SearchAtBuffer( const char *bfr, int bfr_size, const char* search, int search_size ){	// Dummy search algorithm\ Yes yes I know there are better ones but I think this enought for now.
	for(int i=0 ; i < bfr_size - search_size + 1 ; i++ ){
		if(! strncmp( bfr+i, search, search_size )){
			return i;
			}
		}
	return -1;
	}
