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


FindDialog::FindDialog( wxWindow* _parent, FileDifference *_findfile ):FindDialogGui( _parent, wxID_ANY){
	parent = static_cast< HexEditor* >(_parent);
	findfile = _findfile;
	m_textSearch->SetFocus();
	}

// TODO (death#1#):Paint 4 Find
// TODO (death#1#):HEX CHECK for input!
void FindDialog::OnFind( wxCommandEvent& event ){
	uint64_t found = -1;
	if( m_searchtype->GetSelection() == 0 ){//text search
		found = FindText( m_textSearch->GetValue(),
						m_from->GetSelection() == 0 ? 0 : parent->CursorOffset(),
						type_text );
		}
	else{ //hex search
		wxMemoryBuffer mymem = wxHexCtrl::HexToBin( m_textSearch->GetValue());
		found = FindBinary( static_cast<const char*>(mymem.GetData()),mymem.GetDataLen(),
							m_from->GetSelection() == 0 ? 0 : parent->CursorOffset()+1,
							type_hex );
		}
	if( found != -1 )
			parent->Goto( found );
	else
		wxMessageBox(_("Not Found"), _("Nothing found!") );
	}

uint64_t FindDialog::FindText( wxString target, uint64_t start_from, search_type_ type ){
	if( target.IsAscii() ){
		return FindBinary( target.ToAscii(), target.Length(), start_from, type );
		}
// TODO (death#1#): Find in UTF?
	}

uint64_t FindDialog::FindBinary( const char *target, unsigned size, uint64_t from, search_type_ type ){
	if(target == NULL) return -1;
	int64_t offset=from;
	int search_step = 10*MB;
	findfile->Seek( offset, wxFromStart );
	// TODO (death#6#): insert error check message here
	char* buffer = new char [search_step];
	if(buffer == NULL) return -1;
	// TODO (death#6#): insert error check message here
	int unprocessed_bytes = 0;
	int found = -1;
	int readed=0;
	do{
// TODO (death#1#): Seach bar with gauge???
		readed = findfile->Read( buffer + unprocessed_bytes, search_step - unprocessed_bytes );
		found = SearchAtBuffer( buffer, search_step, target, size, type );
		unprocessed_bytes = size - 1;
		if(found >= 0)
			return offset+found;
		else
			offset +=readed - unprocessed_bytes;
		memmove(buffer, buffer + search_step - unprocessed_bytes, unprocessed_bytes);// moving unprocessed buffer to begining.
		}while(readed + unprocessed_bytes >= search_step);
	return -1;
	}

// TODO (death#9#): Implement better search algorithm. (Like GPGPU one using OpenCL)
uint64_t FindDialog::SearchAtBuffer( const char *bfr, int bfr_size, const char* search, int search_size, search_type_ type ){	// Dummy search algorithm\ Yes yes I know there are better ones but I think this enought for now.
	switch( type ){
		case type_hex:type_text_match_case:
			for(int i=0 ; i < bfr_size - search_size + 1 ; i++ )
				if(! memcmp( bfr+i, search, search_size ))
					return i;
			break;

		case type_text:
			char *bfrx = new char [bfr_size];
			for( int i = 0 ; i < bfr_size; i++)
				bfrx[i]=tolower(bfr[i]);
			char *searchx = new char [search_size];
			for( int i = 0 ; i < search_size; i++)
				searchx[i]=tolower(search[i]);
			for(int i=0 ; i < bfr_size - search_size + 1 ; i++ )
				if(! memcmp( bfr+i, search, search_size ))
					return i;
			break;
		}
	return -1;
	}
