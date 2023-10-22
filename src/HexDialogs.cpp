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
*               home  : www.wxhexeditor.org                             *
*               email : spamjunkeater@gmail.com                         *
*************************************************************************/

#define NANINT 0xFFFFFFFFFFFFFFFFULL
#include "HexDialogs.h"
#include <wx/progdlg.h>
#include <wx/filepicker.h>
#include "../mhash/include/mhash.h"

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#ifdef _OPENMP_
#include <omp.h>
#endif

#ifdef __WXMSW__
#define wxNewline wxT("\r\n")
#else
#define wxNewline wxT("\n")
#endif

extern int fake_block_size;

bool HexVerifyAndPrepare(wxString& hexval, wxString Value_Name, wxWindow* parent) {
	if( hexval.Len() < 2 )
		return false;
	//Remove all space chars and update the Search value
	int x = hexval.find(' ');
	while( x >= 0 ) { // using hexval.find(' ')>= 0 generate logic true due compiler error
		hexval.Remove( hexval.find(' '),1);
		x=hexval.find(' ');
		}

	for( unsigned i = 0 ; i < hexval.Len() ; i++ )
		if( !isxdigit( hexval[i] ) || hexval == ' ' ) { //Not hexadecimal!
			OSXwxMessageBox( _("Value is not hexadecimal!"), _("Format Error!"), wxOK, parent );
			wxBell();
			return false;
			}
	//there is odd hex value, must be even digit for search!
	if( hexval.Len() % 2 ) {
		OSXwxMessageBox( _("Value must be even digit!"), _("Format Error!"), wxOK, parent );
		wxBell();
		return false;
		}
	return true;
	}

//Prepares and updates combo boxes with 10 elements, sycnhronizing with wxConfig..
void ComboBoxFill( wxString KeyName, wxComboBox* CurrentBox, bool AddString) {
	wxString TempString;
	wxArrayString SearchStrings;
	//Prepare Array;
	///Note that, lower indices will shown at bottom, not at top!
	for( int i = 0 ; i < 10 ; i ++)
		if (myConfigBase::Get()->Read(KeyName+wxEmptyString<<i, &TempString)) {
			SearchStrings.Add( TempString );
#ifdef _DEBUG_
			std::cout << "ComboBoxFill Read (" << i << ") : " << TempString.ToAscii() << std::endl;
#endif
			}

	wxString AddNewString = CurrentBox->GetValue();
	//Adds New String
	if( AddString && (AddNewString != wxEmptyString) ) {
		if( SearchStrings.Index( AddNewString ) != wxNOT_FOUND )
			SearchStrings.Remove( AddNewString );
		else if( SearchStrings.Count() >= 10 )
			SearchStrings.RemoveAt(0);
		SearchStrings.Add( AddNewString );

		for( unsigned i = 0 ; i < 10 && i < SearchStrings.Count(); i ++) {
#ifdef _DEBUG_
			std::cout << "ComboBoxFill Write (" << i << ") : " << SearchStrings.Item(i).ToAscii() << std::endl;
#endif
			myConfigBase::Get()->Write(KeyName+wxEmptyString<<i,  SearchStrings.Item(i));
			}
		}
	//Set ComboBox
	unsigned i;
	for( i = 0 ; i < SearchStrings.Count()  ; i ++ )
		CurrentBox->SetString( i, SearchStrings.Item(SearchStrings.Count()-i-1) );
	for( ; i < 10 ; i ++ )
		CurrentBox->SetString( i, wxEmptyString);
	CurrentBox->SetSelection(0);
	}

GotoDialog::GotoDialog( wxWindow* _parent, uint64_t& _offset, uint64_t _cursor_offset, uint64_t _filesize, unsigned _BlockSize ):GotoDialogGui(_parent, wxID_ANY) {
	offset = &_offset;
	cursor_offset = _cursor_offset;
	is_olddec = true;
	filesize  = _filesize;
	BlockSize = _BlockSize;
	parent= static_cast< HexEditor* >(_parent);
	SetAutoLayout(true);

	int options=0;
	myConfigBase::Get()->Read( _T("GoToOptions"), &options );

	options & OPT_DEC_INPUT ? m_dec->SetValue(true) : m_hex->SetValue(true);
	/*	wxArrayString hex_letters;//=wxT("abcdefABCDEFxX");
		wxArrayString dec_letters;//=wxT("1234567890");
		wxString decStr=wxT("1234567890");
		for(int i=0;i<decStr.Len();i++)
			dec_letters.Add( decStr[i]);

		hex_letters=dec_letters;
		wxString hexStr=wxT("abcdefABCDEFxX");
		for(int i=0;i<hexStr.Len();i++)
			hex_letters.Add( hexStr[i] );
		wxString *x=new wxString();
		*x=wxT("0");

		m_comboBoxOffset->SetValidator( wxTextValidator( wxFILTER_INCLUDE_CHAR_LIST,  x) );
		static_cast<wxTextValidator*>(m_comboBoxOffset->GetValidator())->SetIncludes ( options & OPT_DEC_INPUT ? dec_letters : hex_letters );
	*/

	m_branch->SetSelection( options & OPT_BRANCH_NORMAL ? 0 :
	                        options & OPT_BRANCH_END ?		2 : 1);

	m_choiceMode->Clear();
	m_choiceMode->SetColumns(2);
	m_choiceMode->Append(_("Offset") );
	wxYield();
	m_choiceMode->SetSelection(0);
	if( BlockSize <= 1 )
		BlockSize=1;
	else {
		m_choiceMode->Append(_("Sector") );
		if(!(options & OPT_OFFSET_MODE)) {
			m_button_next->Show();
			m_button_prev->Show();
			Fit();
			wxYield();
			m_choiceMode->SetSelection( 1 );
			}
		}
	ComboBoxFill( _T("GotoOffset"), m_comboBoxOffset, false);
	m_comboBoxOffset->Select(0);
	m_comboBoxOffset->SetFocus();
	}

void GotoDialog::EventHandler( wxCommandEvent& event ) {
//	if( event.GetId() == m_dec->GetId()
//		|| event.GetId() == m_hex->GetId()
//		)
//		OnConvert( event );

	if( event.GetId()==m_choiceMode->GetId()) {
		m_button_next->Show(event.GetSelection());
		m_button_prev->Show(event.GetSelection());
//		m_staticline->Show(event.GetSelection());
//		m_staticTextSector->Show(event.GetSelection());
		Fit();
		wxYield();
		SetSize(DoGetBestSize());
		//Layout();
		}

	int options=0;
	options |= m_choiceMode->GetSelection() == 0 ? OPT_OFFSET_MODE : 0;
	options |= m_dec->GetValue() ? OPT_DEC_INPUT : 0;
	options |= m_branch->GetSelection() == 0 ? OPT_BRANCH_NORMAL :
	           m_branch->GetSelection() == 2 ? OPT_BRANCH_END : 0;

	myConfigBase::Get()->Write( _T("GoToOptions"), options );

	m_comboBoxOffset->SetFocusFromKbd();
	}

wxString GotoDialog::Filter( wxString text ) {
	for( unsigned i = 0 ; i < text.Length() ; i++ ) {
		if( !(m_dec->GetValue() ? isdigit( text.GetChar( i ) ) : isxdigit( text.GetChar( i ) )))
			text.Remove( i--, 1);
		}
	return text;
	}

void GotoDialog::OnInput( wxCommandEvent& event ) {
	wxString OldValue = m_comboBoxOffset->GetValue();
	wxString NewValue = Filter( OldValue );
	if( OldValue != NewValue ) {
		m_comboBoxOffset->SetValue( NewValue );
		m_comboBoxOffset->SetInsertionPointEnd();
		wxBell();
		}
	event.Skip(true);
	}

void GotoDialog::OnChar( wxKeyEvent& event ) {
	if(	( event.GetKeyCode() == WXK_BACK )
	      || ( event.GetKeyCode() == WXK_RETURN )
	      || ( event.GetKeyCode() == WXK_LEFT )
	      || ( event.GetKeyCode() == WXK_RIGHT )
	      || ( event.GetKeyCode() == WXK_DELETE )
	      || ( event.GetKeyCode() == WXK_TAB )
	      || ( event.GetKeyCode() == WXK_END )
	      || ( event.GetKeyCode() == WXK_HOME )
	      || ( event.GetKeyCode() == WXK_CAPITAL )
	      || ( event.GetKeyCode() == WXK_RETURN )
	      || ( event.ControlDown() && (
	              event.GetKeyCode()==1 			// CTRL+A
	              || event.GetKeyCode()==24	 		// CTRL+X
	              || event.GetKeyCode()==3			// CTRL+C
	              || event.GetKeyCode()==22 		// CTRL+V
	           ))
	      ||	( m_dec->GetValue() ? isdigit(event.GetKeyCode()) : isxdigit( event.GetKeyCode()) )
	  )
		event.Skip(true);
	else {
		wxBell();
		event.Skip(false);
		}
#ifdef _DEBUG_
	std::cout << "CTRL:" << event.ControlDown() << " KeyCode:" << event.GetKeyCode() << " Raw:" << event.GetRawKeyCode() << std::endl ;
#endif
	if( event.GetKeyCode() == WXK_ESCAPE ) {
		if(m_comboBoxOffset->GetValue().IsEmpty())
			Close();
		else
			m_comboBoxOffset->SetValue(wxEmptyString);
		}
	}

void GotoDialog::OnPreviousSector( wxCommandEvent& event ) {
	//parent->Goto( parent->CursorOffset() - BlockSize);
	parent->LoadFromOffset( parent->CursorOffset() - BlockSize );
	}
void GotoDialog::OnNextSector( wxCommandEvent& event ) {
	//parent->Goto( parent->CursorOffset() + BlockSize );
	parent->LoadFromOffset( parent->CursorOffset() + BlockSize );
	}

void GotoDialog::OnGo( wxCommandEvent& event ) {
	if( m_comboBoxOffset->GetValue()==wxEmptyString ) {
		wxBell();
		return;
		}
	wxULongLong_t wxul = 0;
	if( !m_comboBoxOffset->GetValue().ToULongLong( &wxul, m_dec->GetValue() ? 10 : 16 ))//Mingw32/64 workaround
		wxul = strtoull( m_comboBoxOffset->GetValue().ToAscii(), 0, m_dec->GetValue() ? 10 : 16 );
	*offset = wxul;

	//Store value to Registry or Updates it!
	ComboBoxFill( _T("GotoOffset"), m_comboBoxOffset, true);

	unsigned SectorSize=1;
	if( m_choiceMode->GetSelection() == 1)
		SectorSize=BlockSize;

	if( m_branch->GetSelection() == 1)
		*offset += cursor_offset*SectorSize;
	else if( m_branch->GetSelection() == 2) {
		if(filesize < *offset * SectorSize)
			*offset = 0;
		else
			*offset = filesize - *offset * SectorSize;
		}
	else
		*offset *= SectorSize;
	/*
		//No -locations due uint64_t
		if( *offset < 0 )
			*offset = 0;
	*/

	EndModal( wxID_OK );
	}

// TODO (death#1#):Paint 4 Find All
// TODO (death#1#):Remember options last state
FindDialog::FindDialog( wxWindow* _parent, FAL *_findfile, wxString title ):FindDialogGui( _parent, wxID_ANY, title) {
	parent = static_cast< HexEditor* >(_parent);
	findfile = _findfile;

	int options=0;
	myConfigBase::Get()->Read( _T("FindOptions"), &options );
	m_searchtype->SetSelection( options & SEARCH_TEXT ? 0 : 1);
	chkWrapAround->SetValue( options & SEARCH_WRAPAROUND );
	chkSearchBackwards->SetValue( options & SEARCH_BACKWARDS );
	chkUTF8->SetValue( options & SEARCH_UTF8 );
	chkMatchCase->SetValue( options & SEARCH_MATCHCASE );
	chkMatchCase->Enable( options & SEARCH_TEXT );

	//Load previous search results.
	PrepareComboBox( false );
	m_comboBoxSearch->Select(0);
	m_comboBoxSearch->SetFocus();

#ifdef _DEBUG_FIND_UNIT_TEST_
	SearchAtBufferUnitTest();
#endif
	}

bool FindDialog::SearchAtBufferUnitTest(void) {
	enum { STEP = 100000 };
	char buff[STEP];
	for(unsigned i=0; i<STEP; i++) buff[i]=0;
	wxString src = "keYWord";

	wxMemoryBuffer target;
	target.AppendData( src.Lower().ToAscii(), src.Len() );

	char ers[10];
	memset( ers, 0, 10 );
	for(unsigned i=0; i < STEP-src.Len() ; i++) {
		//bzero(buff, MB);
		unsigned sz = i + target.GetDataLen() > STEP ? STEP-i : target.GetDataLen();
		memcpy( buff+i, target, sz );
		UTF8SpeedHackChrs[0]=toupper(target[0]);
		UTF8SpeedHackChrs[1]=toupper(target[1]);

		std::cout << "\rSearching key at: " << i ;
		///Manully change OPTIONS for testing!
		//int f = SearchAtBuffer( buff, STEP, src, strlen(src), SEARCH_TEXT|SEARCH_MATCHCASE );
		//int f = SearchAtBuffer( buff, STEP, tolower(src), strlen(src), SEARCH_TEXT );
		//int f = SearchAtBuffer( buff, STEP, src, strlen(src), SEARCH_HEX|SEARCH_BACKWARDS|SEARCH_MATCHCASE );
		int f = SearchAtBuffer( buff, STEP, (char*)target.GetData(), target.GetDataLen(), SEARCH_TEXT|SEARCH_BACKWARDS );

		memcpy( buff+i, ers, sz );
		if( f == static_cast<int>(i) )
			std::cout << " OK";
		else {
			std::cout << "ERROR \nFor key at : "<< i << "\t result = " << f << "\t sz: " << sz;
			return false;
			}
		std::cout.flush();
		}
	std::cout << std::endl;
	return true;
	}

void FindDialog::PrepareComboBox( bool AddString ) {
	int searchType = m_searchtype->GetSelection() == 0 ? SEARCH_TEXT : SEARCH_HEX;
	if( searchType == SEARCH_TEXT )
		ComboBoxFill( _T("SearchTextString"), m_comboBoxSearch, AddString);
	else
		ComboBoxFill( _T("SearchHexString"), m_comboBoxSearch, AddString);
	}

void FindDialog::OnChar( wxKeyEvent& event ) {
	if( m_searchtype->GetSelection()==1 ) { //Just for Hex Mode
		if(	( event.GetKeyCode() == WXK_BACK )
		      || ( event.GetKeyCode() == WXK_RETURN )
		      || ( event.GetKeyCode() == WXK_LEFT )
		      || ( event.GetKeyCode() == WXK_RIGHT )
		      || ( event.GetKeyCode() == WXK_DELETE )
		      || ( event.GetKeyCode() == WXK_TAB )
		      || ( event.GetKeyCode() == WXK_END )
		      || ( event.GetKeyCode() == WXK_HOME )
		      || ( event.GetKeyCode() == WXK_CAPITAL )
		      || ( event.GetKeyCode() == WXK_RETURN )
		      || ( event.ControlDown() && (
		              event.GetKeyCode()==1 			// CTRL+A
		              || event.GetKeyCode()==24	 		// CTRL+X
		              || event.GetKeyCode()==3			// CTRL+C
		              || event.GetKeyCode()==22 		// CTRL+V
		           ))
		      ||	( isxdigit( event.GetKeyCode()) )
		  )
			event.Skip(true);
		else {
			wxBell();
			event.Skip(false);
			}
		}
	else {
		event.Skip( true );
		}

#ifdef _DEBUG_
	std::cout << "CTRL:" << event.ControlDown() << " KeyCode:" << event.GetKeyCode() << " Raw:" << event.GetRawKeyCode() << std::endl ;
#endif
	if( event.GetKeyCode() == WXK_ESCAPE ) {
		if( static_cast<wxComboBox*>(event.GetEventObject())->GetValue().IsEmpty() )
			Close();
		else
			static_cast<wxComboBox*>(event.GetEventObject())->SetValue(wxEmptyString);
		}
	}

void FindDialog::EventHandler( wxCommandEvent& event ) {
	WX_CLEAR_ARRAY(parent->HighlightArray);
	parent->HighlightArray.Shrink();

	if( event.GetId() == btnFind->GetId())
		OnFind();
	else if(event.GetId() == m_comboBoxSearch->GetId()) {
		if( event.GetEventType() == 10041)//Handles ENTER key
			OnFind();
		else
			chkUTF8->SetValue( !m_comboBoxSearch->GetValue().IsAscii() );
		}
	else if( event.GetId() == m_searchtype->GetId()) {
		chkMatchCase->Enable(m_searchtype->GetSelection() == 0);

		PrepareComboBox( false );
		m_comboBoxSearch->SetValue( wxEmptyString );//For clearing current value to make selection operation proper
		m_comboBoxSearch->SetSelection(0);
		m_comboBoxSearch->SetFocus();
		}
	else if( event.GetId() == btnFindAll->GetId() )
		OnFindAll();
	else if(event.GetId() == btnFindSomeBytes->GetId())
		FindSomeBytes();

	//Disables chkUTF8 setting by user.
	else if( event.GetId() == chkUTF8->GetId() ) {
		chkUTF8->SetValue( !chkUTF8->GetValue( ));
		wxBell();
		}

	int options=0;
	options |= m_searchtype->GetSelection() == 0 ? SEARCH_TEXT : SEARCH_HEX;
	options |= chkWrapAround->GetValue() ? SEARCH_WRAPAROUND : 0;
	options |= chkSearchBackwards->GetValue() ? SEARCH_BACKWARDS : 0;
	options |= chkUTF8->GetValue() ? SEARCH_UTF8 : 0;
	options |= chkMatchCase->GetValue() ? SEARCH_MATCHCASE : 0;
	myConfigBase::Get()->Write( _T("FindOptions"), options );
	}

void FindDialog::FindSomeBytes( void ) {
	if( findfile->Length() == 0)
		return;
	wxString msg= _("Finding Some Bytes...");
	wxString emsg=wxT("\n");
	wxProgressDialog progress_gauge(_("wxHexEditor Searching"), msg+emsg, 1000,  this, wxPD_SMOOTH|wxPD_REMAINING_TIME|wxPD_CAN_ABORT|wxPD_AUTO_HIDE );
	progress_gauge.SetWindowStyleFlag( progress_gauge.GetWindowStyleFlag()|wxSTAY_ON_TOP|wxMINIMIZE_BOX );
// TODO (death#1#): Search icon
	//wxIcon search_ICON (?_xpm);
	//progress_gauge.SetIcon(search_ICON);

	uint64_t current_offset = parent->CursorOffset();
	unsigned BlockSz= 10*MB;
	unsigned search_step = findfile->Length() < BlockSz ? findfile->Length() : BlockSz ;
	findfile->Seek( current_offset, wxFromStart );
	char* buffer = new char [search_step];
	if(buffer == NULL) return;
	// TODO (death#6#): insert error check message here
	int readed = 0;
	char diff_search;
	findfile->Read( &diff_search, 1);
	time_t ts,te;
	time (&ts);
	te=ts;
	uint64_t read_speed=0;
	//From cursor to file end.
	//backward search
	unsigned options = chkSearchBackwards->GetValue() ? SEARCH_BACKWARDS : 0;
	uint64_t backward_offset;
	if( options & SEARCH_BACKWARDS )
		do {
			backward_offset = current_offset < search_step ? 0 : current_offset-search_step;
			search_step = backward_offset + search_step > current_offset ? current_offset-backward_offset-1 : search_step;
			findfile->Seek( backward_offset, wxFromStart );
			readed=findfile->Read( buffer, search_step );
			read_speed += readed;
			for( int i=readed; i >= 0 ; i--)
				if( buffer[i] != diff_search ) {
					parent->Goto( backward_offset+i );
					return;
					}
			time(&te);
			if(ts != te ) {
				ts=te;
				emsg = msg + wxT("\n") + _("Search Speed : ") + wxString::Format( wxT("%.2f "), 1.0*read_speed/MB) + _("MB/s");
				read_speed=0;
				}
			if( ! progress_gauge.Update((findfile->Length()-current_offset)*1000/findfile->Length(), emsg))		// update progress and break on abort
				break;

			current_offset += readed;
			}
		while(backward_offset > 0);
	else
		do {
			findfile->Seek( current_offset, wxFromStart );
			readed = findfile->Read( buffer, search_step );
			read_speed += readed;
			for( int i=0; i < readed ; i++)
				if( buffer[i] != diff_search ) {
					parent->Goto( current_offset+i );
					//Destroy();
					return;
					}

			time(&te);
			if(ts != te ) {
				ts=te;
				emsg = msg + wxT("\n") + _("Search Speed : ") + wxString::Format( wxT("%.2f "), 1.0*read_speed/MB) + _("MB/s");
				read_speed=0;
				}
			if( ! progress_gauge.Update(current_offset*1000/findfile->Length(), emsg))		// update progress and break on abort
				break;

			current_offset += readed;
			}
		while(static_cast<unsigned>(readed) >= search_step);  //indicate also file end.
	wxBell();
	}

bool FindDialog::OnFind( bool internal ) {
#ifdef _DEBUG_
	std::cout << "FindDialog::OnFind() " << std::endl;
#endif
#ifdef _DEBUG_FIND_UNIT_TEST_
	return FindBinaryUnitTest();
#endif // _DEBUG_FIND_UNIT_TEST_

	uint64_t found = NANINT;
	uint64_t search_size = 0;
	//prepare Operator
	unsigned options = 0;
	options |= m_searchtype->GetSelection() == 0 ? SEARCH_TEXT : SEARCH_HEX;
	options |= chkWrapAround->GetValue() ? SEARCH_WRAPAROUND : 0;
	options |= chkSearchBackwards->GetValue() ? SEARCH_BACKWARDS : 0;
	if(options & SEARCH_TEXT) {
		PrepareComboBox( true );
		options |= chkUTF8->GetValue() ? SEARCH_UTF8 : 0;
		options |= chkMatchCase->GetValue() ? SEARCH_MATCHCASE : 0;
		if( m_comboBoxSearch->GetValue().IsAscii() )
			search_size = m_comboBoxSearch->GetValue().Len();
		else {
			search_size=0;
			while( m_comboBoxSearch->GetValue().ToUTF8()[search_size++]);
			search_size--;
			}
		found = FindText( m_comboBoxSearch->GetValue(), parent->CursorOffset()+(options&SEARCH_BACKWARDS ? 0:1), options ); //+1 for forward operations!
		}
	else { //SEARCH_HEX
		//Hex Validation and Format
		wxString hexval = m_comboBoxSearch->GetValue();
		if(!HexVerifyAndPrepare( hexval, _("Search"), this ) )
			return false;
		m_comboBoxSearch->SetValue(hexval.Upper());

		PrepareComboBox( true );
		wxMemoryBuffer search_binary = wxHexCtrl::HexToBin( m_comboBoxSearch->GetValue());
		search_size = search_binary.GetDataLen();
		found = FindBinary( search_binary, parent->CursorOffset()+(options&SEARCH_BACKWARDS ? 0:1), options ); //+1 for forward operations!
		}

	if( found != NANINT ) {
		parent->Goto( found );
		parent->Select( found,  found+search_size-1 );
		return true;
		}

	else if( !internal )
		OSXwxMessageBox(_("Search value not found"), _("Nothing found!"), wxOK, this );
	return false;
	}

uint64_t FindDialog::FindText( wxString target, uint64_t start_from, unsigned options ) {
#ifdef _DEBUG_
	std::cout << "FindText() from: " << start_from << "\t Search Length " << target.Len() << std::endl;
#endif

	if(!(options & SEARCH_MATCHCASE)) {
		//!Compare with lowered characters
		target = target.Lower();
		}

	wxMemoryBuffer textsrc;
	if( target.IsAscii() ) {
		textsrc.AppendData( target.ToAscii(), target.Length() );
		return FindBinary( textsrc, start_from, options );
		}

	else { //Search as UTF string.
		wxCharBuffer a = target.ToUTF8().data(); //Convert to UTF8 Binary
		int i=0;
		char *b=a.data();							//Silences errors
		while(b[i++] != 0);					//Find stream size
		textsrc.AppendData( a, i-1 ); //-1 for discard null termination char

		///Since we cannot convert whole buffer to UTF8 for issuing upper() function on it,
		///we just need "search string" upper & lower versions.
		// First 2 byte will be searched and if matches, full comparison will made.
		if(!(options & SEARCH_MATCHCASE)) { //If non-matchcase operation
			wxCharBuffer a = target.Upper().ToUTF8().data(); 	//Calculate Uppercase variant of first character
			UTF8SpeedHackChrs[0]=a.data()[0];						//This will be huge speed up but needed to handled differently on
			UTF8SpeedHackChrs[1]=a.data()[1];						//SearchAtBuffer stack!
			}
		return FindBinary( textsrc, start_from, options|SEARCH_UTF8 );
		}
	}

bool FindDialog::SkipRAM( uint64_t& current_offset, unsigned search_size, unsigned& search_step, bool backward ) {
	uint64_t map_start;
	uint64_t map_end;
#ifdef _DEBUG_
	printf( "SkipRAM() Processing from 0x%lX, %s\n", current_offset, (backward ? "Backward": ""));
#endif
	if( !parent->ProcessRAM_FindMap( current_offset, map_start, map_end, backward ) ) {
		current_offset = parent->ProcessRAM_FindNextMap(current_offset, backward);
#ifdef _DEBUG_
		printf( "Skipping to 0x%lX\n", current_offset);
#endif
		}

	if(backward) {
		if( current_offset - map_start < search_step )
			search_step = current_offset - map_start;
		}
	else {
		if( map_end - current_offset > search_step )
			search_step = map_end - current_offset;
		}

	if( search_step < search_size) {
		current_offset = parent->ProcessRAM_FindNextMap(current_offset, backward);
#ifdef _DEBUG_
		printf( "Skipping to 0x%lX due step size.\n", current_offset);
#endif
		}
#ifdef _DEBUG_
	std::cout << "Step size:" << search_step << std::endl;
#endif

	if( map_start <= current_offset && map_end >= current_offset ) {
		std::cout << "ProcessRAM_FindMap pass\n" << std::endl;
		}
	if( current_offset == 0 ) //means memory map ends
		return true;
	return false;
	}

#define _MULTI_SEARCH_
// TODO (death#1#):
#ifdef _DEBUG_FIND_UNIT_TEST_
bool FindDialog::FindBinaryUnitTest( void ) {
#ifdef _DEBUG_
	std::cout << "FindDialog::FindBinaryUnitTest() " << std::endl;
#endif
	wxFileName a(wxT("tempfile-deleteable"));
	wxFile b(a.GetFullPath(),wxFile::write );

	int fs=3*MB;
	char *buff= new char[fs];
	memset( buff, 0, fs );
	b.Write( buff, fs );
	b.Close();
	b.Flush();

	FAL tmpfile(a, FAL::ReadWrite ) ;
	findfile=&tmpfile;

	//somehow, FAL->Length return 0 for some time
	while(findfile->Length() <= 0);

	wxString teststr="testSTr";
	wxMemoryBuffer target;
	//For No_MATCH_CASE!
	//teststr=teststr.Lower()
	target.AppendData( teststr.ToAscii(), teststr.Len() );

	for(int i=fs-strlen(teststr); i > 0 ; i-- )  //for Forward Search, put search string to last possible location first
		//for(int i=0; i < fs - 7; i++ ) //for Backward Search, put search string to begining of the file first
		{
		//	tmpfile.Seek( i );
		//	tmpfile.Write( teststr, 7 );
		tmpfile.Add( i, teststr, strlen(teststr) );
		//not need to commit to real file
		//tmpfile.Apply();

		std::cout << "\rSearching key at: " << i ;
		///Test Cases
		int f = FindBinary( target, 0, SEARCH_TEXT | SEARCH_MATCHCASE );
		//int f = FindBinary( target, 0, SEARCH_TEXT );
		//int f = FindBinary( target, findfile->Length()/2, SEARCH_TEXT | SEARCH_MATCHCASE | SEARCH_WRAPAROUND );

		//int f = FindBinary( target, tmpfile.Length()-1, SEARCH_TEXT | SEARCH_MATCHCASE | SEARCH_BACKWARDS);
		//int f = FindBinary( target, findfile->Length()/2, SEARCH_TEXT | SEARCH_MATCHCASE | SEARCH_BACKWARDS | SEARCH_WRAPAROUND ); //SSE2 OK

		tmpfile.Undo();//remove last mod

		if( f == i )
			std::cout << " OK";
		else {
			std::cout << "ERROR \nFor key at : "<< i << "\t result = " << f;
			return false;
			}
		std::cout.flush();
		}
	std::cout << std::endl;
	return false;
	}
#endif // _DEBUG_FIND_UNIT_TEST_

uint64_t FindDialog::FindBinary( wxMemoryBuffer target, uint64_t from, unsigned options ) {
#ifdef _DEBUG_
	std::cout << "FindDialog::FindBinary() From:" << from << std::endl;
#endif
	if( target.GetDataLen() == 0 ) {
		OSXwxMessageBox( wxT("FindBinary() function called with Empty Target!\n"), _("Error"), wxOK, this);
		return NANINT;
		}

	//Disabling search on empty files and if fileLength smaller than file size.
	if( findfile->Length() == 0 || findfile->Length() < static_cast<int>( target.GetDataLen() ))
		return NANINT;
#ifndef _DEBUG_FIND_UNIT_TEST_
	wxString msg= _("Finding matches...");
	wxString emsg=wxT("\n");
	wxProgressDialog progress_gauge(_("wxHexEditor Searching"), msg+emsg, 1000,  this, wxPD_SMOOTH|wxPD_REMAINING_TIME|wxPD_CAN_ABORT|wxPD_AUTO_HIDE );
	progress_gauge.SetWindowStyleFlag( progress_gauge.GetWindowStyleFlag()|wxSTAY_ON_TOP|wxMINIMIZE_BOX );
#endif //_DEBUG_FIND_UNIT_TEST_
// TODO (death#1#): Search icon
	//wxIcon search_ICON (?_xpm);
	//progress_gauge.SetIcon(search_ICON);

	// TODO (death#6#): insert error check message here

	int found = -1;
	uint64_t totalread=0;
	uint64_t current_offset = from;

	//SEARCH_FINDALL forces Forward searching
	if(( options & SEARCH_FINDALL ) || !(options & SEARCH_BACKWARDS) ) {
		if( options & SEARCH_FINDALL )
			current_offset=0;
#ifdef _DEBUG_FIND_UNIT_TEST_
		wxString msg;
		found = FindBinaryForward(target, current_offset, findfile->Length(), options, NULL, msg, totalread);
#else
		found = FindBinaryForward(target, current_offset, findfile->Length(), options, &progress_gauge, msg, totalread );
#endif //_DEBUG_FIND_UNIT_TEST_
		if(found >= 0)
			return found;

		//Second round from file end to current offset
		if(( options & SEARCH_WRAPAROUND ) && !( options & SEARCH_FINDALL ))
#ifdef _DEBUG_FIND_UNIT_TEST_
			return FindBinaryForward(target, 0, current_offset, options, NULL, msg, totalread );
#else
			return FindBinaryForward(target, 0, current_offset, options, &progress_gauge, msg, totalread );
#endif
		}

	// TODO (death#1#): MemorySearch Backward.
	//BACKWARD SEARCH!
	else {
		//First search from current offset to 0
#ifdef _DEBUG_FIND_UNIT_TEST_
		wxString msg;
		found = FindBinaryBackward(target, current_offset, 0, options, NULL, msg, totalread);
#else
		found = FindBinaryBackward(target, current_offset, 0, options, &progress_gauge, msg, totalread );
#endif //_DEBUG_FIND_UNIT_TEST_
		if(found >= 0)
			return found;
		//Second round from file end to current offset
		if( options & SEARCH_WRAPAROUND )
#ifdef _DEBUG_FIND_UNIT_TEST_
			return FindBinaryBackward(target, findfile->Length(), current_offset, options, NULL, msg, totalread );
#else
			return FindBinaryBackward(target, findfile->Length(), current_offset, options, &progress_gauge, msg, totalread );
#endif
		}
	return NANINT;
	}

uint64_t FindDialog::FindBinaryForward(wxMemoryBuffer target, uint64_t from, uint64_t to_offset, unsigned options,
                                       wxProgressDialog* progress_gauge, wxString& gauge_msg, uint64_t& totalread) {
#ifdef _DEBUG_
	std::cout << "FindDialog::FindBinaryForward() From:" << from << " to: " << to_offset << std::endl;
#endif
	if(from>to_offset) {
		std::cout << "FindDialog::FindBinaryForward() called with" << from <<">"<< to_offset << std::endl;
		return -1;
		}

	//bool first_run=true;
	wxString emsg = gauge_msg + wxT("\n") ;
	uint64_t current_offset = from;

	if( options & SEARCH_FINDALL )
		current_offset = 0;

	unsigned BlockSz = MB;
	unsigned search_step = findfile->Length() < BlockSz ? findfile->Length() : BlockSz ;

	char* buffer = new char [search_step];
	if(buffer == NULL)
		return NANINT;

	char* buffer_prefetch = new char [search_step];
	if(buffer_prefetch==NULL) {
		delete [] buffer;
		buffer = NULL;
		return NANINT;
		}
	time_t ts,te;
	time (&ts);
	te=ts;
	int found = -1, readed = -1,readed_prefetch = -1;
	//unsigned read_speed=0,
	unsigned   percentage=0;
	uint64_t read_speed_offset=0,processfootprint=0;

	if( findfile->IsProcess() )
		processfootprint = parent->ProcessRAM_GetFootPrint();

	findfile->Seek( current_offset, wxFromStart );

	std::vector< int > partial_results;
	std::vector< uint64_t > results;

	do {
		//Skipping unmapped regions if ProcessRAM operation
		if( findfile->IsProcess() ) {
			search_step=BlockSz;
			//if( SkipRAM( current_offset, target.GetDataLen(), search_step ) ) //SkipRAM adjust current_offset and search_step
			//	break;
			uint64_t map_start,map_end;
			if( !parent->ProcessRAM_FindMap( current_offset, map_start, map_end ) ) {
#ifdef _DEBUG_
				printf( "Skipping from: 0x%lX to", current_offset) ;
#endif
				current_offset = parent->ProcessRAM_FindNextMap(current_offset);
#ifdef _DEBUG_
				printf( "0x%lX\n", current_offset) ;
#endif
				if( current_offset == 0 ) break;
				}
			/*
						if( map_end - current_offset < search_step )
							search_step = map_end - current_offset;

						if( search_step < search_size){
							#ifdef _DEBUG_
							std::cout << "Skipping due StepSize: " << current_offset <<  " to " ;
							#endif
							current_offset = parent->ProcessRAM_FindNextMap(current_offset);
							#ifdef _DEBUG_
							std::cout << current_offset << std::ios::dec << std::endl;
							#endif
							}
			*/
			}

		if(readed_prefetch>0) { //PreLoad file, +%50 speed on SSD buffers.
			readed=readed_prefetch;
			std::swap(buffer, buffer_prefetch);
			}
		else {
			findfile->Seek( current_offset, wxFromStart );
			readed = findfile->Read( buffer, search_step );
			}
		//read_speed += readed;
#ifdef _DEBUG_
		std::cout << "Readed: " << readed << std::endl;
		printf( "FindBinaryForward() 0x%lX - 0x%lX\n", current_offset, current_offset+search_step);
#endif

// NON-MULTI SEARCH
//		found = SearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options );//Makes raw search here
//		if(found >= 0){//We found something
//			if( options & SEARCH_FINDALL ){
//				TagElement *mytag=new TagElement(current_offset+found, current_offset+found+target.GetDataLen()-1,wxEmptyString,*wxBLACK, wxColour(255,255,0,255) );
//				parent->HighlightArray.Add(mytag);
//				current_offset += found+target.GetDataLen(); //Unprocessed bytes remaining at buffer!!!
//				readed=search_step; //to stay in loop
//				}
//			else{
//				delete [] buffer;
//				delete [] buffer_prefetch;
//				return current_offset+found;
//				}
//			}
//		else{
//			int z = readed - target.GetDataLen() -1;
//			current_offset += (z <= 0 ? 1 : z); //for unprocessed bytes
//			}

		partial_results.clear();
#define _READ_PREFETCH_
#ifdef _READ_PREFETCH_
		#pragma omp parallel sections
			{
			//this preloads next data to swap buffer.
			#pragma omp section
				{
				//precalculate next read location
				findfile->Seek( current_offset+readed-target.GetDataLen()+1, wxFromStart );
				readed_prefetch=findfile->Read( buffer_prefetch, search_step );
				}

			#pragma omp section
				{
				//found = MultiThreadMultiSearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options, &partial_results );//Makes raw search here
				found = SearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options, &partial_results );//Makes raw search here
#ifdef _DEBUG_
				std::cout << "OMP search found buffer offset: " <<  found << std::endl;
#endif
				}
			}
#else	 //code without prefetching
		found = /*MultiThread*/SearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options, &partial_results );//Makes raw search here
#endif //_READ_PREFETCH_

		if (found >= 0) found += current_offset;
		if( partial_results.size() > 0 ) { // Houston, We found something!
			if( options & SEARCH_FINDALL ) {
				for(unsigned i=0 ; i < partial_results.size() ; i++ )
					results.push_back( partial_results[i] + current_offset );
				//current_offset +=  partial_results.back()+target.GetDataLen();
				current_offset += readed-target.GetDataLen()+1;
				}
			//If only searching one result, throw first one.
			else {
				delete [] buffer;
				delete [] buffer_prefetch;
				return current_offset+partial_results.front();
				}
			}
		else {
			int offset_increase = readed - target.GetDataLen() +1;
			current_offset += (offset_increase <= 0 ? 1 : offset_increase); //for unprocessed bytes
#ifdef _DEBUG_
			std::cout << "Offset Increased: " << offset_increase << std::endl;
#endif
			}

#ifndef _DEBUG_FIND_UNIT_TEST_
		///Progress dialog and while loop from now...
		time(&te);
		if(ts != te ) {
			ts=te;
			emsg = gauge_msg + wxT("\n") + _("Search Speed : ") + wxString::Format( wxT("%.2f "), 1.0*(current_offset-read_speed_offset)/MB) + _("MB/s");
			read_speed_offset = current_offset;
			}

		totalread += readed; // We need this on step 2
		if( findfile->IsProcess() ) {
#ifdef _DEBUG_
			printf( "ProcessRAM Virtual Offset: 0x%lX, \t Offset:0x%lX,  \tFootPrint: 0x%lX\n", parent->ProcessRAM_GetVirtualOffset( current_offset ),	current_offset, processfootprint);
#endif
			percentage = parent->ProcessRAM_GetVirtualOffset( current_offset )*1000/processfootprint;
			}

		else
			percentage = ( options & SEARCH_WRAPAROUND && !( options & SEARCH_FINDALL ))
			             ? (current_offset-from)*1000/(findfile->Length())
			             : (current_offset-from)*1000/(findfile->Length()-from+1);//+1 to avoid error

		if( ! progress_gauge->Update( percentage, emsg))		// update progress and break on abort
			break;
#endif
		}
	while( (to_offset > current_offset + target.GetDataLen()) && (readed == static_cast<int>(search_step)));  //indicate also file end.

	//WX_CLEAR_ARRAY( parent->HighlightArray )
	//parent->HighlightArray.Shrink();

	delete [] buffer;
	delete [] buffer_prefetch;
	if (! (options & SEARCH_FINDALL))
		return found;

	//Create tags from results and put them into HighlightArray
	for(unsigned i=0 ; i < results.size() &&  i < 100000 ; i++ ) {
		TagElement *mytag=new TagElement(results[i], results[i]+target.GetDataLen()-1,wxEmptyString,*wxBLACK, wxColour(255,255,0,255) );
		parent->HighlightArray.Add(mytag);
		}

	if(results.size() > 0)
		found = results[0];

	partial_results.clear();
	results.clear();

	return found;
	}

uint64_t FindDialog::FindBinaryBackward(wxMemoryBuffer target, uint64_t from, uint64_t to_offset, unsigned options,
                                        wxProgressDialog* progress_gauge, wxString& gauge_msg, uint64_t& totalread) {
	//BACKWARD SEARCH!
	bool first_run=true;
	wxString emsg = gauge_msg + wxT("\n") ;
	uint64_t current_offset = from ; //higher offset
	uint64_t end_offset = to_offset; //lower offset due backward search
	if( current_offset < end_offset )
		wxSwap( current_offset, end_offset);
	uint64_t backward_offset = current_offset;

	//bool first_search=true;
	unsigned BlockSz = MB;
	unsigned search_step = findfile->Length() < BlockSz ? findfile->Length() : BlockSz ;

	char* buffer = new char [search_step];
	if(buffer == NULL) return NANINT;
	// TODO (death#6#): insert error check message here
	int found = -1, readed = -1;

	char* buffer_prefetch = new char [search_step];
	if(buffer_prefetch == NULL) {
		delete [] buffer;
		return NANINT;
		}
	int readed_prefetch = -1;

	time_t ts,te;
	time (&ts);
	te=ts;

	std::vector< int > partial_results;

	unsigned read_speed=0,percentage=0;
	//uint64_t read_speed_offset=0
	uint64_t   processfootprint=0;
	if( findfile->IsProcess() )
		processfootprint = parent->ProcessRAM_GetFootPrint();
	do {
		if( findfile->IsProcess() ) {
			search_step=BlockSz;
			if( SkipRAM( current_offset, target.GetDataLen(), search_step, true ) ) //SkipRAM adjust current_offset and search_step
				break;
			}
		backward_offset = current_offset < search_step ? 0 : current_offset-search_step;
		search_step = backward_offset + search_step > current_offset ? current_offset-backward_offset : search_step;

#ifdef _DEBUG_
		std::cout << std::dec << "FindBinary() BACKWARD " << backward_offset << "-" << current_offset << " \t " << "SearchStep:" << search_step<< std::endl;
#endif
		if(readed_prefetch>0) { //PreLoad file, +%50 speed on SSD buffers.
			readed=readed_prefetch;
			std::swap(buffer, buffer_prefetch);
			}
		else {
			findfile->Seek( backward_offset, wxFromStart );
			readed=findfile->Read( buffer, search_step );
			}

		read_speed += readed;

#ifdef _READ_PREFETCH_xxx
		#pragma omp parallel sections
			{
			//this preloads next data to swap buffer.
			#pragma omp section
				{
				//precalculate next read location from original functions:
				//current_offset = backward_offset + target.GetDataLen();
				//backward_offset = current_offset < search_step ? 0 : current_offset-search_step;
				//search_step = backward_offset + search_step > current_offset ? current_offset-backward_offset : search_step;

				uint64_t current_offset_prefech = backward_offset + target.GetDataLen();
				uint64_t backward_offset_prefetch = current_offset_prefech < search_step ? 0 : current_offset_prefech-search_step;
				unsigned search_step_prefetch = backward_offset_prefetch + search_step > current_offset_prefech ? current_offset_prefech-backward_offset_prefetch : search_step;
				if (backward_offset_prefetch > end_offset) {
					findfile->Seek( backward_offset_prefetch, wxFromStart );
					readed_prefetch=findfile->Read( buffer_prefetch, search_step_prefetch );
					}
				else
					readed_prefetch=0;
				}

			#pragma omp section
				{
				//we do not need MultiSearch here. Because it's not a SEARCH_FINDALL
				found = SearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options );
				}
			}
#else	 //code without prefetching

		//Since it's not possible SEARCH_FINDALL and SEARCH_BACKWARD flags same time, we don't need MultiSearchAtBufer function.
		found = SearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options, &partial_results );
#endif //_READ_PREFETCH_

		if(found >= 0) {
			delete [] buffer;
			delete [] buffer_prefetch;
			return backward_offset+found;
			}
		else
			current_offset = backward_offset + target.GetDataLen(); //Unprocessed bytes

#ifndef _DEBUG_FIND_UNIT_TEST_ // Unit test will disable gauge.
		//Gauge update
		time(&te);
		if(ts != te || first_run ) {
			first_run=false;
			ts=te;
			emsg = gauge_msg + wxT("\n") + _("Search Speed : ") + wxString::Format( wxT("%.2f "), (1.0*read_speed/MB )) + _("MB/s");
			read_speed=0;
			}
		totalread += readed;

		if( findfile->IsProcess() ) {
			percentage = options & SEARCH_WRAPAROUND
			             ? (processfootprint-totalread)*1000 / processfootprint
			             :	parent->ProcessRAM_GetVirtualOffset(current_offset)*1000/(parent->ProcessRAM_GetVirtualOffset(from)+1); //+1 to avoid error
			percentage = percentage > 1000 ? 1000 : percentage;
			}
		else {
			percentage =  (findfile->Length()-totalread)*1000/findfile->Length();
			percentage = percentage > 1000 ? 1000 : percentage;
			}
		if( progress_gauge != NULL) {
			if(first_run) {
				if( ! progress_gauge->Update( percentage ) )		// update progress and break on abort
					break;
				}
			else if( ! progress_gauge->Update( percentage, emsg))		// update progress and break on abort
				break;
			}
#endif //_DEBUG_FIND_UNIT_TEST_
		}
	while(backward_offset > end_offset);

	delete [] buffer;
	delete [] buffer_prefetch;
	return NANINT;
	}

void FindDialog::OnFindAll( bool internal ) {
	for( unsigned i=0; i < parent->HighlightArray.Count(); i++ )
		delete parent->HighlightArray.Item(i);
	parent->HighlightArray.Clear();

	unsigned options = SEARCH_FINDALL; //fill continue search until file and with this option.
	options |= m_searchtype->GetSelection() == 0 ? SEARCH_TEXT : SEARCH_HEX;
	options |= chkWrapAround->GetValue() ? SEARCH_WRAPAROUND : 0;
	options |= chkSearchBackwards->GetValue() ? SEARCH_BACKWARDS : 0;

	//Text Search
	if(options & SEARCH_TEXT) {
		PrepareComboBox( true );
		options |= chkUTF8->GetValue() ? SEARCH_UTF8 : 0;
		options |= chkMatchCase->GetValue() ? SEARCH_MATCHCASE : 0;

		///FindAll discards SEARCH_BACKWARDS and SEARCH_WRAPAROUND options.
		FindText( m_comboBoxSearch->GetValue(), 0, options );

		/*
		if( options & SEARCH_WRAPAROUND )
			FindText( m_comboBoxSearch->GetValue(), 0, options );
		else if( options & SEARCH_BACKWARDS )
			FindText( m_comboBoxSearch->GetValue(), parent->CursorOffset() , options );
		else
			FindText( m_comboBoxSearch->GetValue(), parent->CursorOffset()+1 , options );
		*/
		}

	//Hexadecimal / Binary Search
	else {
		wxString hexval = m_comboBoxSearch->GetValue();
		//parent->Goto(0);
		if(!HexVerifyAndPrepare( hexval, _("Search"), this ) )
			return;
		m_comboBoxSearch->SetValue(hexval.Upper());
		PrepareComboBox( true );
		FindBinary( wxHexCtrl::HexToBin( m_comboBoxSearch->GetValue()), 0,options );
		}

	if(parent->HighlightArray.GetCount()==0) {
		OSXwxMessageBox(_("Search value not found"), _("Nothing found!"), wxOK, this );
		}
	else {
		parent->Goto( parent->HighlightArray.Item(0)->start );

		if( !internal ) {
			this->Hide();
			OSXwxMessageBox(wxString::Format(_("Found %d matches."), static_cast<int>( parent->HighlightArray.GetCount()) ), _("Find All Done!"), wxOK, parent );
			#ifdef __WXMAC__
				EndModal(0);
			#endif
			//this->Show();
			}
		}

	wxUpdateUIEvent eventx( SEARCH_CHANGE_EVENT );
	wxPostEvent(parent, eventx );
	}

//inline int FindDialog::MultiThreadMultiSearchAtBuffer( char *bfr, int bfr_size, char* search, int search_size, unsigned options, std::vector<int> *ret_ptr ){
//#define _DUAL_THREAD_SEARCH_
//
//#ifdef _DUAL_THREAD_SEARCH_
//	///DualThread
//	std::vector<int> v1,v2;
//	#pragma omp parallel sections // starts a new team
//	{
//		#pragma omp section
//		{MultiSearchAtBuffer( bfr, bfr_size/2, search, search_size, options, &v1 );}
//		#pragma omp section
//		{MultiSearchAtBuffer( bfr+ bfr_size-bfr_size/2-search_size+1,bfr_size/2+search_size-1, search, search_size, options , &v2);}
//	}
//	int cnt;
//	cnt=v1.size();
//	for( int i=0; i<cnt; i++)
//		ret_ptr->push_back(v1[i]);
//	cnt=v2.size();
//	for( int i=0; i<cnt; i++)
//		ret_ptr->push_back(v2[i]+bfr_size-bfr_size/2-search_size+1);
//
//	return ret_ptr->size();
//
//#else
/////QuadThread
//	int ztep=bfr_size/4;
//	std::vector<int> v1,v2,v3,v4;;
///*
//	#ifndef OMP_H
//	return SearchAtBufferSingleThread( bfr, bfr_size, search, search_size, options );
//	#else
//*/
//	if( ztep < search_size*128 )	//For safety & avoid overhead
//		return MultiSearchAtBuffer( bfr, bfr_size, search, search_size, options, ret_ptr );
//	#pragma omp parallel sections // starts a new team
//		{
//			{ MultiSearchAtBuffer( bfr, bfr_size/2, search, search_size, options, &v1); }
//			#pragma omp section
//			{ MultiSearchAtBuffer( bfr+ztep-search_size+1, ztep+search_size-1, search, search_size, options, &v2); }
//			#pragma omp section
//			{ MultiSearchAtBuffer( bfr+(ztep*2)-search_size+1, ztep+search_size-1, search, search_size, options, &v3); }
//			#pragma omp section
//			{ MultiSearchAtBuffer( bfr+(ztep*3)-search_size+1, bfr_size-ztep*3+search_size-1, search, search_size, options, &v4); }
//		}
//
//	int cnt;
//	cnt=v1.size();
//	for( int i=0; i<cnt; i++)
//		ret_ptr->push_back(v1[i]);
//	cnt=v2.size();
//	for( int i=0; i<cnt; i++)
//		ret_ptr->push_back(v2[i]+ztep-search_size+1);
//	cnt=v3.size();
//	for( int i=0; i<cnt; i++)
//		ret_ptr->push_back(v3[i]+(ztep*2)-search_size+1);
//	cnt=v4.size();
//	for( int i=0; i<cnt; i++)
//		ret_ptr->push_back(v4[i]+(ztep*3)-search_size+1);
//
//	return ret_ptr->size();
//#endif
//	}

///This function will slow down searching process due overhead of OpenMP
//inline int FindDialog::SearchAtBufferMultiThread( char *bfr, int bfr_size, char* search, int search_size, unsigned options){
//	return SearchAtBuffer( bfr, bfr_size, search, search_size, options );
//
/////DualThread
//	int f1,f2;
//	#pragma omp parallel sections // starts a new team
//	{
//		#pragma omp section
//		{f1=SearchAtBuffer( bfr, bfr_size/2, search, search_size, options );}
//		#pragma omp section
//		{f2=SearchAtBuffer( bfr+ bfr_size-bfr_size/2-search_size+1, bfr_size/2+search_size-1, search, search_size, options );}
//	}
//	return f1 >= 0 ? f1 : bfr_size/2+f2;
//
/////QuadThread
//	int ztep=bfr_size/4;
//	int f1,f2,f3,f4;
//
//	#ifndef OMP_H
//	return SearchAtBuffer( bfr, bfr_size, search, search_size, options );
//	#else
//	if( ztep < search_size*128 )	//For safety & avoid overhead
//		return SearchAtBufferSingleThread( bfr, bfr_size, search, search_size, options );
//	#pragma omp parallel sections // starts a new team
//		{
//			{ f1=SearchAtBufferSingleThread( bfr, ztep, search, search_size, options ); }
//			#pragma omp section
//			{ f2=SearchAtBufferSingleThread( bfr+ztep-search_size+1, ztep+search_size-1, search, search_size, options ); }
//			#pragma omp section
//			{ f3=SearchAtBufferSingleThread( bfr+(ztep*2)-search_size+1, ztep+search_size-1, search, search_size, options ); }
//			#pragma omp section
//			{ f4=SearchAtBufferSingleThread( bfr+(ztep*3)-search_size+1, bfr_size-ztep*3+search_size-1, search, search_size, options ); }
//		}
//	//std::cout << "SearchAtBuffer f1:" << f1 << "\tf2:" <<  f2 << "\tf3:" <<  f3 << "\tf4:" << f4 << std::endl;
//	return f1>=0 ? f1 :
//			 f2>=0 ? f2 + ztep-search_size:
//			 f3>=0 ? f3 + 2*ztep:
//			 f4>=0 ? f4 + 3*ztep : -1;
//	#endif
//	}

// TODO (death#9#): Implement better search algorithm instead of 1:1 comparison + (using OpenCL, OpenMP, SIMD) :)
// DONE : SIMD SSE2 engine.



//Returns indice of byte if used with (! options & SEARCH_FINDALL)
//Returns indice of first found if used with (  options & SEARCH_FINDALL) ret_ptr return vector pointer filled with locations at buffer
//WARNING! THIS FUNCTION WILL CHANGE BFR and/or SEARCH strings if SEARCH_MATCHCASE not selected as an option!
inline int FindDialog::SearchAtBuffer( char *bfr, int bfr_size, char* search, int search_size, unsigned options, std::vector<int> *ret_ptr ) {
	static const int REG_SZ = sizeof(__m128i);

	char internal_array[REG_SZ];

	if( bfr_size < search_size )
		return -1;
	if(bfr_size < REG_SZ) {
		memset(&internal_array[0], 0, sizeof(internal_array));
		memcpy(&internal_array[0], bfr, bfr_size);
		bfr = &internal_array[0];
	}

	///SEARCH_FINDALL operation supersedes SEARCH_BACKWARDS and SEARCH_WRAPAROUND
	if(options & SEARCH_FINDALL)
		options &= ~SEARCH_BACKWARDS;

	//UTF with NO matched case handled here !!!SLOW!!! both for FORWARD and BACKWARD
	if(options & SEARCH_UTF8 && options & SEARCH_TEXT && !(options & SEARCH_MATCHCASE) ) {
		//!!!UTF8 Search Speed Hack handling here!!!

		// TODO (death#1#): Have doubts if thish lead Segmentation fault or works OK every time? Need unit test for this fx
		//For UTF8, search_size must be > 2. Than, we are in the safe! :)
		if(1) {
			wxString ucode;
			wxCharBuffer ubuf;

			if( (options & SEARCH_FINDALL) || !(options & SEARCH_BACKWARDS) )
				for(int i=0 ; i <= bfr_size - search_size ; i++ ) {
					//Compare buffer's first and second bytes with search(lower version) and UTF8SpeedHackChrs (upper version)
					if( ( bfr[i]==search[0] && bfr[i+1]==search[1] ) ||
					      ( bfr[i]==UTF8SpeedHackChrs[0] && bfr[i+1]==UTF8SpeedHackChrs[1] )  // Safe due UTF8??
					  ) {
						//Partial match found!
						//Now generate UTF8 Lower binary from buffer for full comparison.
						ucode = wxString::FromUTF8(bfr+i, search_size);
						ubuf = ucode.Lower().ToUTF8();
						if(! memcmp( ubuf, search, search_size)) {	//if match found
							ret_ptr->push_back(i);
							if(options & SEARCH_FINDALL)
								continue;
							else
								return i;
							}
						}
					}
			else //Backward Search!
				for(int i=bfr_size - search_size ; i >= 0 ; i-- ) {
					if( ( bfr[i]==search[0] && bfr[i+1]==search[1] ) ||
					      ( bfr[i]==UTF8SpeedHackChrs[0] && bfr[i+1]==UTF8SpeedHackChrs[1] )  // Safe due UTF8??
					  ) {
						//Partial match found!
						//Now generate UTF8 Lower binary from buffer for full comparison.
						ucode = wxString::FromUTF8(bfr+i, search_size);
						ubuf = ucode.Lower().ToUTF8();
						if(! memcmp( ubuf, search, search_size)) {	//if match found
							ret_ptr->push_back(i);
							if(options & SEARCH_FINDALL)
								continue;
							else
								return i;
							}
						}
					}
			}
		else { //Old and deadly slow code
			wxString ucode;
			wxCharBuffer ubuf;
			if((options & SEARCH_FINDALL) || !( options & SEARCH_BACKWARDS)) { //Backward Search!
				for(int i=0 ; i <= bfr_size - search_size ; i++ ) {
					ucode = wxString::FromUTF8(bfr+i, search_size);
					ubuf = ucode.Lower().ToUTF8();
					if(! memcmp( ubuf, search, search_size )) {	//if match found
						ret_ptr->push_back(i);
						if(options & SEARCH_FINDALL)
							continue;
						else
							return i;
						}
					}
				}
			else { //Backward Search!
				for(int i=bfr_size - search_size ; i >= 0 ; i-- ) {
					ucode = wxString::FromUTF8(bfr+i, search_size);
					ubuf = ucode.Lower().ToUTF8();
					if(! memcmp( ubuf, search, search_size )) {	//if match found
						ret_ptr->push_back(i);
						if(options & SEARCH_FINDALL)
							continue;
						else
							return i;
						}
					}
				}
			}
		}

	//TEXT search handled here with NO_MATCH CASE, both for FORWARD and BACKWARD
	else if(options & SEARCH_TEXT && !(options & SEARCH_MATCHCASE)) {
		//Search at no match case ASCII handled here
		///Search text already need to be lowered at FindText()
#ifdef __SSE2__
		const char FirstChL = search[0];
		const char FirstChU = toupper(FirstChL);

		const __m128i b = _mm_set1_epi8(FirstChL);
		const __m128i c = _mm_set1_epi8(FirstChU);
		if((options & SEARCH_FINDALL) || !( options & SEARCH_BACKWARDS)) { //Normal Operation
			for(int i = 0; i <= bfr_size - std::max(search_size, REG_SZ) ; i += REG_SZ) {
				const __m128i a = _mm_loadu_si128 ( reinterpret_cast<__m128i*>(bfr+i));
				const __m128i r1 = _mm_cmpeq_epi8 ( a, b );
				const __m128i r2 = _mm_cmpeq_epi8 ( a, c );
				const int reg1 = _mm_movemask_epi8( r1 );
				const int reg2 = _mm_movemask_epi8( r2 );
				if(reg1 || reg2) {	//If SIMD return a possible match in this block
#ifdef _DEBUG_FIND_
					char s[REG_SZ];
					__m128i* k = (__m128i*)(s);
					std::cout << "\nSSE2 Debug for i=" << i << std::hex << "\t Reg1 : 0x" << reg1 << "\t Reg2 : 0x" << reg2 << std::dec;
					_mm_storeu_si128(k, a);
					std::cout << "\nREG MMX A=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << (s[z]==0?' ':s[z]);
					_mm_storeu_si128(k, b);
					std::cout << "\nREG MMX B=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << s[z];
					_mm_storeu_si128(k, r1);
					std::cout << "\nREG MMX R=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << (s[z]==0?' ':'X');
					_mm_storeu_si128(k, c);
					std::cout << "\nREG MMX C=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << s[z];
					_mm_storeu_si128(k, r2);
					std::cout << "\nREG MMX R=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << (s[z]==0?' ':'X');
					std::cout << std::endl;
#endif // _DEBUG_FIND_
					for(int j = 0; j < REG_SZ; j++ ) {
						const int  ij   = i + j;
						if(ij > bfr_size - search_size) break;
						const char bfij = bfr[ij];
						//since search[0] allready lowered & UTF8SpeedHackChrs[0] is uppered
						if (bfij == FirstChL || bfij == FirstChU)	{
							//we got first byte match here, let lower bfr[i:search_size] to full match
							//partialy lowering buffer
							for (int k = ij + 1; (k < bfr_size) && ((k - ij) < search_size); k++)
								bfr[k] = tolower(bfr[k]);
							if (!memcmp(bfr + ij + 1, search + 1, search_size - 1)) {	//if match found
								ret_ptr->push_back(ij);
								if(options & SEARCH_FINDALL)
									continue;
								else
									return ij;
							}
						}
					}
				}
			}
		}
		else { //Backward Search!
			for (int i = bfr_size - std::max(search_size, REG_SZ); i > -REG_SZ; i -= REG_SZ) {
				if (i < 0) i = 0;
				__m128i a = _mm_loadu_si128 ( reinterpret_cast<__m128i*>(bfr+i));
				__m128i r1 = _mm_cmpeq_epi8 ( a, b );
				__m128i r2 = _mm_cmpeq_epi8 ( a, c );
				int reg1=_mm_movemask_epi8( r1 );
				int reg2=_mm_movemask_epi8( r2 );
				if(reg1 || reg2) {	//If SIMD return a possible match in this block
#ifdef _DEBUG_FIND_
					char s[REG_SZ];
					__m128i* k = (__m128i*)(s);
					std::cout << "\nSSE2 Debug for i=" << i << std::hex << "\t Reg1 : 0x" << reg1 << "\t Reg2 : 0x" << reg2 << std::dec;
					_mm_storeu_si128(k, a);
					std::cout << "\nREG MMX A=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << (s[z]==0?' ':s[z]);
					_mm_storeu_si128(k, b);
					std::cout << "\nREG MMX B=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << s[z];
					_mm_storeu_si128(k, r1);
					std::cout << "\nREG MMX R=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << (s[z]==0?' ':'X');
					_mm_storeu_si128(k, c);
					std::cout << "\nREG MMX C=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << s[z];
					_mm_storeu_si128(k, r2);
					std::cout << "\nREG MMX R=";
					for(int z=0; z< REG_SZ ; z++ ) std::cout << (s[z]==0?' ':'X');
					std::cout << std::endl;
#endif // _DEBUG_FIND_
					for (int j = std::min(REG_SZ - 1, bfr_size - search_size - i); j >= 0; j--) {
						const int  ij   = i + j;
						const char bfij = bfr[ij];

						if (bfij == FirstChL || bfij == FirstChU) {
							//we got first byte match here, let lower bfr[i:search_size] to full match
							//partialy lowering buffer
							for (int k = ij + 1; (k < bfr_size) && ((k - ij) < search_size); k++)
								bfr[k] = tolower(bfr[k]);
							if (!memcmp(bfr + ij + 1, search + 1, search_size - 1)) {	//if match found
								ret_ptr->push_back(ij);
								if(options & SEARCH_FINDALL)
									continue;
								else
									return ij;
							}
						}
					}
				}
				if (!i) break;
			}
		}
#else
		//Forward Operation
		if((options & SEARCH_FINDALL) || !( options & SEARCH_BACKWARDS)) {
			for(int i=0 ; i <= bfr_size - search_size ; i++ )
				//since search[0] allready lowered, UTF8SpeedHackChrs[0] is uppered
				if( bfr[i] == search[0] || bfr[i] == UTF8SpeedHackChrs[0] ) {
					//we got first byte match here, let lower bfr[i:search_size] to full match
					//partialy lowering
					for( int j = i ; (j < bfr_size) && (j-i<search_size); j++)
						bfr[j]=tolower(bfr[j]);

					if(! memcmp( bfr+i, search, search_size )) {	//if match found
						ret_ptr->push_back(i);
						if(options & SEARCH_FINDALL)
							continue;
						else
							return i;
						}
					}
			}
		//Backward Operation
		else {
			for( int i=bfr_size - search_size ; i >= 0 ; i-- )
				if( bfr[i] == search[0] || bfr[i] == UTF8SpeedHackChrs[0] ) {
					//partial lowering code
					for( int j = i ; (j < bfr_size) && (j-i<search_size); j++)
						bfr[j]=tolower(bfr[j]);

					if(! memcmp( bfr+i, search, search_size )) {	//if match found
						ret_ptr->push_back(i);
						if(options & SEARCH_FINDALL)
							continue;
						else
							return i;
						}
					}
			}
#endif
		}

	//Search as HEX, TEXT with MATCHCASE FORWARD Search.
	else if((options & SEARCH_FINDALL) || !(options & SEARCH_BACKWARDS)) {
		//#pragma omp parallel for schedule(static)
#ifdef __SSE2__
		//681 clk with SSE2 vs 1200clk with C++
		__m128i b = _mm_set1_epi8 ( search[0] );

		for(int i = 0; i <= bfr_size - std::max(search_size, REG_SZ) ; i += REG_SZ ) {
			__m128i a = _mm_loadu_si128 ( reinterpret_cast<__m128i*>(bfr+i));
			__m128i r = _mm_cmpeq_epi8 ( a, b );
			short reg=_mm_movemask_epi8( r );
		#ifdef _DEBUG_FIND_
			char s[16];
			__m128i* k = (__m128i*)(s);
			std::cout << " SSE2 Debug for i=" << i << std::hex << "\t Reg : 0x" << reg << std::dec;
			_mm_storeu_si128(k, a);
			std::cout << "\nREG MMX A=";
			for(int z=0; z< 16 ; z++ ) std::cout << (s[z]==0?' ':s[z]);
			_mm_storeu_si128(k, b);
			std::cout << "\nREG MMX B=";
			for(int z=0; z< 16 ; z++ ) std::cout << s[z];
			_mm_storeu_si128(k, r);
			std::cout << "\nREG MMX R=";
			for(int z=0; z< 16 ; z++ ) std::cout << (s[z]==0?' ':'X');
			std::cout << std::endl;
		#endif // _DEBUG_FIND_
			if( reg )
				for(int j = 0; j < REG_SZ; j++ ) {
					const int  ij   = i + j;
					if(ij > bfr_size - search_size) break;
					const char bfij = bfr[ij];
					//if( (1<<j) & reg)
					if( bfij == search[0] ) //xxxclk
						if(! memcmp( bfr + ij + 1, search + 1, search_size - 1 )) {	//if match found
							ret_ptr->push_back(ij);
							if(options & SEARCH_FINDALL)
								continue;
							else
								return i+j;
						}
				}
			}
		//Process last chunk that smaller than 16 byte (SSE2 register's max load is 16 bytes)
//		for(int i=0 ; i < 16 ; i++ )
//			if( bfr[i] == search[0] )
//				if(! memcmp( bfr+i, search, search_size ))	//if match found
//					ret_ptr->push_back(i);

#else
		for(int i=0 ; i <= bfr_size - search_size ; i++ )
			if( bfr[i] == search[0] )
				if(! memcmp( bfr+i, search, search_size )) {	//if match found
					ret_ptr->push_back(i);
					if(options & SEARCH_FINDALL)
						continue;
					else
						return i;
					}
#endif //__SSE2__
		}

	//Search as HEX, TEXT with MATCHCASE BACKWARD Search.
	else {
#ifdef __SSE2__
		//676clk with SSE2, 1251 clk with C++
		__m128i b = _mm_set1_epi8 ( search[0] );
		for (int i = bfr_size - std::max(search_size, REG_SZ); i > -REG_SZ; i -= REG_SZ) {
			if (i < 0) i = 0;
			__m128i a = _mm_loadu_si128 ( reinterpret_cast<__m128i*>(bfr+i));
			__m128i r = _mm_cmpeq_epi8 ( a, b );
			short reg=_mm_movemask_epi8( r );
#ifdef _DEBUG_FIND_

			char s[REG_SZ];
			__m128i* k = (__m128i*)(s);
			std::cout << " SSE2 Debug for i=" << i << std::hex << "\t Reg : 0x" << reg << std::dec;
			_mm_storeu_si128(k, a);
			std::cout << "\nREG MMX A=";
			for(int z=0; z< REG_SZ ; z++ ) std::cout << (s[z]==0?' ':s[z]);
			_mm_storeu_si128(k, b);
			std::cout << "\nREG MMX B=";
			for(int z=0; z< REG_SZ ; z++ ) std::cout << s[z];
			_mm_storeu_si128(k, r);
			std::cout << "\nREG MMX R=";
			for(int z=0; z< REG_SZ ; z++ ) std::cout << (s[z]==0?' ':'X');
			std::cout << std::endl;
#endif // _DEBUG_FIND_
			if( reg ) {
				for( int j = std::min(REG_SZ - 1, bfr_size - search_size - i); j >= 0; j--) {
					const int  ij   = i + j;
					const char bfij = bfr[ij];
					//if( (1<<j) & reg)
					if( bfij == search[0] )
						if (!memcmp(bfr + ij + 1, search + 1, search_size - 1)) {	//if match found
							ret_ptr->push_back(ij);
							if(options & SEARCH_FINDALL ) {
								continue;
								}
							else
								return ij;
						}
				}
			}
			if (!i) break;
		}
#else
		for(int i=bfr_size - search_size ; i >= 0 ; i-- )
			if( bfr[i] == search[0] )
				if(! memcmp( bfr+i, search, search_size )) {	//if match found
					ret_ptr->push_back(i);
					if(options & SEARCH_FINDALL)
						continue;
					else
						return i;
					}
#endif // __SSE2__
		}


	if((options & SEARCH_FINDALL) && (ret_ptr->size() > 0) )
		return ret_ptr->at(0);
	return -1;
	}

ReplaceDialog::ReplaceDialog( wxWindow* parent, FAL *find_file, wxString title ):FindDialog( parent, find_file, title ) {
	//Enabling hidden replace dialog elements.
	m_comboBoxReplace->Show();
	m_static_replace->Show();
	m_staticline->Show();
	btnReplace->Show();
	btnReplaceAll->Show();
	Fit();
	//Load previous search results to replace box.
	PrepareComboBox( false );
	m_comboBoxReplace->Select(0);
	}

int ReplaceDialog::OnReplace( bool internal ) {
	if( !parent->select->GetState() ) {
		if( OnFind( internal ) == false )
			return 0;
		else
			return -1;
		}

	else {
		if( m_searchtype->GetSelection() == 0 ) { //text search
			PrepareComboBox( true );
			if( parent->select->GetSize() == m_comboBoxReplace->GetValue().Len() ) {
				parent->FileAddDiff( parent->CursorOffset(), m_comboBoxReplace->GetValue().ToAscii(), m_comboBoxReplace->GetValue().Len());
				parent->select->SetState( false );
				parent->Reload();
				wxUpdateUIEvent eventx( UNREDO_EVENT );
				wxPostEvent(parent, eventx );
				return 1;
				}
			else {
				OSXwxMessageBox(_("Search and Replace sizes are not equal!\nReplacing with different size are not supported yet."), _("Error!"), wxOK, this);
				return 0;
				}
			}
		else { //hex search
			//Hex Validation and Format
			wxString hexval = m_comboBoxReplace->GetValue();

			if( !HexVerifyAndPrepare( hexval, _("Replace"), this ) )
				return 0;

			m_comboBoxReplace->SetValue(hexval.Upper());

			PrepareComboBox( true );

			wxMemoryBuffer search_binary = wxHexCtrl::HexToBin( m_comboBoxReplace->GetValue());
			if( parent->select->GetSize() == search_binary.GetDataLen() ) {
				parent->FileAddDiff( parent->CursorOffset(), static_cast<char*>(search_binary.GetData()),search_binary.GetDataLen() );
				parent->select->SetState( false );
				parent->Reload();
				wxUpdateUIEvent eventx( UNREDO_EVENT );
				wxPostEvent( parent, eventx);
				return 1;
				}
			else {
				OSXwxMessageBox(_("Search and Replace sizes are not equal!\nReplacing with different size are not supported yet."), _("Error!"), wxOK, this);
				return 0;
				}
			}
		}
	}

void ReplaceDialog::OnReplaceAll( void ) {
	//First search all file with find all to detect locations.
	OnFindAll( true );
	//Now Highlight array has matches. We could replace them with replace string.
	for( uint32_t i=0 ; i < parent->HighlightArray.Count() ; i++ ) {
		if( m_searchtype->GetSelection() == 0 ) { //text search
			PrepareComboBox( true );
			parent->FileAddDiff( parent->HighlightArray.Item(i)->start,
			                     m_comboBoxReplace->GetValue().ToAscii(),
			                     m_comboBoxReplace->GetValue().Len());
			}
		else { //hex search
			//Hex Validation and Format
			wxString hexval = m_comboBoxReplace->GetValue();
			if(!HexVerifyAndPrepare( hexval, _("Replace"), this ) )
				return;
			m_comboBoxReplace->SetValue(hexval.Upper());
			PrepareComboBox( true );

			wxMemoryBuffer search_binary = wxHexCtrl::HexToBin( m_comboBoxReplace->GetValue());
			parent->FileAddDiff( parent->HighlightArray.Item(i)->start,
			                     static_cast<char*>(search_binary.GetData()),
			                     search_binary.GetDataLen() );
			}

		if( parent->HighlightArray.Count() < 20 )						 // if there is too much matches,
			parent->Goto( parent->HighlightArray.Item(i)->start ); // this make program unresponsive and slow.
		}

	if( parent->HighlightArray.Count() > 0) {
		parent->Goto( parent->HighlightArray.Item(0)->start );
		parent->Refresh();
		this->Hide();
		wxUpdateUIEvent eventx( UNREDO_EVENT );
		wxPostEvent( parent, eventx);
		OSXwxMessageBox(wxString::Format(_("%d records changed."), parent->HighlightArray.Count() ), _("Info"), wxOK, parent);
		#ifdef __WXMAC__
			EndModal(0);
		#endif
		}
	}

void ReplaceDialog::EventHandler( wxCommandEvent& event ) {
	int id = event.GetId();
	if( id == btnFind->GetId() )
		OnFind();
	else if( id == btnFindAll->GetId() )
		OnFindAll();
	else if( id == btnReplace->GetId() )
		OnReplace();
	else if( id == btnReplaceAll->GetId() )
		OnReplaceAll();
	}

void ReplaceDialog::PrepareComboBox( bool AddString ) {
	int searchType = m_searchtype->GetSelection() == 0 ? SEARCH_TEXT : SEARCH_HEX;
	if( searchType == SEARCH_TEXT )
		ComboBoxFill( _T("ReplaceTextString"), m_comboBoxReplace, AddString);
	else
		ComboBoxFill( _T("ReplaceHexString"), m_comboBoxReplace, AddString);
	}

CopyAsDialog::CopyAsDialog( wxWindow* _parent, FAL *file, Select *select_, ArrayOfTAG* MainTagArray_):CopyAsDialogGui( _parent, wxID_ANY) {
	parent = static_cast< HexEditor* >( _parent );
	spnBytePerLine->SetValue( parent->BytePerLine() );
	select = select_;
	copy_file = file;
	MainTagArray=MainTagArray_;
	bool IsBigEndian;
	//parent->interpreter->chkBigEndian->GetValue(); Does this approach better than remembering old value?
	myConfigBase::Get()->Read( _T("CopyAsBigEndian"), &IsBigEndian );
	chkBigEndian->SetValue( IsBigEndian );

	int CopyAsFunction;
	if (myConfigBase::Get()->Read(_T("CopyAsSelectedFunction"), &CopyAsFunction))
		chcCopyAs->SetSelection( CopyAsFunction );
	PrepareOptions( CopyAsFunction );
	}

void CopyAsDialog::PrepareOptions( int SelectedFunction ) {
//wxT("Full Text"), wxT("Special Hex"), wxT("HTML"), wxT("C/C++"), wxT("Assembler")
	chcOption->Enable(SelectedFunction > 0);
	chcOption->Clear();

	if( SelectedFunction == 0) {
		chcOption->Insert(_("Not Available"),0);
		chkOffset->Enable(true);
		chkHex->Enable(true);
		chkText->Enable(true);
		}
	else if( SelectedFunction == 1) { // Hex
		chcOption->Insert(_("Raw Hex"),0);
		chcOption->Insert(_("With Space"),1);
		chcOption->Insert(_("Quad Hex"),2);
		chcOption->Insert(_("with 0x"),3);
		chcOption->Insert(_("with 0x and Comma"),4);
		chcOption->Insert(_("with \\x"),5);
		chkOffset->Enable(false);
		chkHex->Enable(false);
		chkText->Enable(false);

		}
	else if( SelectedFunction == 2) { // HTML
		chcOption->Insert(_("HTML format"),0);
		chcOption->Insert(_("HTML with TAGs"),1);
		chcOption->Insert(_("phpBB forum style"),2);
		chcOption->Insert(_("WiKi format"),3);
		chcOption->Insert(_("WiKi with TAGs"),4);
		chkOffset->Enable(true);
		chkHex->Enable(true);
		chkText->Enable(true);
		}
	else if( SelectedFunction >= 3) { // C/C++/ASM Sources
		chcOption->Insert(_("8bit - Byte "),0);
		chcOption->Insert(_("16bit - Words"),1);
		chcOption->Insert(_("32bit - Dwords"),2);
		chcOption->Insert(_("64bit - Qwords"),3);
		chkOffset->Enable(false);
		chkHex->Enable(false);
		chkText->Enable(false);
		}
	int option=0;
	myConfigBase::Get()->Read(_T("CopyAsSelectedOptionOfFunction") + wxString::Format(wxT("%d"),SelectedFunction), &option);
	chcOption->SetSelection( option );

	//Enable big endian checkbox if multi byte representation selected for C/C++/ASM sources.
	chkBigEndian->Enable( chcCopyAs->GetSelection() >=3 && chcOption->GetSelection() > 0 );

	wxYield();
	this->GetSizer()->Fit(this);
	this->GetSizer()->Layout();
	}

void CopyAsDialog::EventHandler( wxCommandEvent& event ) {
	int id = event.GetId();
	if( id == wxID_CANCEL )
		EndModal(id);
	else if( id == wxID_OK ) {
		Copy();
		EndModal(id);
		}
	else if( id == chcCopyAs->GetId() ) {
		int SelectedFunction =  chcCopyAs->GetSelection();
		PrepareOptions(SelectedFunction);

		int option;
		//Adjustinf selection part
		if (myConfigBase::Get()->Read(_T("CopyAsSelectedOptionOfFunction") + wxString::Format(wxT("%d"),SelectedFunction), &option))
			chcOption->SetSelection( option );
		else
			chcOption->SetSelection(0);

		myConfigBase::Get()->Write( _T("CopyAsSelectedFunction"), SelectedFunction );
		}
	else if( id == chcOption->GetId() ) {
		myConfigBase::Get()->Write( _T("CopyAsSelectedOptionOfFunction") + wxString::Format(wxT("%d"), chcCopyAs->GetSelection()), chcOption->GetSelection() );

		//Enable big endian checkbox if multi byte representation selected for C/C++/ASM sources.
		chkBigEndian->Enable( chcCopyAs->GetSelection() >=3 && chcOption->GetSelection() > 0 );
		}
	else if( id == chkBigEndian->GetId() ) {
		myConfigBase::Get()->Write( _T("CopyAsBigEndian"), chkBigEndian->GetValue() );
		}

	}

void CopyAsDialog::PrepareFullText( wxString& cb, wxMemoryBuffer& buff ) {
	unsigned BytePerLine = spnBytePerLine->GetValue();
	for(unsigned current_offset = 0; current_offset < select->GetSize() ; current_offset += BytePerLine) {
		if(chkOffset->GetValue()) {
			cb += parent->GetFormatedOffsetString( select->GetStart() + current_offset );
			cb += wxT("   ");
			}

		//Add 16 hex val
		if(chkHex->GetValue()) {
			for(unsigned i = 0 ; i < BytePerLine ; i++) {
				if( i + current_offset < select->GetSize())
					cb+= wxString::Format( wxT("%02X "), (unsigned char)buff[ current_offset + i] );
				else
					cb+= wxT("   "); //fill with zero to make text area at proper location
				}
			cb += wxT("  "); //Why only 2 ? Because we got extra 1 space from Hex
			}

		if(chkText->GetValue()) {
			//Add 16 Ascii rep
//			unsigned char chr;
//			for(unsigned i = 0 ; i < BytePerLine ; i++){
//				if( i + current_offset < select->GetSize()){
//					chr = buff[ current_offset + i];
//					//Char filter for ascii
//					if( (chr !=173) && ( (chr>31 && chr<127) || chr>159) )
//						cb+= wxString::FromAscii( buff[ current_offset + i]%0x7F);
//					else
//					cb+= '.';
//					}
//				}
			cb += wxString::From8BitData( &buff[current_offset],
			                              (current_offset+BytePerLine > select->GetSize()) ?
			                              select->GetSize()-current_offset : BytePerLine);
			}
		cb += wxNewline;
		}
	}

void CopyAsDialog::PrepareFullTextWithTAGs( wxString& cb, wxMemoryBuffer& buff, wxString startup, wxString ending, wxString blank ) {
	bool TAGenable=true;

	wxString x3Blank=blank+blank+blank;

	unsigned BytePerLine = spnBytePerLine->GetValue();
	wxString last_color_hex,last_color_text;
	if(TAGenable) {
		cb += startup+wxT("TAG List:")+ending;
		for( unsigned i =0 ; i < MainTagArray->Count() ; i++ ) {
			TagElement *tag = MainTagArray->Item(i);
			if(( tag->start <  select->GetStart() && tag->end   >= select->GetStart() ) ||
			      ( tag->start >= select->GetStart() && tag->start <= select->GetEnd() ) ||
			      ( tag->end   >= select->GetStart() && tag->end   <= select->GetEnd() ) ) {

				cb += startup+wxT("<span style=\"background-color:") + tag->SoftColour( tag->NoteClrData.GetColour() ).GetAsString(wxC2S_HTML_SYNTAX) +
				      wxT(";color:") + tag->FontClrData.GetColour().GetAsString(wxC2S_HTML_SYNTAX) +  wxT(";\">") + tag->tag +wxT("</span>")+ending;
				}
			}
		cb += startup+ending;
		}

	for(unsigned current_offset = 0; current_offset < select->GetSize() ; current_offset += BytePerLine) {
		if(chkOffset->GetValue()) {
			cb += startup + parent->GetFormatedOffsetString( select->GetStart() + current_offset );
			cb += x3Blank;
			}

		if(chkHex->GetValue()) {
			//Add 16 hex val
			//Check for middle TAG selection starts
			if( current_offset == 0 )
				for( unsigned j = 0 ; j< MainTagArray->Count() ; j++ ) {
					TagElement *tg = MainTagArray->Item(j);
					if( tg->isCover( select->GetStart() ) )
						last_color_hex = last_color_hex = tg->SoftColour(tg->NoteClrData.GetColour()).GetAsString(wxC2S_HTML_SYNTAX);
					}

			if( last_color_hex.Len() )
				cb += wxT("<span style=\"background-color:") + last_color_hex + wxT(";\">");

			for(unsigned i = 0 ; i < BytePerLine ; i++) {

				//TAG Paint Loop
				if(TAGenable)
					for( unsigned j = 0 ; j< MainTagArray->Count() ; j++ ) {
						TagElement *tg = MainTagArray->Item(j);
						if( MainTagArray->Item(j)->start == i + current_offset + select->GetStart()) {
							last_color_hex = tg->SoftColour(tg->NoteClrData.GetColour()).GetAsString(wxC2S_HTML_SYNTAX);
							cb += wxT("<span style=\"background-color:") + last_color_hex + wxT(";\">");
							}
						if( MainTagArray->Item(j)->end +1== i + current_offset + select->GetStart() ) {
							cb += wxT("</span>");
							last_color_hex = wxEmptyString;
							}
						}

				if( i + current_offset < select->GetSize())
					cb+= wxString::Format( wxT("%02X "), (unsigned char)buff[ current_offset + i] );
				else {
					if(last_color_hex.Len() )
						cb += wxT("</span>");
					last_color_hex = wxEmptyString;
					cb+= x3Blank; //fill with zero to make text area at proper location
					}
				//This avoid to paint text section.
				if(last_color_hex.Len() && i==BytePerLine-1)
					cb += wxT("</span>");
				}
			//cb += wxT("  "); //Why only 2 ? Because we got extra 1 space from Hex
			cb += blank+blank; //Why only 2 ? Because we got extra 1 space from Hex
			}

		if(chkText->GetValue()) {
			//Add 16 Ascii rep

			if( current_offset == 0 )
				for( unsigned j = 0 ; j< MainTagArray->Count() ; j++ ) {
					TagElement *tg = MainTagArray->Item(j);
					if( tg->isCover( select->GetStart() ) )
						last_color_text = tg->SoftColour(tg->NoteClrData.GetColour()).GetAsString(wxC2S_HTML_SYNTAX);
					}

			if( last_color_text.Len() )
				cb += wxT("<span style=\"background-color:") + last_color_text + wxT(";\">");
			for(unsigned i = 0 ; i < BytePerLine ; i++) {
				if( i + current_offset < select->GetSize()) {
					//TAG Paint Loop
					if(TAGenable)
						for( unsigned j = 0 ; j< MainTagArray->Count() ; j++ ) {
							TagElement *tg = MainTagArray->Item(j);
							if( MainTagArray->Item(j)->start == i + current_offset + select->GetStart()) {
								last_color_text = tg->SoftColour( tg->NoteClrData.GetColour()).GetAsString(wxC2S_HTML_SYNTAX);
								cb += wxT("<span style=\"background-color:") + last_color_text + wxT(";\">");
								}
							if( MainTagArray->Item(j)->end +1== i + current_offset + select->GetStart()) {
								cb += wxT("</span>");
								last_color_text = wxEmptyString;
								}
							}

					//Char filter for ascii
					cb += wxString::From8BitData( &buff[ current_offset + i], 1 );
					}
				if(last_color_text.Len() && i==BytePerLine-1)
					cb += wxT("</span>");
				}
			}
		cb += ending;
		}
	cb += startup + ending;
	}

void CopyAsDialog::Copy( void ) {
	if( select->GetState() ) {
		int BytePerLine = spnBytePerLine->GetValue();
		wxString cb;
		uint64_t RAM_limit = 10*MB;
		wxMemoryBuffer buff;
		if(select->GetSize() > RAM_limit) {
			OSXwxMessageBox( _("Selected block bigger than limit." ),_("Error!"), wxOK|wxICON_ERROR, this);
			return;
			}

		copy_file->Seek( select->GetStart(), wxFromStart );
		buff.UngetWriteBuf( copy_file->Read( static_cast<unsigned char*>(buff.GetWriteBuf(select->GetSize())), select->GetSize() ) );

		if( chcCopyAs->GetSelection() == 0 ) { //Full text copy
			PrepareFullText( cb, buff );
			}
		else if( chcCopyAs->GetSelection() == 1) {
			wxString HexFormat;
			bool quad=false;
			switch( chcOption->GetSelection()) {
				case 0:
					HexFormat=wxT("%02X");
					break; //Raw Hex
				case 1:
					HexFormat=wxT("%02X ");
					break; //Standard
				case 2:
					HexFormat=wxT("%02X");
					quad=true;
					break; //Quad
				case 3:
					HexFormat=wxT("0x%02X ");
					break; //Ox
				case 4:
					HexFormat=wxT("0x%02X, ");
					break; //Ox with period
				case 5:
					HexFormat=wxT("\\x%02X");
					break; //\x
				}

			for(unsigned current_offset = 0; current_offset < select->GetSize() ; current_offset ++) {
				cb+= wxString::Format( HexFormat, (unsigned char)buff[ current_offset ] );
				if(current_offset+1==select->GetSize())
					if (HexFormat.EndsWith(", ")) //Ox with period
						cb=cb.RemoveLast(2);
				if( quad && ((current_offset+1)%2)==0)
					cb += wxT(" ");
				if(( (current_offset+1) % BytePerLine)==0 )
					cb += wxNewline;
				}

			}

		else if( chcCopyAs->GetSelection() == 2) { //Internet
			if( chcOption->GetSelection() == 0 ) { //html
//				<html><head><title></title></head><body>
//				<pre>
//				<code style="color:#000000;background-color:#FFFFFF">0000E973   1375 6E5A 8696 553A 01C9 51A2 F244 90BD   .unZ..U:.ÉQ¢òD.½</code>
//				<code style="color:#000000;background-color:zebra?;">0000E983   1375 6E5A 8696 553A 01C9 51A2 F244 90BD   .unZ..U:.ÉQ¢òD.½</code>
//				</pre>
//				<font size="-3">Generated by <a href="http://wxhexeditor.sourceforge.net/">wxHexEditor</a></font>
//				</body></html>
				cb += wxT("<pre><code style=\"color:#000000;\">");
				PrepareFullText( cb, buff );
				cb += wxT("</code></pre><font size=\"-3\">Generated by <a href=\"http://www.wxHexEditor.org/\">wxHexEditor</a></font>");
				}

			else if( chcOption->GetSelection() == 1 ) { //HTML with Tags
				cb += wxT("<pre><code style=\"color:#000000;\">");
				PrepareFullTextWithTAGs( cb, buff, wxT(""), wxT("<br/>"), wxT("&nbsp;") );
				cb += wxT("</code></pre><font size=\"-3\">Generated by <a href=\"http://www.wxHexEditor.org\">wxHexEditor</a></font>");
				}

			else if( chcOption->GetSelection() == 2 ) { //phpBB Forum
				cb+= wxT("[code]");
				PrepareFullText( cb, buff );
				cb += wxT("[/code]Generated by [url=http://www.wxHexEditor.org]wxHexEditor[/url]" wxNewline);
				}

			else if( chcOption->GetSelection() == 3 ) { //WiKi format
				cb+= wxT("<pre>");
				PrepareFullText( cb, buff );
				cb += wxT("</pre> Generated by [http://www.wxHexEditor.org/ wxHexEditor]" wxNewline);
				}
			else if( chcOption->GetSelection() == 4 ) { //WiKi with TAGs
				PrepareFullTextWithTAGs( cb, buff, wxT(" "),wxT("\n"),wxT(" ") );
				cb += wxT(" Generated by [http://www.wxHexEditor.org/ wxHexEditor]" wxNewline);
				}
			}
		else if( chcCopyAs->GetSelection() == 3) { //C/C++
			unsigned HexSize = pow( 2, chcOption->GetSelection());
			unsigned count = select->GetSize()/HexSize;
			cb+=wxT("// Generated by wxHexEditor //" wxNewline);
			switch( HexSize ) {
				case 1:
					cb += wxT("int8_t");
					break;
				case 2:
					cb += wxT("int16_t");
					break;
				case 4:
					cb += wxT("int32_t");
					break;
				case 8:
					cb += wxT("int64_t");
					break;
				}

			cb+=wxString::Format( wxT(" hexData[0x%x] = {" wxNewline "  "), count );
			bool bigEndianSwapReq = chkBigEndian->GetValue() && chcOption->GetSelection();//No big endian for 8 bit
			int b;
			int limit=(bigEndianSwapReq ? -1 : HexSize);
			int incr = (bigEndianSwapReq ? -1 : +1);
			for(unsigned current_offset = 0; current_offset < count ; current_offset ++) {
				b = (bigEndianSwapReq ? HexSize-1 : 0);
				cb+= wxT("0x");
				for(; b != limit ; b+=incr)
					cb+= wxString::Format( wxT("%02X"), reinterpret_cast<unsigned char*>( buff.GetData() )[current_offset*HexSize+b]);
				cb+= wxT(", ");
//				switch( HexSize ){
//					case 1: cb+= wxString::Format( wxT("0x%02X, "), *reinterpret_cast<unsigned char*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 2: cb+= wxString::Format( wxT("0x%04X, "), *reinterpret_cast<unsigned short*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 4: cb+= wxString::Format( wxT("0x%08X, "), *reinterpret_cast<unsigned int*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 8: cb+= wxString::Format( wxT("0x%016" wxLongLongFmtSpec "X, "), *reinterpret_cast<uint64_t*>( buff.GetData()+current_offset*HexSize )); break;
//					}
				if(( (current_offset+1) % (BytePerLine/HexSize)==0 ) && current_offset != count)
					cb += wxT("" wxNewline "  ");
				}
			cb=cb.BeforeLast(',')+wxT(" }" wxNewline);
			}
		else if( chcCopyAs->GetSelection() == 4) { //ASM
			unsigned HexSize = pow( 2, chcOption->GetSelection());
			unsigned count = select->GetSize()/HexSize;
			cb+=wxT(";Generated by wxHexEditor");

			wxString HexFormat;
			switch( HexSize ) {
				case 1:
					HexFormat += wxT("" wxNewline "db ");
					break;
				case 2:
					HexFormat += wxT("" wxNewline "dw ");
					break;
				case 4:
					HexFormat += wxT("" wxNewline "dd ");
					break;
				case 8:
					HexFormat += wxT("" wxNewline "dq ");
					break;
				}

			bool bigEndianSwapReq = chkBigEndian->GetValue() && chcOption->GetSelection();//No big endian for 8 bit
			int b;
			int limit=(bigEndianSwapReq ? -1 : HexSize);
			int incr = (bigEndianSwapReq ? -1 : +1);
			for(unsigned current_offset = 0; current_offset < count ; current_offset ++) {
				if( current_offset % (BytePerLine/HexSize)==0 )
					cb += HexFormat;
				b = (bigEndianSwapReq ? HexSize-1 : 0);
				cb+= wxT("0");
				for(; b != limit ; b+=incr)
					cb+= wxString::Format( wxT("%02X"), reinterpret_cast<unsigned char*>( buff.GetData() )[current_offset*HexSize+b]);
				cb+= wxT("h ");
//				switch( HexSize ){
//					case 1: cb+= wxString::Format( wxT("0%02Xh "), *reinterpret_cast<unsigned char*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 2: cb+= wxString::Format( wxT("0%04Xh "), *reinterpret_cast<unsigned short*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 4: cb+= wxString::Format( wxT("0%08Xh "), *reinterpret_cast<unsigned int*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 8: cb+= wxString::Format( wxT("0%016" wxLongLongFmtSpec "Xh "), *reinterpret_cast<uint64_t*>( buff.GetData()+current_offset*HexSize )); break;
//					}
				}
			cb+=wxT('\n');
			}

		if(wxTheClipboard->Open()) {
//					if (wxTheClipboard->IsSupported( wxDF_TEXT )){
			wxTheClipboard->Clear();
			wxTheClipboard->SetData( new wxTextDataObject( cb ));
			wxTheClipboard->Flush();
			wxTheClipboard->Close();
			}
		}
	}

CompareDialog::CompareDialog( wxWindow* parent_, wxString File1, wxString File2 ):CompareDialogGui(parent_, wxID_ANY) {
	parent = static_cast< HexEditorFrame* >(parent_);

	filePick1->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(CompareDialog::OnFileDrop),NULL, this);
	filePick2->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(CompareDialog::OnFileDrop),NULL, this);

	if( File1 != wxEmptyString ) {
		filePick1->SetPath( File1 );
		filePick2->SetPath( File2 );
		filePick1->Hide();
		filePick2->Hide();
		m_TextFile1->Hide();
		m_TextFile2->Hide();
		btnCompare->Enable(true);
		Fit();
		}

	//Not working on linux somehow???
//	else{
//		wxString tmp;
//		myConfigBase::Get()->Read( _T("CompareFile1"), &tmp );
//		filePick1->SetLabel( tmp );
//		filePick1->SetPath( tmp );
//
//		myConfigBase::Get()->Read( _T("CompareFile2"), &tmp );
//		filePick2->SetLabel( tmp );
//		filePick2->SetPath( tmp );
//		filePick2->UpdateTextCtrlFromPicker();
//		}

	int options=0;
	myConfigBase::Get()->Read( _T("CompareOptions"), &options );
	checkConnectFiles->SetValue( options & OPT_CMP_CONNECT );
	checkMergeSection->SetValue( options & OPT_CMP_MERGE_SECTION );
	spinMergeSection->Enable( options & OPT_CMP_MERGE_SECTION );
	checkStopCompare->SetValue( options & OPT_CMP_STOP_AFTER );
	spinStopCompare->Enable( options & OPT_CMP_STOP_AFTER );
	checkSaveResults->SetValue( options & OPT_CMP_SAVE );
	filePickSave->Enable( options & OPT_CMP_SAVE );
	m_radioDifferent->SetValue( options & OPT_CMP_SEARCH_DIFF );
	m_radioSame->SetValue( !(options & OPT_CMP_SEARCH_DIFF) );

	int tmp;
	myConfigBase::Get()->Read( _T("CompareOptionStopAfter"), &tmp );
	spinStopCompare->SetValue(tmp);

	myConfigBase::Get()->Read( _T("CompareOptionMergeSection"), &tmp );
	spinMergeSection->SetValue(tmp);
	}

CompareDialog::~CompareDialog(void) {
	filePick1->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler(CompareDialog::OnFileDrop),NULL, this);
	filePick2->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler(CompareDialog::OnFileDrop),NULL, this);
	}

bool CompareDialog::Compare( wxFileName fl1, wxFileName fl2, bool SearchForDiff, int StopAfterNMatch, wxFileName flsave ) {
	if(!fl1.IsFileReadable()) {
		wxMessageBox( _("File #1 is not readable."), _("Error") );
		return false;
		}
	if(!fl2.IsFileReadable() ) {
		wxMessageBox( _("File #2 is not readable."), _("Error") );
		return false;
		}

	myConfigBase::Get()->Write( _T("CompareFile1"), fl1.GetFullPath() );
	myConfigBase::Get()->Write( _T("CompareFile2"), fl2.GetFullPath() );
	myConfigBase::Get()->Flush();
//	if( flsave != wxEmptyString ){
//		if(!flsave.IsFileWritable() )
//			wxMessageBox( _("Error, Save File is not writeable.") );
//			return false;
//		}

	wxFFile f1,f2,fs;
// TODO (death#1#): Why we dont use FAL at ALL?
	if( !f1.Open( fl1.GetFullPath(), wxT("rb") ) ) {
		wxMessageBox( _("Error, File #1 cannot open." ) );
		return false;
		}
	if( !f2.Open( fl2.GetFullPath(), wxT("rb") ) ) {
		wxMessageBox( _("Error, File #2 cannot open." ) );
		return false;
		}

	if( flsave != wxEmptyString )
		if( !fs.Open( flsave.GetFullPath(), wxT("wb") ) ) {
			wxMessageBox( _("Error, Save File cannot open." ) );
			return false;
			}
	wxString msg= _("Comparing files...");
	wxString emsg=wxT("\n");
	wxProgressDialog pdlg( _("wxHexEditor Comparing"), msg+emsg, 1000, this, wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_APP_MODAL|wxPD_CAN_ABORT/*|wxPD_CAN_SKIP*/|wxPD_REMAINING_TIME); //SKIP not ready and Buggy
	pdlg.SetWindowStyleFlag( pdlg.GetWindowStyleFlag()|wxSTAY_ON_TOP|wxMINIMIZE_BOX );
	pdlg.Show();

	time_t ts,te;
	time (&ts);
	te=ts;
	unsigned read_speed=0;


	int64_t diffBuff[1*MB]; //up to 500K differences.
	//Structure is : even indices for difference start offset, odd indices for end offsets
	int diffHit = 0;
	bool diff=false;
	uint64_t drange = wxMin( f1.Length(), f2.Length() );
	if(drange == 0)
		drange++; //to avoid Gauge zero div error

	//Section Merge Code
	int compare_range=0;
	if( checkMergeSection->GetValue() )
		compare_range=spinMergeSection->GetValue();

	bool BreakDoubleFor=false;
	int rdBlockSz=1*MB;

	int rd1,rd2;
	char *buffer1=new char[rdBlockSz];
	char *buffer2=new char[rdBlockSz];
	char *buffer1_prefetch=new char[rdBlockSz];
	char *buffer2_prefetch=new char[rdBlockSz];
	int* bfr_int1;
	int* bfr_int2;
	int rd1_prefetch=0,rd2_prefetch=0;
//#ifdef _DEBUG_
	std::cout << "StopAfterNMatch:" << StopAfterNMatch << std::endl;
//#endif

//	for( uint64_t mb = 0 ; !(f1.Eof() || f2.Eof() || BreakDoubleFor) ; mb+=rdBlockSz){
	for( int64_t mb = 0 ; mb < wxMin(f1.Length(),f2.Length()) && !BreakDoubleFor ; mb+=rdBlockSz) {
		if(rd1_prefetch==0) {
//			buffer1.UngetWriteBuf( f1.Read(buffer1.GetWriteBuf( rdBlockSz ),rdBlockSz) );
//			buffer2.UngetWriteBuf( f2.Read(buffer2.GetWriteBuf( rdBlockSz ),rdBlockSz) );
			rd1=f1.Read(buffer1,rdBlockSz);
			rd2=f2.Read(buffer2,rdBlockSz);
			}
		else {
			std::swap(buffer1, buffer1_prefetch);
			std::swap(buffer2, buffer2_prefetch);
			rd1=rd1_prefetch;
			rd2=rd2_prefetch;
			}
#ifdef _DEBUG_COMPARE_
		std::cout << "Diff Compare Start Offset: " << mb << std::endl;
#endif
		bool show_limit_message=false;
//Not enabled due f1.Eof() loop. If read last byte, for loop get out immeditialy!
#ifdef _READ_PREFETCH_
		#pragma omp parallel sections
//		#pragma omp parallel
//		#pragma omp single nowait
			{
			//this preloads next data to swap buffer.
			#pragma omp section
			//#pragma omp task shared(f1,f2,rd1_prefetch,rd2_prefetch)
				{
				//precalculate next read location
//				buffer1_prefetch.UngetWriteBuf( f1.Read(buffer1_prefetch.GetWriteBuf( rdBlockSz ),rdBlockSz) );
//				buffer2_prefetch.UngetWriteBuf( f2.Read(buffer2_prefetch.GetWriteBuf( rdBlockSz ),rdBlockSz) );
				rd1_prefetch=f1.Read(buffer1_prefetch,rdBlockSz);
				rd2_prefetch=f2.Read(buffer2_prefetch,rdBlockSz);
				}
			#pragma omp section
			//#pragma omp task
				{
				bfr_int1 = reinterpret_cast<int*>(buffer1);
				bfr_int2 = reinterpret_cast<int*>(buffer2);

				for( int i = 0 ; (i < wxMin( rd1, rd2)) && !BreakDoubleFor; ) {
					if(diffHit >= 500000) {
						show_limit_message=true;
						BreakDoubleFor = true;
						//break; //not possible to use under OpenMP, instead use continue
						continue;
						}

					//Here we made the comparison on INTEGER for speedup
					if (i%sizeof(int)==0) //otherwise we can't find some diffs! Bug#190
						if( bfr_int1[i/sizeof(int)]==bfr_int2[i/sizeof(int)]  ) {
							//bytes are eq, goto check next integer
							if( !diff ) {
								i+=sizeof(int);
								continue;
								}
							}


					//If integer comparison is failed, here we made the comparison in byte
					if((buffer1[i] != buffer2[i]) == SearchForDiff) {
						//Different bytes found.
						if(!diff) { //Set difference start
#ifdef _DEBUG_COMPARE_
							std::cout << "Diff Start " << mb+i << " to " ;
#endif
							diff=true;
							if( ( (mb+i)-diffBuff[diffHit-1] ) <= compare_range &&
							      compare_range > 0 && diffHit>1) {
								diffHit--; //re-push old diff ending to stack
								//StopAfterNMatch++;//and count 2 difference as 1
#ifdef _DEBUG_COMPARE_
								std::cout << "StopAfterNMatch:" << StopAfterNMatch << std::endl;
#endif
								}
							else
								diffBuff[diffHit++]=mb+i;
							}

						//this adds latest diff stream to array if one file ends
						if( i+1 == wxMin( rd1, rd2) ) {
							//#pragma omp taskwait
							if(f1.Eof() || f2.Eof() ) {
								//avoid false file ends at using prefetches!
								if( wxMin(rd1_prefetch,rd2_prefetch) <= 0 )
									diffBuff[diffHit++]=mb+i;
								}
							}
						}

					else {			//bytes are eq.
						if(diff) { //Set difference end
#ifdef _DEBUG_COMPARE_
							std::cout << mb+i-1 << std::endl;
#endif
							diff=false;
							diffBuff[diffHit++]=mb+i-1;


							if( --StopAfterNMatch == 0 ) {
#ifdef _DEBUG_COMPARE_
								std::cout << "Break comparison due StopAfterNMatch." << std::endl ;
#endif
								BreakDoubleFor=true;
								break;
								//continue;//not possible to use under OpenMP, instead use continue
								}
							}
						}
					i++;
					}// Buffer comparison block for loop end
				}
			}//omp parallel end

		if(show_limit_message)
			wxMessageBox( wxString(_("Sorry, this program supports up to 500K differences."))+wxT("\n")+_("Remaining differences not shown."), _("Error on Comparison"));

#endif // _READ_PREFETCH_

		bool skip=false;
		read_speed+=rdBlockSz;
		//Progress window processing..
		time(&te);
		if(ts != te ) {
			ts=te;
			emsg = msg + wxT("\n") + _("Comparison Speed : ") + wxString::Format( wxT("%.2f "), 1.0*read_speed/MB) + _("MB/s");
			read_speed=0;
			}

		if( !pdlg.Update( mb/drange, emsg, &skip ) ) {
			f1.Close();
			f2.Close();
			return false;
			}

		if(skip)
			break;
		}//End of for loops / comparison

	delete [] buffer1;
	delete [] buffer2;
	delete [] buffer1_prefetch;
	delete [] buffer2_prefetch;

	pdlg.Show( false );

	if( flsave != wxEmptyString ) {
		wxString ln = _("File #1 : ") + fl1.GetFullPath() + wxT("\n")+_("File #2 : ") + fl2.GetFullPath() + wxT("\n\n");
		fs.Write( ln );
		wxString line;
		for(int i = 0 ; i < diffHit-1 ; i+=2) {
			line = wxString::Format( "%s found %" wxLongLongFmtSpec "u + %" wxLongLongFmtSpec "u bytes.\n", ( SearchForDiff ? wxT("Diff"):wxT("Match")), diffBuff[i], diffBuff[i+1]-diffBuff[i]+1 );
			fs.Write( line );
			}

		if( f1.Length() != f2.Length() ) {
			if( f1.Length() > f2.Length() )
				line =  wxString::Format( "\nFile #2 ends at offset %" wxLongLongFmtSpec "u. File #1 has extra %" wxLongLongFmtSpec "u bytes.\n",f2.Length(), f1.Length() - f2.Length() );

			else
				line =  wxString::Format( "\nFile #1 ends at offset %" wxLongLongFmtSpec "u. File #2 has extra %" wxLongLongFmtSpec "u bytes.\n",f1.Length(), f2.Length() - f1.Length() );

			fs.Write( line );
			}
		}
	int file1size= f1.Length();
	int file2size= f2.Length();
	f1.Close();
	f2.Close();
	fs.Close();

	HexEditor* hexeditor1 = parent->OpenFile( fl1 );
	HexEditor* hexeditor2 = parent->OpenFile( fl2, true);
	hexeditor1->SetFileAccessMode( FAL::ReadOnly );
	hexeditor2->SetFileAccessMode( FAL::ReadOnly );
//	parent->MyNotebook.Split(-1, wxRIGHT); //Not available due protected wxNotepad. Changed OpenFile function instead.

	if(checkConnectFiles->GetValue()) {
		hexeditor1->ConnectScroll( hexeditor2 );
		}

	if(hexeditor1 != NULL && hexeditor2 != NULL) {
		for(int i = 0 ; i < diffHit-1 ; i+=2) {
			TagElement *mytag=new TagElement(diffBuff[i], diffBuff[i+1],wxEmptyString,*wxBLACK, *wxRED );
			hexeditor1->CompareArray.Add(mytag);
			mytag=new TagElement(diffBuff[i], diffBuff[i+1],wxEmptyString,*wxBLACK, *wxRED );
			hexeditor2->CompareArray.Add(mytag);
			}
		if( file1size != file2size ) {
			if( file1size > file2size ) {
				TagElement *mytag=new TagElement(file2size, file1size,_("This part doesn't exist at compared file"),*wxBLACK, *wxCYAN );
				hexeditor1->CompareArray.Add(mytag);
				}
			else {
				TagElement *mytag=new TagElement(file1size, file2size,_("This part doesn't exist at compared file"),*wxBLACK, *wxCYAN );
				hexeditor2->CompareArray.Add(mytag);
				}
			}
		//Show first tag
		if( hexeditor1->CompareArray.Count() > 0 )
			hexeditor1->Goto( hexeditor1->CompareArray.Item(0)->start, true );
		if( hexeditor2->CompareArray.Count() > 0 ) //This check is needed if files are identical but one file has bigger than other.
			hexeditor2->Goto( hexeditor2->CompareArray.Item(0)->start, false );

		//Generate event to show compare panel
		wxUpdateUIEvent eventx( COMPARE_CHANGE_EVENT );
		wxPostEvent( parent, eventx );
		}

	return true;
	}

void CompareDialog::OnFileChange( wxFileDirPickerEvent& event ) {
#ifdef _DEBUG_COMPARE_
	std::cout << "CompareDialog::OnFileChange()" << std::endl;
#endif
	if( filePick1->GetPath() != wxEmptyString && filePick2->GetPath() != wxEmptyString)
		btnCompare->Enable(true);
	else
		btnCompare->Enable(false);
	}


void CompareDialog::EventHandler( wxCommandEvent& event ) {
#ifdef _DEBUG_COMPARE_
	std::cout << "CompareDialog::EventHandler()" << std::endl;
#endif
	if(event.GetId() == wxID_CANCEL)
		EndModal(event.GetId());
	else if(event.GetId() == btnCompare->GetId()) {
		if( !filePick1->GetPath().IsEmpty() && !filePick2->GetPath().IsEmpty()) {
			if( checkSaveResults->GetValue() && filePickSave->GetPath().IsEmpty() ) {
				wxMessageBox( _("Error, Save File is not selected.") );
				return;
				}
			///Note:Triggers stack Overflow on windows. Use bigger stack...
#ifndef __WXMSW__
			SetStackLimit();
#endif

			if( Compare( filePick1->GetPath(),			//First file.
			             filePick2->GetPath(),			//Second file to compare.
			             m_radioDifferent->GetValue(), //Compare diffs or same bytes option.
			             (checkStopCompare->GetValue() ? spinStopCompare->GetValue() : 0),	//Stop after N Matches. 0 means unlimited.
			             (checkSaveResults->GetValue() ? filePickSave->GetPath() : wxT("") ))		//comparison result save path.
			  )
				EndModal(event.GetId());
			}
		else
			wxBell();
		}
	else if( event.GetId() == checkStopCompare->GetId() )
		spinStopCompare->Enable(event.IsChecked());

	else if( event.GetId() == checkMergeSection->GetId() )
		spinMergeSection->Enable(event.IsChecked());

	else if( event.GetId() == checkSaveResults->GetId() )
		filePickSave->Enable(event.IsChecked());

	int options=0;
	options|=checkConnectFiles->GetValue() ? OPT_CMP_CONNECT : 0;
	options|=checkMergeSection->GetValue() ? OPT_CMP_MERGE_SECTION : 0;
	options|=checkStopCompare->GetValue() ? OPT_CMP_STOP_AFTER : 0;
	options|=checkSaveResults->GetValue() ? OPT_CMP_SAVE : 0;
	options|=m_radioDifferent->GetValue() ? OPT_CMP_SEARCH_DIFF : 0;
	myConfigBase::Get()->Write( _T("CompareOptions"), options );

	int optionStopAfter = spinStopCompare->GetValue();
	int optionMergeSection = spinMergeSection->GetValue();
	myConfigBase::Get()->Write( _T("CompareOptionStopAfter"), optionStopAfter );
	myConfigBase::Get()->Write( _T("CompareOptionMergeSection"), optionMergeSection );
	}

void CompareDialog::OnFileDrop( wxDropFilesEvent& event ) {
#ifdef _DEBUG_COMPARE_
	std::cout << "CompareDialog::OnFileDrop()" << std::endl;
#endif
	wxBell();
	if( filePick1->GetPath() != wxEmptyString && filePick2->GetPath() != wxEmptyString)
		btnCompare->Enable(true);
	else
		btnCompare->Enable(false);
	}

ChecksumDialog::ChecksumDialog( wxWindow* parent_ ):ChecksumDialogGui(parent_, wxID_ANY) {
	parent = static_cast< HexEditorFrame* >(parent_);
	bool active_hex = parent->GetActiveHexEditor() != NULL;
	chkFile->Enable(active_hex);
	chkFile->SetValue(active_hex);
	filePick->Enable(!active_hex);
	btnCalculate->Enable( active_hex );

	int options;
	myConfigBase::Get()->Read( _T("SelectedChecksumFunctions"), &options );

	chkMD2->SetValue( options & (1 << MHASH_MD2) );
	chkMD4->SetValue( options & (1 << MHASH_MD4) );
	chkMD5->SetValue( options & (1 << MHASH_MD5) );
	chkSHA1->SetValue( options & (1 << MHASH_SHA1) );
	chkSHA224->SetValue( options & (1 << MHASH_SHA224) );
	chkSHA256->SetValue( options & (1 << MHASH_SHA256) );
	chkSHA384->SetValue( options & (1 << MHASH_SHA384) );
	chkSHA512->SetValue( options & (1 << MHASH_SHA512) );
	chkRIPEMD128->SetValue( options & (1 << MHASH_RIPEMD128) );
	chkRIPEMD160->SetValue( options & (1 << MHASH_RIPEMD160) );
	chkRIPEMD256->SetValue( options & (1 << MHASH_RIPEMD256) );
	chkRIPEMD320->SetValue( options & (1 << MHASH_RIPEMD320) );

	chkHAVAL128->SetValue( options & (1 << MHASH_HAVAL128) );
	chkHAVAL160->SetValue( options & (1 << MHASH_HAVAL160) );
	chkHAVAL192->SetValue( options & (1 << MHASH_HAVAL192) );
	chkHAVAL224->SetValue( options & (1 << MHASH_HAVAL224) );
	chkHAVAL256->SetValue( options & (1 << MHASH_HAVAL256) );

	chkTIGER128->SetValue( options & (1 << MHASH_TIGER128) );
	chkTIGER160->SetValue( options & (1 << MHASH_TIGER160) );
	chkTIGER   ->SetValue( options & (1 << MHASH_TIGER192) );

	chkADLER32	->SetValue( options & (1 << MHASH_ADLER32) );
	chkCRC32		->SetValue( options & (1 << MHASH_CRC32) );
	chkCRC32B	->SetValue( options & (1 << MHASH_CRC32B) );
	chkWHIRLPOOL->SetValue( options & (1 << MHASH_WHIRLPOOL) );
	chkGOST		->SetValue( options & (1 << MHASH_GOST) );
	chkSNEFRU128->SetValue( options & (1 << MHASH_SNEFRU128) );
	chkSNEFRU256->SetValue( options & (1 << MHASH_SNEFRU256) );

	}

void ChecksumDialog::EventHandler( wxCommandEvent& event ) {
#ifdef _DEBUG_
	std::cout << "ChecksumDialog::EventHandler()" << std::endl;
#endif
	int options=0;
	options |= (chkMD2->GetValue()    ? 1 << MHASH_MD2    : 0);
	options |= (chkMD4->GetValue()    ? 1 << MHASH_MD4    : 0);
	options |= (chkMD5->GetValue()    ? 1 << MHASH_MD5    : 0);

	options |= (chkSHA1->GetValue()   ? 1 << MHASH_SHA1   : 0);
	//options |= (chkSHA192->GetValue() ? 1 << MHASH_SHA192 : 0);
	options |= (chkSHA224->GetValue() ? 1 << MHASH_SHA224 : 0);
	options |= (chkSHA256->GetValue() ? 1 << MHASH_SHA256 : 0);
	options |= (chkSHA384->GetValue() ? 1 << MHASH_SHA384 : 0);
	options |= (chkSHA512->GetValue() ? 1 << MHASH_SHA512 : 0);

	options |= (chkRIPEMD128->GetValue() ? 1 << MHASH_RIPEMD128 : 0);
	options |= (chkRIPEMD160->GetValue() ? 1 << MHASH_RIPEMD160 : 0);
	options |= (chkRIPEMD256->GetValue() ? 1 << MHASH_RIPEMD256 : 0);
	options |= (chkRIPEMD320->GetValue() ? 1 << MHASH_RIPEMD320 : 0);

	options |= (chkHAVAL128->GetValue() ? 1 << MHASH_HAVAL128 : 0);
	options |= (chkHAVAL160->GetValue() ? 1 << MHASH_HAVAL160 : 0);
	options |= (chkHAVAL192->GetValue() ? 1 << MHASH_HAVAL192 : 0);
	options |= (chkHAVAL224->GetValue() ? 1 << MHASH_HAVAL224 : 0);
	options |= (chkHAVAL256->GetValue() ? 1 << MHASH_HAVAL256 : 0);

	options |= (chkTIGER128->GetValue() ? 1 << MHASH_TIGER128 : 0);
	options |= (chkTIGER160->GetValue() ? 1 << MHASH_TIGER160 : 0);
	options |= (chkTIGER->GetValue()    ? 1 << MHASH_TIGER192 : 0);

	options |= (chkADLER32->GetValue()   ? 1 << MHASH_ADLER32 : 0);
	options |= (chkCRC32->GetValue()     ? 1 << MHASH_CRC32 : 0);
	options |= (chkCRC32B->GetValue()    ? 1 << MHASH_CRC32B : 0);
	options |= (chkWHIRLPOOL->GetValue() ? 1 << MHASH_WHIRLPOOL : 0);
	options |= (chkGOST->GetValue()      ? 1 << MHASH_GOST : 0);
	options |= (chkSNEFRU128->GetValue() ? 1 << MHASH_SNEFRU128 : 0);
	options |= (chkSNEFRU256->GetValue() ? 1 << MHASH_SNEFRU256 : 0);

	myConfigBase::Get()->Write( _T("SelectedChecksumFunctions"), options );

	if(event.GetId() == wxID_CANCEL)
		EndModal(event.GetId());

	else if(event.GetId() == btnCalculate->GetId()) {
		wxString msg;
		FAL *F=NULL;
		if( chkFile->GetValue() )
			F=parent->GetActiveHexEditor()->myfile;

		else if( filePick->GetPath() != wxEmptyString ) {
			wxFileName fl( filePick->GetPath() );
			F= new FAL( fl );
			}
		std::cout << F->Length() << std::endl;
		if( F->Length() == 0 ) {
			wxMessageBox(_("Cannot calculate checksum of an empty file."), _("Error"));
			return;
			}

		msg = CalculateChecksum( *F, options );
		if(msg != wxEmptyString)
			msg = _("File: ") + F->GetFileName().GetFullPath() + wxNewline
			      +(chkFile->GetValue() && F->IsChanged() ? _("Notice: Checksum includes non-saved changes.") + wxString(wxNewline) + wxNewline : wxNewline )
			      + msg;

		if(!chkFile->GetValue() && (F != NULL)) {
			F->Close();
			delete F;
			}

		if(wxTheClipboard->Open()) {
//			if (wxTheClipboard->IsSupported( wxDF_TEXT )){
			wxTheClipboard->Clear();
			wxTheClipboard->SetData( new wxTextDataObject( msg ));
			wxTheClipboard->Flush();
			wxTheClipboard->Close();
//			}
			}

		//Not looks so good due variable size of the text.
		wxMessageBox( msg + wxNewline + _("Results copied to the clipboard."), _("Checksum Results"),wxOK|wxCENTRE );

		}

	else if(event.GetId() == chkFile->GetId() ) {
		filePick->Enable( !event.IsChecked() );
		}

	if( options == 0 || !((filePick->GetPath() != wxEmptyString) || chkFile->GetValue()) )
		btnCalculate->Enable(false);
	else
		btnCalculate->Enable(true);
	}

void ChecksumDialog::OnFileChange( wxFileDirPickerEvent& event ) {
	wxCommandEvent e;
	EventHandler( e );
	}

wxString ChecksumDialog::CalculateChecksum(FAL& f, int options) {
	f.Seek(0);
	wxString msg = _("Please wait while calculating checksum.");
	wxString emsg = wxT("\n");
	wxProgressDialog mypd(_("Calculating Checksum"), msg+emsg, 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
	mypd.Show();
	//checksum_options_strings = { "MD5","SHA1","SHA256","SHA384","SHA512" };

	unsigned NumBits=0;
	for( unsigned i = 0; i < 32 ; i ++ ) {
		NumBits += (options>>i)&0x1;
		}
	MHASH *myhash=new MHASH[NumBits];
	unsigned i=0;

	hashid algs[]= {MHASH_MD2,MHASH_MD4,MHASH_MD5,
	                MHASH_SHA1,MHASH_SHA224,MHASH_SHA256,MHASH_SHA384,MHASH_SHA512,
	                MHASH_RIPEMD128,MHASH_RIPEMD160,MHASH_RIPEMD256,MHASH_RIPEMD320,
	                MHASH_HAVAL128,MHASH_HAVAL160,MHASH_HAVAL192,MHASH_HAVAL224,MHASH_HAVAL256,
	                MHASH_TIGER128,MHASH_TIGER160,MHASH_TIGER192,
	                MHASH_ADLER32,MHASH_CRC32,MHASH_CRC32B,MHASH_WHIRLPOOL,MHASH_GOST,MHASH_SNEFRU128,MHASH_SNEFRU256
	               };
	for( unsigned j = 0 ; j < sizeof algs/sizeof algs[0]; j++)
		if( options & (1 << algs[j] ))
			myhash[i++]= mhash_init(algs[j]);

	int rdBlockSz = MB;
	unsigned char *buffer = new unsigned char[rdBlockSz];
	unsigned char *buffer_prefetch = new unsigned char[rdBlockSz];
	int readed=rdBlockSz;
	int readed_prefetch=0;

	uint64_t readfrom=0,read_speed=0, range=f.Length();
	time_t ts,te;
	time (&ts);

	do {
		if(readed_prefetch>0) { //PreLoad file, +%50 speed on SSD buffers.
			readed=readed_prefetch;
			std::swap(buffer, buffer_prefetch);
			}
		else {
			readed=f.Read( buffer, rdBlockSz );
			}

		readfrom+=readed;
		read_speed+=readed;

		//Paralelize with OpenMP
		#pragma omp parallel shared(myhash) private(i)
			{
			//Use OpenMP for hash many cores at once.
			#pragma omp for schedule(dynamic) nowait
			for( unsigned i = 0 ; i < NumBits ; i++) {
				mhash( myhash[i], buffer, readed);
				}

			//this preloads next data to swap buffer
			#pragma omp single
				{
				readed_prefetch=f.Read( buffer_prefetch, rdBlockSz );
				}

			}

		time(&te);
		if(ts != te ) {
			ts=te;
			emsg = msg + wxT("\n") + _("Hash Speed : ") + wxString::Format( wxT("%.2f "), 1.0*read_speed/MB) + _("MB/s");
			read_speed=0;
			}
		if(!mypd.Update((readfrom*1000)/range, emsg )) {
			delete [] buffer;
			delete [] buffer_prefetch;
			return wxEmptyString;
			}
		}
	while(readed == rdBlockSz);

	delete [] buffer;
	delete [] buffer_prefetch;
	wxString results;
	i=0;

	unsigned char *hash;
	wxString AlgName;
	unsigned MaxAlgoName=0;
	for(unsigned i = 0 ; i < NumBits ; i++)
		MaxAlgoName = wxMax( MaxAlgoName, wxString::FromAscii( reinterpret_cast<const char*>(mhash_get_hash_name_static( mhash_get_mhash_algo(myhash[i]) ))).Len() );

	for(unsigned i = 0 ; i < NumBits ; i++) {
		AlgName = wxString::FromAscii( reinterpret_cast<const char*>(mhash_get_hash_name_static( mhash_get_mhash_algo(myhash[i]) )));
		results << AlgName;
		results << wxT(":");

		//Instead of tab char, we got better formatter loop here
		for( unsigned j = 0 ; j < MaxAlgoName +1 - AlgName.Len(); j++ )
			results << wxT(" ");

		hash = static_cast<unsigned char *>( mhash_end(myhash[i]) );
		unsigned hashblocksize=mhash_get_block_size( mhash_get_mhash_algo(myhash[i]));
		for (unsigned k = 0; k < hashblocksize ; k++)
			results << wxString::Format( wxT("%.2x"), hash[k]);

		results << wxNewline;
		}
	delete [] myhash;
	return results;//		checksum_options_strings = { "MD5","SHA1","SHA256","SHA384","SHA512" };
	}


XORViewDialog::XORViewDialog( wxWindow* parent, wxMemoryBuffer *XORKey_ ):XORViewDialogGui(parent, wxID_ANY) {
	XORKey=XORKey_;
	XORtext->SetFocus();
	}

void XORViewDialog::EventHandler( wxCommandEvent& event ) {
	if( event.GetId() == wxID_CANCEL ) {
		EndModal( wxID_CANCEL );
		}
	else if( event.GetId() == wxID_OK) {
		if( XORtext->GetValue()==wxEmptyString ) {
			wxBell();
			return;
			}
		if( radioHex->GetValue() ) {
			wxString hexval = XORtext->GetValue();
			if(!HexVerifyAndPrepare( hexval, wxT("XOR"), this ) )
				return;
			wxMemoryBuffer z = wxHexCtrl::HexToBin( hexval );
			XORKey->AppendData( z.GetData(), z.GetDataLen() );
			EndModal( wxID_OK );
			}
		else {
			XORKey->AppendData( XORtext->GetValue().ToAscii(), XORtext->GetValue().Len() );
			EndModal( wxID_OK );
			}
		}
	else if( event.GetId() == XORtext->GetId())
		XORtext->SetFocus();
	event.Skip();
	}

PreferencesDialog::PreferencesDialog( wxWindow* parent ):PreferencesDialogGui(parent, wxID_ANY) {
	GetInstalledLanguages( LangNames, LangIds );
	chcLang->Clear();
	LangNames.Sort();
	chcLang->Append( LangNames );
	wxConfigBase *pConfig = myConfigBase::Get();

	if ( ! pConfig->Read(_T("Language")).IsEmpty() ) {
		wxString lang = pConfig->Read(_T("Language"), wxEmptyString) ;
		if ( !lang.IsEmpty() )
			for( unsigned i = 0 ; i < LangNames.Count() ; i++)
				if( lang == LangNames.Item(i)) {
					chcLang->SetSelection(i);
					break;
					}
		}
	else
		chcLang->SetSelection(0);

	wxString TempString;
	bool TempBool;
	if( myConfigBase::Get()->Read( _T("ColourHexForeground"), &TempString) )				clrPickerForeground->SetColour( TempString );
	if( myConfigBase::Get()->Read( _T("ColourHexBackground"), &TempString) )				clrPickerBackground->SetColour( TempString );
	if( myConfigBase::Get()->Read( _T("ColourHexBackgroundZebra"), &TempString) )			clrPickerBackgroundZebra->SetColour( TempString );
	if( myConfigBase::Get()->Read( _T("ColourHexSelectionForeground"), &TempString) )	clrPickerSelectionForeground->SetColour(TempString);
	if( myConfigBase::Get()->Read( _T("ColourHexSelectionBackground"), &TempString) )	clrPickerSelectionBackground->SetColour(TempString);
	if( myConfigBase::Get()->Read( _T("AutoShowTagPanel"), &TempBool ) )						chkAutoShowTagPanel->SetValue( TempBool );
	if( myConfigBase::Get()->Read( _T("SingleInstance"), &TempBool ) )						chkSingleInstance->SetValue( TempBool );

	if( myConfigBase::Get()->Read( _T("UseCustomHexFormat"), &TempBool ) )					chkCustom->SetValue( TempBool );
	if( myConfigBase::Get()->Read( _T("CustomHexFormat"), &TempString	)	)					comboCustomHexFormat->SetValue( TempString );
	comboCustomHexFormat->Enable( chkCustom->IsChecked() );
	if( myConfigBase::Get()->Read( _T("useBytesPerLineLimit"), &TempBool	)	) 				chkBytePerLineLimit->SetValue( TempBool );
	spinBytePerLine->Enable( chkBytePerLineLimit->IsChecked() );
	if( myConfigBase::Get()->Read( _T("FakeBlockLines"), &TempBool	)	)						chkFakeBlockLines->SetValue( TempBool );
	textCtlFakeBlockSize->Enable( chkFakeBlockLines->IsChecked() );
	if( myConfigBase::Get()->Read( _T("FakeBlockSize"), &TempString )	) 						textCtlFakeBlockSize->SetValue( TempString );
	spinBytePerLine->Enable( chkBytePerLineLimit->IsChecked() );
	chkPortable->SetValue( wxFileExists(wxT("wxHexEditor.cfg")) );


	int TempInt;
	myConfigBase::Get()->Read( _T("BytesPerLineLimit"), &TempInt, 16);
	spinBytePerLine->SetValue( TempInt );

	myConfigBase::Get()->Read( _T("FontSize"), &TempInt, 10 );
	spinFontSize->SetValue( TempInt );

	if( AvailableEncodings.IsEmpty() )
		AvailableEncodings=GetSupportedEncodings();

	chcCharacterEncodingFamily->Clear();
	chcCharacterEncoding->Clear();

	wxArrayString ChrEncFamArray;
	ChrEncFamArray.Add(wxT("Code for Information Interchange"));
	ChrEncFamArray.Add(wxT("DOS"));
	ChrEncFamArray.Add(wxT("ISO/IEC"));
	ChrEncFamArray.Add(wxT("KOI"));
	ChrEncFamArray.Add(wxT("Macintosh"));
	ChrEncFamArray.Add(wxT("Industrial Standard"));
	ChrEncFamArray.Add(wxT("Extended Binary Coded Decimal Interchange Code"));
	ChrEncFamArray.Add(wxT("Windows"));
	ChrEncFamArray.Add(wxT("UTF"));
	ChrEncFamArray.Sort();
	ChrEncFamArray.Add(wxT("Experimental"));
	ChrEncFamArray.Add(wxT("Other"));
	chcCharacterEncodingFamily->Append(ChrEncFamArray);

	myConfigBase::Get()->Read( _T("CharacterEncodingFamily"), &TempString, wxT("DOS") );
	chcCharacterEncodingFamily->SetStringSelection( TempString );

	ExperimentalEncodingsList.Clear();
	for(unsigned i=0; i< AvailableEncodings.Count(); i++) {
		if(( AvailableEncodings.Item(i).Find( wxT("Industrial Standard") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("Arabic") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("Hebrew") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("ISCII") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("TSCII") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("ANSEL") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("AtariST") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("EUC") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("Big5") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("JIS") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("GBK") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("Thai") ) != wxNOT_FOUND ) ||
		      ( AvailableEncodings.Item(i).Find( wxT("Iran") ) != wxNOT_FOUND )
		  )
			ExperimentalEncodingsList.Add( AvailableEncodings.Item(i) );
		}

	wxCommandEvent e;
	e.SetString( TempString );
	e.SetId( chcCharacterEncodingFamily->GetId() );
	EventHandler( e );

	myConfigBase::Get()->Read( _T("CharacterEncoding"), &TempString, wxT("OEM - IBM PC/DOS CP437 - MS-DOS Latin US" ));
	if(TempString==wxT("Extended Binary Coded Decimal Interchange Code"))
		TempString=wxT("EBCDIC");
	if( !chcCharacterEncoding->SetStringSelection( TempString ) )
		chcCharacterEncoding->SetSelection( 0 );
	}

void PreferencesDialog::GetInstalledLanguages(wxArrayString & names, wxArrayLong & identifiers) {
	names.Clear();
	identifiers.Clear();
	const wxLanguageInfo * langinfo;
	wxString name = wxLocale::GetLanguageName(wxLANGUAGE_DEFAULT);
	if(!name.IsEmpty()) {
		names.Add(_T("Default"));
		identifiers.Add(wxLANGUAGE_DEFAULT);
		}

	names.Add(_T("English"));
	identifiers.Add(wxLANGUAGE_ENGLISH);

	wxString dirname;
	wxFileName flnm( wxGetApp().argv[0] );
	//This part scans default installation on linux directory for available catalogs - both on main and on local share
	for( int i = 0 ; i < 2 ; i++ ) {
		if( i == 0 )
			dirname = (_T("/usr/local/share/locale"));
		else dirname = (_T("/usr/share/locale"));
		if(wxDir::Exists( dirname )) {
			wxDir dir(dirname);
			for(bool cont = dir.GetFirst(&dirname,_T(""),wxDIR_DEFAULT);
			      cont;
			      cont = dir.GetNext(&dirname)) {
				langinfo = wxLocale::FindLanguageInfo(dirname);
				if(langinfo != NULL) {
					if(wxFileExists(
					         dir.GetName() + wxFileName::GetPathSeparator() +
					         dirname + wxFileName::GetPathSeparator() +
					         _T("LC_MESSAGES") + wxFileName::GetPathSeparator() +
					         _T("wxHexEditor") + _T(".mo"))) {

						bool duplicate = false;
						for( unsigned i = 0 ; i < identifiers.Count() ; i++ )	//Avoid duplicated locales
							if( identifiers.Item(i) == langinfo->Language ) {
								duplicate = true;
								break;
								}
						if ( duplicate ) break;

						names.Add(langinfo->Description);
						identifiers.Add(langinfo->Language);
						}
					}
				}
			}
		}

	//This part scans for for available catalogs on local directory, for Windows and Linux.
	dirname = (flnm.GetPath() + wxFileName::GetPathSeparator() + _T("locale"));
	if(wxDir::Exists( dirname )) {
		wxDir dir(dirname);
		for(bool cont = dir.GetFirst(&dirname,_T(""),wxDIR_DEFAULT);
		      cont;
		      cont = dir.GetNext(&dirname)) {
			langinfo = wxLocale::FindLanguageInfo(dirname);
			if(langinfo != NULL) {
				if(wxFileExists(
				         dir.GetName() + wxFileName::GetPathSeparator() +
				         dirname + wxFileName::GetPathSeparator() +
				         _T("wxHexEditor") + _T(".mo"))) {

					bool duplicate = false;
					for( unsigned i = 0 ; i < identifiers.Count() ; i++ )	//Avoid duplicated locales
						if( identifiers.Item(i) == langinfo->Language ) {
							duplicate = true;
							break;
							}
					if ( duplicate ) break;

					names.Add(langinfo->Description);
					identifiers.Add(langinfo->Language);
					}
				}
			}
		}

#ifdef __WXMAC__
	//This part scans for for available catalogs on Bundle directory,for Mac.
	dirname = (flnm.GetPath() + wxFileName::GetPathSeparator() +
	           _T("..") + wxFileName::GetPathSeparator() +
	           _T("Resources") + wxFileName::GetPathSeparator() +
	           _T("locale"));

	if(wxDir::Exists( dirname )) {
		wxDir dir(dirname);
		for(bool cont = dir.GetFirst(&dirname,_T(""),wxDIR_DEFAULT);
		      cont;
		      cont = dir.GetNext(&dirname)) {
			langinfo = wxLocale::FindLanguageInfo(dirname);
			if(langinfo != NULL) {
				if(wxFileExists(
				         dir.GetName() + wxFileName::GetPathSeparator() +
				         dirname + wxFileName::GetPathSeparator() +
				         _T("wxHexEditor") + _T(".mo"))) {

					bool duplicate = false;
					for( unsigned i = 0 ; i < identifiers.Count() ; i++ )	//Avoid duplicated locales
						if( identifiers.Item(i) == langinfo->Language ) {
							duplicate = true;
							break;
							}
					if ( duplicate ) break;

					names.Add(langinfo->Description);
					identifiers.Add(langinfo->Language);
					}
				}
			}
		}
#endif  //__WXMAC__
	}

//Could use EventHandler directly but that breaks GCC4.4 :p
void PreferencesDialog::SpinEventHandler( wxSpinEvent& event ) {
	myConfigBase::Get()->Write( _T("FontSize"), spinFontSize->GetValue() );
	myConfigBase::Get()->Flush();
	wxUpdateUIEvent eventx( RESET_STYLE_EVENT );
	wxPostEvent( GetParent(), eventx );
	}

void PreferencesDialog::EventHandler( wxCommandEvent& event ) {
	wxString PrevSelection;
	myConfigBase::Get()->Read( _T("CharacterEncoding"), &PrevSelection );

	if(event.GetId()==chcCharacterEncodingFamily->GetId()) {
		wxArrayString Encodings;
		if( event.GetString()==wxT("Experimental") )
			Encodings=ExperimentalEncodingsList;

		else if( event.GetString()==wxT("Other") ) {
			Encodings=AvailableEncodings;
			for(unsigned i=0; i< ExperimentalEncodingsList.Count(); i++)
				Encodings.Remove(ExperimentalEncodingsList.Item(i));

			wxArrayString families=chcCharacterEncodingFamily->GetStrings();
			for(unsigned f=0; f< families.Count(); f++)
				for(unsigned i=0; i< AvailableEncodings.Count(); i++) {
					wxString family=families.Item(f);
					if(family==wxT("Extended Binary Coded Decimal Interchange Code"))
						family=wxT("EBCDIC");
					if( AvailableEncodings.Item(i).Find( family ) != wxNOT_FOUND )
						Encodings.Remove( AvailableEncodings.Item(i) );
					}
			}
		else
			for(unsigned i=0; i< AvailableEncodings.Count(); i++) {
				wxString family=event.GetString();
				if(family==wxT("Extended Binary Coded Decimal Interchange Code"))
					family=wxT("EBCDIC");
				if(( AvailableEncodings.Item(i).Find( family ) != wxNOT_FOUND ) &&
				      (	ExperimentalEncodingsList.Index( AvailableEncodings.Item(i) )==wxNOT_FOUND) )
					Encodings.Add( AvailableEncodings.Item(i) );
				}

		Encodings.Sort();
		chcCharacterEncoding->Clear();
		chcCharacterEncoding->Append( Encodings );
		wxString Selection;
		myConfigBase::Get()->Read( _T("CharacterEncoding"), &Selection );
		if( !chcCharacterEncoding->SetStringSelection( Selection ) )
			chcCharacterEncoding->SetSelection( 0 );
		}
	/*
		if(event.GetId()==chcCharacterEncoding->GetId()){
			myConfigBase::Get()->Write( _T("CharacterEncoding"), chcCharacterEncoding->GetStringSelection() );
			myConfigBase::Get()->Flush();
			wxUpdateUIEvent eventx( REDRAW_EVENT );
			wxPostEvent( GetParent(), eventx );
			}
	*/
	if(event.GetId() == chkCustom->GetId() )
		comboCustomHexFormat->Enable( event.IsChecked() );
	if(event.GetId() == chkBytePerLineLimit->GetId() )
		spinBytePerLine->Enable( event.IsChecked() );
	if(event.GetId() == chkFakeBlockLines->GetId() ){
		textCtlFakeBlockSize->Enable( event.IsChecked() );
		uint64_t get_block_size=0;
		if(event.IsChecked())
			if( kMGT_ToUInt(textCtlFakeBlockSize->GetValue(), &get_block_size) )
				fake_block_size=get_block_size;
		fake_block_size=get_block_size;
		}


	if(event.GetId() == chkPortable->GetId() ) {
		if( event.IsChecked() ) {
			wxFile f(wxT("wxHexEditor.cfg"), wxFile::write );
			f.Close();
			}
		else {
			wxRemoveFile(wxT("wxHexEditor.cfg"));
			}
		}

	//Redrawing because we need to re-interpret the readed bytes.
	//SaveRegistry();
	}

void PreferencesDialog::SaveRegistry( void ) {
	myConfigBase::Get()->Write( _T("Language"), wxLocale::FindLanguageInfo( chcLang->GetString( chcLang->GetSelection() ) )->Description ); //Saves string into configuration file.

	myConfigBase::Get()->Write( _T("ColourHexForeground"), clrPickerForeground->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );
	myConfigBase::Get()->Write( _T("ColourHexBackground"), clrPickerBackground->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );
	myConfigBase::Get()->Write( _T("ColourHexBackgroundZebra"), clrPickerBackgroundZebra->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );
	myConfigBase::Get()->Write( _T("ColourHexSelectionForeground"), clrPickerSelectionForeground->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );
	myConfigBase::Get()->Write( _T("ColourHexSelectionBackground"), clrPickerSelectionBackground->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );

	myConfigBase::Get()->Write( _T("UseCustomHexFormat"), chkCustom->GetValue() );
	myConfigBase::Get()->Write( _T("CustomHexFormat"), comboCustomHexFormat->GetValue() );
	myConfigBase::Get()->Write( _T("FakeBlockLines"), chkFakeBlockLines->GetValue() );
	myConfigBase::Get()->Write( _T("FakeBlockSize"),  textCtlFakeBlockSize->GetValue() );

	uint64_t get_block_size=0;
	if(chkFakeBlockLines->GetValue())
		if( kMGT_ToUInt(textCtlFakeBlockSize->GetValue(), &get_block_size) )
			fake_block_size=get_block_size;
	fake_block_size=get_block_size;

	myConfigBase::Get()->Write( _T("UseBytesPerLineLimit"), chkBytePerLineLimit->GetValue() );
	myConfigBase::Get()->Write( _T("BytesPerLineLimit"), spinBytePerLine->GetValue());
	myConfigBase::Get()->Write( _T("CharacterEncodingFamily"), chcCharacterEncodingFamily->GetStringSelection() );
	myConfigBase::Get()->Write( _T("CharacterEncoding"), chcCharacterEncoding->GetStringSelection() );
	myConfigBase::Get()->Write( _T("FontSize"), spinFontSize->GetValue() );

	myConfigBase::Get()->Write( _T("AutoShowTagPanel"), chkAutoShowTagPanel->GetValue() );
	myConfigBase::Get()->Write( _T("SingleInstance"), chkSingleInstance->GetValue() );

	myConfigBase::Get()->Flush();

	wxUpdateUIEvent eventx( REDRAW_EVENT );
	wxPostEvent( GetParent(), eventx );
	}
void PreferencesDialog::OnSave( wxCommandEvent& event ) {
	//myConfigBase::Get()->Write( _T("Language"), LangIds.Item(chcLang->GetSelection()) );
	//myConfigBase::Get()->Write( _T("Language"), wxLocale::FindLanguageInfo( chcLang->GetString( chcLang->GetSelection() ) )->Language );
	SaveRegistry();
	Close();
	}

void PreferencesDialog::OnResetColours( wxCommandEvent& event ) {
	if( wxOK == wxMessageBox( _("Are you sure about resetting colours?"), _("Resetting Hex Colours"), wxOK|wxCANCEL) ) {
		myConfigBase::Get()->DeleteEntry(_T("ColourHexForeground"));
		myConfigBase::Get()->DeleteEntry(_T("ColourHexBackground"));
		myConfigBase::Get()->DeleteEntry(_T("ColourHexBackgroundZebra"));
		myConfigBase::Get()->DeleteEntry(_T("ColourHexSelectionForeground"));
		myConfigBase::Get()->DeleteEntry(_T("ColourHexSelectionBackground"));
		myConfigBase::Get()->Flush();

		wxUpdateUIEvent eventx( RESET_STYLE_EVENT );
		wxPostEvent( GetParent(), eventx );
		Close();
		}
	}

extern wxArrayString GetDeviceList( bool=true );

DeviceBackupDialog::DeviceBackupDialog( wxWindow* parent ):DeviceBackupDialogGui(parent, wxID_ANY) {
	wxArrayString disks = GetDeviceList();
	for( unsigned i =0 ; i < disks.Count() ; i++)
		disks[i]=disks.Item(i).AfterLast('/');

#ifdef __WXMSW__
	for( unsigned i=0 ; i < disks.Count() ; i++ )
		if(disks.Item(i).StartsWith(wxT("\\Device")))
			disks.RemoveAt(i);
#endif
	chcPartition->Append( disks );
	}

void DeviceBackupDialog::OnBackup( wxCommandEvent &event ) {
	wxArrayString disks = GetDeviceList();
	//disks.Sort();
	wxFileName src(disks.Item( chcPartition->GetSelection() ) );
	wxFileName dst(filePickBackup->GetPath());

	FAL src_fl(src);

	wxFFile dst_fl(dst.GetFullPath(), wxT("wb"));

	wxString msg = _("Please wait while backing up disk/partition image.");
	wxString emsg = wxT("\n");
	wxProgressDialog mypd(_("Disk/Partition Backup"), msg+emsg, 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
	mypd.Show();

	MHASH myhash = mhash_init( MHASH_MD5 );

	enum { rdBlockSz=MB };
	unsigned char buff[rdBlockSz];
	int rd=rdBlockSz;

	uint64_t readfrom=0,read_speed=0, range=src_fl.Length();
	time_t ts,te;
	time (&ts);

	while(rd == rdBlockSz) {
		rd=src_fl.Read(buff, rdBlockSz);
		read_speed+=rd;
		readfrom+=rd;
		if( static_cast<unsigned int>(rd) != dst_fl.Write( buff, rd ) ) {
			wxMessageBox( _("Error on writing to backup image."), _("ERROR!") );
			return;
			}
		mhash( myhash, buff, rd);

		time(&te);
		if(ts != te ) {
			ts=te;
			emsg = msg + wxT("\n") + _("Backup Speed : ") + wxString::Format( wxT("%.2f "), 1.0*read_speed/MB) + _("MB/s");
			read_speed=0;
			}

		if(!mypd.Update((readfrom*1000)/range, emsg )) {
			src_fl.Close();
			dst_fl.Close();
			wxRemoveFile(dst.GetFullPath());
			return;
			}
		}
	src_fl.Close();
	dst_fl.Close();

	wxString AlgName = wxString::FromAscii( reinterpret_cast<const char*>(mhash_get_hash_name_static( mhash_get_mhash_algo(myhash) )));
	wxFFile dst_hash(dst.GetFullPath()+wxT(".")+AlgName.Lower(), wxT("wb"));

	unsigned char *hash;
	hash = static_cast<unsigned char *>( mhash_end(myhash) );

	wxString result;
	for (unsigned k = 0; k < mhash_get_block_size( mhash_get_mhash_algo(myhash) ); k++)
		result << wxString::Format( wxT("%.2x"), hash[k]);

	dst_hash.Write( result );
	dst_hash.Write( wxT("  ") );
	dst_hash.Write( dst.GetFullName() );
	dst_hash.Close();

	wxMessageBox( _("Backup operation completed."), _("Operation Complete") );
	}

DeviceRestoreDialog::DeviceRestoreDialog( wxWindow* parent ):DeviceRestoreDialogGui(parent, wxID_ANY) {
	wxArrayString disks = GetDeviceList();
	for( unsigned i =0 ; i < disks.Count() ; i++)
		disks[i]=disks.Item(i).AfterLast('/');
#ifdef __WXMSW__
	for( unsigned i=0 ; i < disks.Count() ; i++ )
		if(disks.Item(i).StartsWith(wxT("\\Device")))
			disks.RemoveAt(i);
#endif
	chcPartition->Append( disks );
	}

void DeviceRestoreDialog::OnRestore( wxCommandEvent &event ) {
	wxArrayString disks = GetDeviceList();
	wxFileName dst(disks.Item( chcPartition->GetSelection() ) );
	wxFileName src(filePickBackup->GetPath());

	wxFFile src_fl(src.GetFullPath(), wxT("rb"));

	FAL dst_fl( dst, FAL::ReadWrite );

	if( wxCANCEL == wxMessageBox( _("WARNING! THIS OPERATION WILL DESTROY EVERY INFORMATION AT :\n")+dst.GetFullPath()+wxT("\n")+_("ARE YOU SURE?"), _("WARNING!"), wxOK|wxCANCEL|wxCENTRE, this ))
		return;

	if( src_fl.Length() > dst_fl.Length() ) {
		wxMessageBox( _("ERROR!: Disk space is smaller than the image file."), _("ERROR!"), wxCANCEL|wxCENTRE, this );
		return;
		}

	if( src_fl.Length() < dst_fl.Length() ) {
		if( wxCANCEL==wxMessageBox( _("WARNING!: Disk space is bigger than the image file. \nRemaining space will remain untouched."), _("WARNING!"), wxOK|wxCANCEL|wxCENTRE, this ) )
			return;
		}

	wxString msg = _("Please wait while restoring disk/partition.");
	wxString emsg = wxT("\n");
	wxProgressDialog mypd(_("Disk/Partition Restore"), msg+emsg, 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
	mypd.Show();

	enum { rdBlockSz=MB };
	unsigned char buff[rdBlockSz];
	int rd=rdBlockSz;

	uint64_t readfrom=0,read_speed=0, range=src_fl.Length();
	time_t ts,te;
	time (&ts);


	while(rd == rdBlockSz) {
		rd=src_fl.Read(buff, rdBlockSz);
		read_speed+=rd;
		readfrom+=rd;

		dst_fl.BlockWrite( buff, rd );

		time(&te);
		if(ts != te ) {
			ts=te;
			emsg = msg + wxT("\n") + _("Restore Speed : ") + wxString::Format( wxT("%.2f "), 1.0*read_speed/MB) + _("MB/s");
			read_speed=0;
			}
		if(!mypd.Update((readfrom*1000)/range, emsg ))
			return;
		}
	src_fl.Close();
	dst_fl.Close();
	wxMessageBox( _("Restore operation completed."), _("Operation Complete") );
	}

DeviceEraseDialog::DeviceEraseDialog( wxWindow* parent ):DeviceEraseDialogGui(parent, wxID_ANY) {
	wxArrayString disks = GetDeviceList();
	for( unsigned i =0 ; i < disks.Count() ; i++)
		disks[i]=disks.Item(i).AfterLast('/');
	chcPartition->Append( disks );
	}

void DeviceEraseDialog::OnErase( wxCommandEvent &event ) {
	wxArrayString disks = GetDeviceList();
	wxFileName dst(disks.Item( chcPartition->GetSelection() ) );
	FAL dst_fl( dst, FAL::ReadWrite );

	if( wxCANCEL == wxMessageBox( _("WARNING! THIS OPERATION WILL DESTROY EVERY INFORMATION AT :\n")+dst.GetFullPath()+wxT("\n")+_("ARE YOU SURE?"), _("WARNING!"), wxOK|wxCANCEL|wxCENTRE, this ))
		return;

	wxString msg = _("Please wait while erasing disk/partition image.");
	wxString emsg = wxT("\n");
	wxProgressDialog mypd(_("Disk/Partition Erase"), msg+emsg, 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
	mypd.Show();

	enum { rdBlockSz=MB };
	unsigned char buff[rdBlockSz];
	int rd=rdBlockSz;

	uint64_t readfrom=0,read_speed=0, range=dst_fl.Length();
	time_t ts,te;
	time (&ts);

	int sel = radioErase->GetSelection();
	memset( buff, (sel == 0 ? 0x00 : 0xFF ), rdBlockSz);

	srand((unsigned)time(0));

	while(readfrom < range) {

// TODO (death#1#): Speed up for random
		if( sel == 2 )
			for( unsigned i = 0 ; i < rdBlockSz ; i++ )
				buff[i]=rand()%0xFF;

		if( readfrom+rd > range )
			rd = range-readfrom;

		dst_fl.BlockWrite( buff, rd );
		read_speed+=rd;
		readfrom+=rd;

		time(&te);
		if(ts != te ) {
			ts=te;
			emsg = msg + wxT("\n") + _("Erase Speed : ") + wxString::Format( wxT("%.2f "), 1.0*read_speed/MB) + _("MB/s");
			read_speed=0;
			}
		if(!mypd.Update((readfrom*1000)/range, emsg ))
			return;
		}

	wxMessageBox( _("Erase operation completed."), _("Operation Complete") );

	}

OSXMessageDialog::OSXMessageDialog( wxWindow* parent, wxString message, wxString Caption):OSXMessageDialogGui(parent, wxID_ANY) {
	txtMessage->SetLabel( message );
	txtCaption->SetLabel( Caption );
	wxIcon wxHexEditor_ICON ( wxhex_xpm );
	icon_bitmap->SetIcon(wxHexEditor_ICON);
	this->Fit();
	this->Layout();
	}

void OSXwxMessageBox( wxString message, wxString Caption, int x, wxWindow* parent ) {
#ifndef __WXMAC__
	wxMessageBox( message,Caption,x, parent );
#else
	OSXMessageDialog osxmsg(parent, message, Caption );
	osxmsg.ShowModal();
#endif
	}
