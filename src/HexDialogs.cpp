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

#define NANINT 0xFFFFFFFFFFFFFFFFLL
#include "HexDialogs.h"
#include <wx/progdlg.h>
#include "../mhash/include/mhash.h"

#ifdef _OPENMP
   #include <omp.h>
#endif

#ifdef __WXMSW__
	#define wxNewline wxT("\r\n")
#else
	#define wxNewline wxT("\n")
#endif

#ifndef __WXMSW__
	#include <sys/resource.h>

void SetStackLimit(void){
	const rlim_t kStackSize = 32L * 1024L * 1024L;
	struct rlimit rl;
	int result;

	result = getrlimit(RLIMIT_STACK, &rl);
	fprintf(stderr, "current stack limit = %d\n", rl );
	if (result == 0){
		  if (rl.rlim_cur < kStackSize)
		  {
				rl.rlim_cur = kStackSize;
				result = setrlimit(RLIMIT_STACK, &rl);
				if (result != 0)
				{
					 fprintf(stderr, "setrlimit returned result = %d\n", result);
				}
		  }
		}
	}
#endif

bool HexVerifyAndPrepare(wxString& hexval, wxString Value_Name, wxWindow* parent) {
   if( hexval.Len() < 2 )
      return false;

   for( unsigned i = 0 ; i < hexval.Len() ; i++ )
      if( !isxdigit( hexval[i] ) or hexval == ' ' ) { //Not hexadecimal!
         OSXwxMessageBox( _("Value is not hexadecimal!"), _("Format Error!"), wxOK, parent );
         wxBell();
         return false;
         }
   //Remove all space chars and update the Search value
   while( hexval.find(' ') != -1 )
      hexval.Remove( hexval.find(' '),1);

   //there is odd hex value, must be even digit for search!
   if( hexval.Len() % 2 ) {
      OSXwxMessageBox( _("Value must be even digit!"), _("Format Error!"), wxOK, parent );
      wxBell();
      return false;
      }
   return true;
   }

//Prepares and updates combo boxes with 10 elements, sycnhronizing with wxConfig..
void ComboBoxFill( wxString KeyName, wxComboBox* CurrentBox, bool AddString){
	wxString TempString;
	wxArrayString SearchStrings;
	//Prepare Array;
	for( int i = 0 ; i < 10 ; i ++)
		if (wxConfigBase::Get()->Read(KeyName+wxEmptyString<<i , &TempString)){
			SearchStrings.Add( TempString );
			#ifdef _DEBUG_
			std::cout << "ComboBoxFill Read (" << i << ") : " << TempString.ToAscii() << std::endl;
			#endif
			}

	wxString AddNewString = CurrentBox->GetValue();
	//Adds New String
	if( AddString and (AddNewString not_eq wxEmptyString) ){
		if( SearchStrings.Index( AddNewString ) not_eq wxNOT_FOUND )
			SearchStrings.Remove( AddNewString );
		else if( SearchStrings.Count() >= 10 )
			SearchStrings.RemoveAt(0);
		SearchStrings.Add( AddNewString );

		for( unsigned i = 0 ; i < 10 and i < SearchStrings.Count(); i ++){
			#ifdef _DEBUG_
			std::cout << "ComboBoxFill Write (" << i << ") : " << SearchStrings.Item(i).ToAscii() << std::endl;
			#endif
			wxConfigBase::Get()->Write(KeyName+wxEmptyString<<i,  SearchStrings.Item(i));
			}
		}
	//Set ComboBox
	unsigned i;
	for( i = 0 ; i < SearchStrings.Count()  ; i ++ )
		CurrentBox->SetString( i, SearchStrings.Item(SearchStrings.Count()-i-1) );
	for( ; i < 10 ; i ++ )
		CurrentBox->SetString( i, wxEmptyString);
	#ifdef __WXMSW__ //Or windows leaves empty the gauge!
		CurrentBox->SetSelection(0);
	#endif
	}

GotoDialog::GotoDialog( wxWindow* _parent, uint64_t& _offset, uint64_t _cursor_offset, uint64_t _filesize, unsigned _BlockSize ):GotoDialogGui(_parent, wxID_ANY){
	offset = &_offset;
	cursor_offset = _cursor_offset;
	is_olddec = true;
	filesize  = _filesize;
	BlockSize = _BlockSize;
	parent= static_cast< HexEditor* >(_parent);
	SetAutoLayout(true);

	int options=0;
	wxConfigBase::Get()->Read( _T("GoToOptions"), &options );

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
	else{
		m_choiceMode->Append(_("Sector") );
		if(not (options & OPT_OFFSET_MODE)){
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

void GotoDialog::EventHandler( wxCommandEvent& event ){
//	if( event.GetId() == m_dec->GetId()
//		or event.GetId() == m_hex->GetId()
//		)
//		OnConvert( event );

	if( event.GetId()==m_choiceMode->GetId()){
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

	wxConfigBase::Get()->Write( _T("GoToOptions"), options );

	m_comboBoxOffset->SetFocusFromKbd();
	}

wxString GotoDialog::Filter( wxString text ){
	for( unsigned i = 0 ; i < text.Length() ; i++ ){
		if( not (m_dec->GetValue() ? isdigit( text.GetChar( i ) ) : isxdigit( text.GetChar( i ) )))
			text.Remove( i--, 1);
		}
	return text;
	}

void GotoDialog::OnInput( wxCommandEvent& event ){
 	wxString OldValue = m_comboBoxOffset->GetValue();
 	wxString NewValue = Filter( OldValue );
 	if( OldValue not_eq NewValue ){
		m_comboBoxOffset->SetValue( NewValue );
		m_comboBoxOffset->SetInsertionPointEnd();
		wxBell();
		}
 	event.Skip(true);
	}

void GotoDialog::OnChar( wxKeyEvent& event ){
	if(	( event.GetKeyCode() == WXK_BACK )
		or ( event.GetKeyCode() == WXK_RETURN )
		or ( event.GetKeyCode() == WXK_LEFT )
		or ( event.GetKeyCode() == WXK_RIGHT )
		or ( event.GetKeyCode() == WXK_DELETE )
		or ( event.GetKeyCode() == WXK_TAB )
		or ( event.GetKeyCode() == WXK_END )
		or ( event.GetKeyCode() == WXK_HOME )
		or ( event.GetKeyCode() == WXK_CAPITAL )
		or ( event.GetKeyCode() == WXK_RETURN )
		or ( event.ControlDown() and (
					 event.GetKeyCode()==1 			// CTRL+A
				 or event.GetKeyCode()==24	 		// CTRL+X
				 or event.GetKeyCode()==3			// CTRL+C
				 or event.GetKeyCode()==22 		// CTRL+V
				 ))
		or	( m_dec->GetValue() ? isdigit(event.GetKeyCode()) : isxdigit( event.GetKeyCode()) )
			)
		event.Skip(true);
	else{
		wxBell();
		event.Skip(false);
		}
	#ifdef _DEBUG_
	std::cout << "CTRL:" << event.ControlDown() << " KeyCode:" << event.GetKeyCode() << " Raw:" << event.GetRawKeyCode() << std::endl ;
	#endif
	if( event.GetKeyCode() == WXK_ESCAPE ){
		if(m_comboBoxOffset->GetValue().IsEmpty())
			Close();
		else
			m_comboBoxOffset->SetValue(wxEmptyString);
		}
	}

void GotoDialog::OnPreviousSector( wxCommandEvent& event ){
	//parent->Goto( parent->CursorOffset() - BlockSize);
	parent->LoadFromOffset( parent->CursorOffset() - BlockSize );
	}
void GotoDialog::OnNextSector( wxCommandEvent& event ){
	//parent->Goto( parent->CursorOffset() + BlockSize );
	parent->LoadFromOffset( parent->CursorOffset() + BlockSize );
	}

void GotoDialog::OnGo( wxCommandEvent& event ){
	if( m_comboBoxOffset->GetValue()==wxEmptyString ){
		wxBell();
		return;
		}
	wxULongLong_t wxul = 0;
	if( not m_comboBoxOffset->GetValue().ToULongLong( &wxul, m_dec->GetValue() ? 10 : 16 ))//Mingw32/64 workaround
		wxul = strtoull( m_comboBoxOffset->GetValue().ToAscii(), '\0', m_dec->GetValue() ? 10 : 16 );
	*offset = wxul;

	//Store value to Registry or Updates it!
	ComboBoxFill( _T("GotoOffset"), m_comboBoxOffset, true);

	unsigned SectorSize=1;
	if( m_choiceMode->GetSelection() == 1)
		SectorSize=BlockSize;

	if( m_branch->GetSelection() == 1)
		*offset += cursor_offset*SectorSize;
	else if( m_branch->GetSelection() == 2)
		*offset = filesize - *offset*SectorSize;
	else
		*offset *= SectorSize;

	if( *offset < 0 )
		*offset = 0;
	EndModal( wxID_OK );
	}

// TODO (death#1#):Paint 4 Find All
// TODO (death#1#):Remember options last state
FindDialog::FindDialog( wxWindow* _parent, FAL *_findfile, wxString title ):FindDialogGui( _parent, wxID_ANY, title){
	parent = static_cast< HexEditor* >(_parent);
	findfile = _findfile;

	int options=0;
	wxConfigBase::Get()->Read( _T("FindOptions"), &options );
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

	//SearchAtBufferUnitTest();
	}

bool FindDialog::SearchAtBufferUnitTest(void){
	unsigned STEP = 100000;
	char buff[STEP];
	for(unsigned i=0;i<STEP;i++) buff[i]=0;
	char src[] = "keyword";
	for(unsigned i=0; i < STEP ; i++){
		//bzero(buff, MB);
		unsigned sz = i + strlen(src) > STEP ? STEP-i : strlen(src);
		memcpy( buff+i, src, sz );
		UTF8SpeedHack[0]=toupper(src[0]);
		UTF8SpeedHack[1]=toupper(src[1]);
		//Manully change OPTIONS for testing!
		int f = SearchAtBuffer( buff, STEP, src, strlen(src), SEARCH_TEXT|SEARCH_MATCHCASE );
		//int f = SearchAtBufferMultiThread( buff, STEP, src, strlen(src), SEARCH_TEXT);
		#ifdef _DEBUG_
		if( f not_eq i )
			std::cout << "For key at : "<< i << "\t result = " << f << "\t sz: " << sz << std::endl;
		else
			std::cout << "Searching key at: " << i << " OK\r";
		std::cout.flush();
		#endif
		}
	return true;
	}

void FindDialog::PrepareComboBox( bool AddString ){
	int searchType = m_searchtype->GetSelection() == 0 ? SEARCH_TEXT : SEARCH_HEX;
	if( searchType == SEARCH_TEXT )
		ComboBoxFill( _T("SearchTextString"), m_comboBoxSearch, AddString);
	else
		ComboBoxFill( _T("SearchHexString"), m_comboBoxSearch, AddString);
	}

void FindDialog::OnChar( wxKeyEvent& event ){
	if( m_searchtype->GetSelection()==1 ){ //Just for Hex Mode
		if(	( event.GetKeyCode() == WXK_BACK )
			or ( event.GetKeyCode() == WXK_RETURN )
			or ( event.GetKeyCode() == WXK_LEFT )
			or ( event.GetKeyCode() == WXK_RIGHT )
			or ( event.GetKeyCode() == WXK_DELETE )
			or ( event.GetKeyCode() == WXK_TAB )
			or ( event.GetKeyCode() == WXK_END )
			or ( event.GetKeyCode() == WXK_HOME )
			or ( event.GetKeyCode() == WXK_CAPITAL )
			or ( event.GetKeyCode() == WXK_RETURN )
			or ( event.ControlDown() and (
						event.GetKeyCode()==1 			// CTRL+A
					 or event.GetKeyCode()==24	 		// CTRL+X
					 or event.GetKeyCode()==3			// CTRL+C
					 or event.GetKeyCode()==22 		// CTRL+V
					 ))
			or	( isxdigit( event.GetKeyCode()) )
				)
			event.Skip(true);
		else{
			wxBell();
			event.Skip(false);
			}
		}
	else{
		event.Skip( true );
		}

	#ifdef _DEBUG_
	std::cout << "CTRL:" << event.ControlDown() << " KeyCode:" << event.GetKeyCode() << " Raw:" << event.GetRawKeyCode() << std::endl ;
	#endif
	if( event.GetKeyCode() == WXK_ESCAPE ){
		if( static_cast<wxComboBox*>(event.GetEventObject())->GetValue().IsEmpty() )
			Close();
		else
			static_cast<wxComboBox*>(event.GetEventObject())->SetValue(wxEmptyString);
		}
	}

void FindDialog::EventHandler( wxCommandEvent& event ){
	parent->HighlightArray.Clear();
	if( event.GetId() == btnFind->GetId())
		OnFind();
	else if(event.GetId() == m_comboBoxSearch->GetId()){
		if( event.GetEventType() == 10041)//Handles ENTER key
			OnFind();
		else
			chkUTF8->SetValue( not m_comboBoxSearch->GetValue().IsAscii() );
		}
	else if( event.GetId() == m_searchtype->GetId()){
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
	else if( event.GetId() == chkUTF8->GetId() ){
		chkUTF8->SetValue( not chkUTF8->GetValue( ));
		wxBell();
		}

	int options=0;
	options |= m_searchtype->GetSelection() == 0 ? SEARCH_TEXT : SEARCH_HEX;
	options |= chkWrapAround->GetValue() ? SEARCH_WRAPAROUND : 0;
	options |= chkSearchBackwards->GetValue() ? SEARCH_BACKWARDS : 0;
	options |= chkUTF8->GetValue() ? SEARCH_UTF8 : 0;
	options |= chkMatchCase->GetValue() ? SEARCH_MATCHCASE : 0;
	wxConfigBase::Get()->Write( _T("FindOptions"), options );
	}

void FindDialog::FindSomeBytes( void ){
	wxString msg= _("Finding Some Bytes... ");
	wxString emsg;
	wxProgressDialog progress_gauge(_("wxHexEditor Searching") , msg, 1000,  this, wxPD_SMOOTH|wxPD_REMAINING_TIME|wxPD_CAN_ABORT|wxPD_AUTO_HIDE );
	progress_gauge.SetWindowStyleFlag( progress_gauge.GetWindowStyleFlag()|wxSTAY_ON_TOP|wxMINIMIZE_BOX );
// TODO (death#1#): Search icon	//wxIcon search_ICON (?_xpm);
	//progress_gauge.SetIcon(search_ICON);

	uint64_t current_offset = parent->CursorOffset();
	int BlockSz= 10*1024*1024;
	int search_step = parent->FileLength() < BlockSz ? parent->FileLength() : BlockSz ;
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
	//Search step 1: From cursor to file end.
	do{
		findfile->Seek( current_offset, wxFromStart );
		readed = findfile->Read( buffer , search_step );
		read_speed += readed;
		for( int i=0; i < readed ; i++)
			if( buffer[i] != diff_search ){
				parent->Goto( current_offset+i );
				//Destroy();
				return;
				}

		time(&te);
		if(ts != te ){
				ts=te;
				emsg = msg + wxT("\n") + wxString::Format(_("Search Speed : %.2f MB/s"), 1.0*read_speed/MB) + wxT("\n");
				read_speed=0;
				}
		if( ! progress_gauge.Update(current_offset*1000/parent->FileLength(), emsg))		// update progress and break on abort
			break;

		current_offset += readed;
		}while(readed >= search_step); //indicate also file end.
	wxBell();
	}

bool FindDialog::OnFind( bool internal ){
	#ifdef _DEBUG_
		std::cout << "FindDialog::OnFind() " << std::endl;
	#endif
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
		else{
			search_size=0;
			while( m_comboBoxSearch->GetValue().ToUTF8()[search_size++]);
			search_size--;
			}
		found = FindText( m_comboBoxSearch->GetValue(), parent->CursorOffset()+(options&SEARCH_BACKWARDS ? 0:1), options ); //+1 for forward operations!
		}
	else { //SEARCH_HEX
		//Hex Validation and Format
		wxString hexval = m_comboBoxSearch->GetValue();
		if(not HexVerifyAndPrepare( hexval, _("Search"), this ) )
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

uint64_t FindDialog::FindText( wxString target, uint64_t start_from, unsigned options ){
	#ifdef _DEBUG_
		std::cout << "FindText() from: " << start_from << "\t Search Length " << target.Len() << std::endl;
	#endif
	wxMemoryBuffer textsrc;
	if(not (options & SEARCH_MATCHCASE)){
		target = target.Lower();
		//!Bad speed hack here.
		UTF8SpeedHack[0]=target.Upper()[0];
		}

	if( target.IsAscii() ){
		textsrc.AppendData( target.ToAscii() , target.Length() );
		return FindBinary( textsrc, start_from, options );
		}

	else{//Search as UTF string.
		wxCharBuffer a = target.ToUTF8().data(); //Convert to UTF8 Binary
		int i=0;
		char *b=a.data();							//Silences errors
		while(b[i++] not_eq 0);					//Find stream size
		textsrc.AppendData( a , i-1 );//-1 for discard null termination char

		//!!Ugly hack here.
		if(not (options & SEARCH_MATCHCASE)){//If non-matchcase operation
			wxCharBuffer a = target.Upper().ToUTF8().data(); 	//Calculate Uppercase variant of first character
			UTF8SpeedHack[0]=a.data()[0];								//This will be huge speed up but needed to handled differently on
			UTF8SpeedHack[1]=a.data()[1];								//SearchAtBuffer stack!
			}
		return FindBinary( textsrc, start_from, options|SEARCH_UTF8 );
		}
	}

// TODO (death#1#): FindDialog::FindBinaryUnitTest()
bool FindDialog::SkipRAM( uint64_t& current_offset, unsigned search_size, unsigned& search_step, bool backward ){
	uint64_t map_start;
	uint64_t map_end;
	#ifdef _DEBUG_
	printf( "SkipRAM() Processing from 0x%llX, %s\n", current_offset, (backward ? "Backward": ""));
	#endif
	if( not parent->ProcessRAM_FindMap( current_offset, map_start, map_end, backward ) ){
		current_offset = parent->ProcessRAM_FindNextMap(current_offset, backward);
		#ifdef _DEBUG_
		printf( "Skipping to 0x%llX\n", current_offset);
		#endif
		}

	if(backward){
		if( current_offset - map_start < search_step )
			search_step = current_offset - map_start;
		}
	else{
		if( map_end - current_offset > search_step )
			search_step = map_end - current_offset;
		}

	if( search_step < search_size){
		current_offset = parent->ProcessRAM_FindNextMap(current_offset, backward);
		#ifdef _DEBUG_
		printf( "Skipping to 0x%llX due step size.\n", current_offset);
		#endif
		}
	#ifdef _DEBUG_
	std::cout << "Step size:" << search_step << std::endl;
	#endif

	if( map_start <= current_offset and map_end >= current_offset ){
		std::cout << "ProcessRAM_FindMap pass\n" << std::endl;
		}
	if( current_offset == 0 ) //means memory map ends
		return true;
	return false;
	}

uint64_t FindDialog::FindBinary( wxMemoryBuffer target, uint64_t from, unsigned options ){
	#ifdef _DEBUG_
		std::cout << "FindDialog::FindBinary() From:" << from << std::endl;
	#endif
	if( target.GetDataLen() == 0 ){
		OSXwxMessageBox( wxT("FindBinary() function called with Empty Target!\n"), _("Error"), wxOK, this);
		return NANINT;
		}
	wxString msg= _("Finding matches... ");
	wxString emsg;
	wxProgressDialog progress_gauge(_("wxHexEditor Searching") , msg, 1000,  this, wxPD_SMOOTH|wxPD_REMAINING_TIME|wxPD_CAN_ABORT|wxPD_AUTO_HIDE );
	progress_gauge.SetWindowStyleFlag( progress_gauge.GetWindowStyleFlag()|wxSTAY_ON_TOP|wxMINIMIZE_BOX );
// TODO (death#1#): Search icon	//wxIcon search_ICON (?_xpm);
	//progress_gauge.SetIcon(search_ICON);

	uint64_t current_offset = from;
	unsigned BlockSz= 1024*1024;
	unsigned search_step = parent->FileLength() < BlockSz ? parent->FileLength() : BlockSz ;
	findfile->Seek( current_offset, wxFromStart );
	char* buffer = new char [search_step];
	if(buffer == NULL) return NANINT;
	// TODO (death#6#): insert error check message here
	int found = -1;
	int readed = 0;

	time_t ts,te;
	time (&ts);
	te=ts;
	unsigned read_speed=0;
	unsigned percentage=0;
	uint64_t processfootprint=0;
	if( findfile->IsProcess() )
		processfootprint = parent->ProcessRAM_GetFootPrint();
	uint64_t totalread=0;


// TODO (death#1#): MemorySearch Backward.
	if( options & SEARCH_BACKWARDS and not ( options & SEARCH_FINDALL ) ){ //SEARCH_FINDALL forces Forward searching
		//BACKWARD SEARCH!
		uint64_t current_offset = from;
		uint64_t backward_offset = current_offset;
		//Search Step 1: Backward search
		int first_search=1;
		do{
			if( findfile->IsProcess() ){
				search_step=BlockSz;
				if( SkipRAM( current_offset, target.GetDataLen(), search_step, true ) ) //SkipRAM adjust current_offset and search_step
					break;
				}
			backward_offset = current_offset < search_step ? 0 : current_offset-search_step;
			search_step = backward_offset + search_step > current_offset ? current_offset-backward_offset : search_step;
			findfile->Seek( backward_offset, wxFromStart );
			#ifdef _DEBUG_
				std::cout << "FindBinary() BACKWARD1 " << backward_offset << "-" << current_offset << " \t " << "SearchStep:" << search_step<< std::endl;
			#endif
			if(first_search--)
				readed=findfile->Read( buffer , search_step+target.GetDataLen()-1 );
			else
				readed=findfile->Read( buffer , search_step ); //Up TO FROM!!!
			read_speed += readed;

			found = SearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options );//Makes raw search here

			if(found >= 0){
				delete [] buffer;
				return backward_offset+found;
				}
			else
				current_offset = backward_offset + target.GetDataLen(); //Unprocessed bytes

			//Gauge update
			time(&te);
			if(ts != te ){
					ts=te;
					emsg = msg + wxT("\n") + wxString::Format(_("Search Speed : %.2f MB/s") , 1.0*read_speed/MB) + wxT("\n");
					read_speed=0;
					}
			totalread += readed;

			if( findfile->IsProcess() ){
				percentage = options & SEARCH_WRAPAROUND
								 ? (processfootprint-totalread)*1000 / processfootprint
								 :	parent->ProcessRAM_GetVirtualOffset(current_offset)*1000/(parent->ProcessRAM_GetVirtualOffset(from)+1); //+1 to avoid error
				}
			else
				percentage =  options & SEARCH_WRAPAROUND
									? (parent->FileLength()-(from-current_offset))*1000/(parent->FileLength())
									: current_offset*1000/(1+from);  //+1 to avoid error

			if( ! progress_gauge.Update( percentage, msg))		// update progress and break on abort
				break;

			}while(current_offset > target.GetDataLen());

		//Search step 2: From end to to current offset.
		if( options & SEARCH_WRAPAROUND and not ( options & SEARCH_FINDALL ) ){
			current_offset = findfile->Length();
			search_step = parent->FileLength() < BlockSz ? parent->FileLength() : BlockSz ;
			do{
				if( findfile->IsProcess() ){
					search_step=BlockSz;
					if( SkipRAM( current_offset, target.GetDataLen(), search_step, true ) ) //SkipRAM adjust current_offset and search_step
						break;
					}
				backward_offset = current_offset - search_step < from ? from : current_offset-search_step;
				search_step = backward_offset + search_step > current_offset ? current_offset-backward_offset : search_step;
				#ifdef _DEBUG_
					std::cout << "FindBinary() BACKWARD2 " << backward_offset << "-" << current_offset << " \t " << "SearchStep:" << search_step<< std::endl;
				#endif
				findfile->Seek( backward_offset, wxFromStart );
				readed=findfile->Read( buffer , search_step ); //Up TO FROM!!!
				read_speed += readed;
				found = SearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options );//Makes raw search here

				if(found >= 0){
					delete [] buffer;
					return backward_offset+found;
					}
				else{
					current_offset = backward_offset + target.GetDataLen(); //Unprocessed bytes
					}

				//Gauge update
				time(&te);
				if(ts != te ){
						ts=te;
						emsg = msg + wxT("\n") + wxString::Format(_("Search Speed : %.2f MB/s"), 1.0*read_speed/MB) + wxT("\n");
						read_speed=0;
						}
				totalread += readed;
				if( findfile->IsProcess() )
					percentage = (processfootprint - totalread)*1000 / processfootprint;
				else
					percentage = (current_offset-from)*1000/parent->FileLength();
				if( ! progress_gauge.Update(percentage, emsg))		// update progress and break on abort
					break;

				}while( current_offset >= from );
			}
		}
	else{
		if( options & SEARCH_FINDALL )
			current_offset=0;
		//Search step 1: From cursor to file end.
		do{
			//Skipping unmapped regions if ProcessRAM operation
			if( findfile->IsProcess() ){
				search_step=BlockSz;
				//if( SkipRAM( current_offset, target.GetDataLen(), search_step ) ) //SkipRAM adjust current_offset and search_step
				//	break;
				uint64_t map_start,map_end;
				if( not parent->ProcessRAM_FindMap( current_offset, map_start, map_end ) ){
					#ifdef _DEBUG_
					printf( "Skipping from: 0x%llX to", current_offset) ;
					#endif
					current_offset = parent->ProcessRAM_FindNextMap(current_offset);
					#ifdef _DEBUG_
					printf( "0x%llX\n", current_offset) ;
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

			findfile->Seek( current_offset, wxFromStart );
			readed = findfile->Read( buffer , search_step );
			read_speed += readed;

			#ifdef _DEBUG_
				std::cout << "Readed: " << readed << std::endl;
				printf( "FindBinary() FORWARD1 0x%llX - 0x%llX\n" , current_offset , current_offset+search_step);
			#endif
			found = SearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options );//Makes raw search here

			if(found >= 0){//We found something
				if( options & SEARCH_FINDALL ){
					TagElement *mytag=new TagElement(current_offset+found, current_offset+found+target.GetDataLen()-1,wxEmptyString,*wxBLACK, wxColour(255,255,0,0) );
					parent->HighlightArray.Add(mytag);
					current_offset += found+target.GetDataLen(); //Unprocessed bytes
					readed=search_step; //to stay in loop
					}
				else{
					delete [] buffer;
					return current_offset+found;
					}
				}
			else{
				int z = readed - target.GetDataLen() -1;
				current_offset += (z <= 0 ? 1 : z); //Unprocessed bytes
				}

			time(&te);
			if(ts != te ){
					ts=te;
					emsg = msg + wxT("\n") + wxString::Format(_("Search Speed : %.2f MB/s"), 1.0*read_speed/MB) + wxT("\n");
					read_speed=0;
					}

			totalread += readed; // We need this on step 2
			if( findfile->IsProcess() ){
				#ifdef _DEBUG_
				printf( "ProcessRAM Virtual Offset: 0x%llX, \t Offset:0x%llX,  \tFootPrint: 0x%llX\n", parent->ProcessRAM_GetVirtualOffset( current_offset ),
								current_offset, processfootprint);
				#endif
				percentage = parent->ProcessRAM_GetVirtualOffset( current_offset )*1000/processfootprint;
				}

			else
				percentage = ( options & SEARCH_WRAPAROUND and not ( options & SEARCH_FINDALL ))
													? (current_offset-from)*1000/(parent->FileLength())
													: (current_offset-from)*1000/(parent->FileLength()-from+1);//+1 to avoid error

			if( ! progress_gauge.Update( percentage, emsg))		// update progress and break on abort
				break;

			}while(readed == search_step); //indicate also file end.

		//Search step 2: From start to file end.
		if( options & SEARCH_WRAPAROUND and not ( options & SEARCH_FINDALL ) ){
			current_offset = 0;
			search_step = parent->FileLength() < BlockSz ? parent->FileLength() : BlockSz ;
			#ifdef _DEBUG_
				std::cout << "FindBinary() FORWARD2 " << current_offset << "-" << current_offset+search_step<< std::endl;
			#endif
			do{
				if( findfile->IsProcess() ){
					search_step=BlockSz;
					//if( SkipRAM( current_offset, target.GetDataLen(), search_step ) )
					//	break;
					}
				findfile->Seek(current_offset, wxFromStart );
				readed = findfile->Read( buffer , search_step );
				read_speed += readed;
				if( readed + current_offset > from )
					search_step = readed + current_offset - from - 1;
				found = SearchAtBuffer( buffer, readed, static_cast<char*>(target.GetData()),target.GetDataLen(), options );//Makes raw search here
				if(found >= 0){
					delete [] buffer;
					return current_offset+found;
					}
				else{
					int z= readed - target.GetDataLen() -1;
					current_offset += (z <= 0 ? 1 : z); //Unprocessed bytes
					}

				time(&te);
				if(ts != te ){
						ts=te;
						emsg = msg + wxT("\n") + wxString::Format(_("Search Speed : %.2f MB/s"), 1.0*read_speed/MB) + wxT("\n");
						read_speed=0;
						}

				totalread += readed;
				if( findfile->IsProcess() )
					percentage = totalread*1000 / processfootprint;
				else
					percentage = (current_offset+parent->FileLength()-from)*1000/parent->FileLength();

				if( ! progress_gauge.Update( percentage, emsg))	// update progress and break on abort
					break;

				}while(current_offset + readed < from); //Search until cursor
			}
		}
	delete [] buffer;
	return NANINT;
	}

void FindDialog::OnFindAll( bool internal ){
	parent->HighlightArray.Clear();

	unsigned options = SEARCH_FINDALL; //fill continue search until file and with this option.
	options |= m_searchtype->GetSelection() == 0 ? SEARCH_TEXT : SEARCH_HEX;
	options |= chkWrapAround->GetValue() ? SEARCH_WRAPAROUND : 0;
	options |= chkSearchBackwards->GetValue() ? SEARCH_BACKWARDS : 0;

	if(options & SEARCH_TEXT) {
		PrepareComboBox( true );
		options |= chkUTF8->GetValue() ? SEARCH_UTF8 : 0;
		options |= chkMatchCase->GetValue() ? SEARCH_MATCHCASE : 0;

		if( options & SEARCH_WRAPAROUND )
			FindText( m_comboBoxSearch->GetValue(), 0, options );
		else if( options & SEARCH_BACKWARDS )
			FindText( m_comboBoxSearch->GetValue(), parent->CursorOffset() , options );
		else
			FindText( m_comboBoxSearch->GetValue(), parent->CursorOffset()+1 , options );
		}

	else {
		wxString hexval = m_comboBoxSearch->GetValue();
		//parent->Goto(0);
		if(not HexVerifyAndPrepare( hexval, _("Search"), this ) )
            return;
		m_comboBoxSearch->SetValue(hexval.Upper());
		PrepareComboBox( true );
		FindBinary( wxHexCtrl::HexToBin( m_comboBoxSearch->GetValue()), 0 ,options );
		}

	if(parent->HighlightArray.GetCount()==0 and not internal) {
		OSXwxMessageBox(_("Search value not found"), _("Nothing found!"), wxOK, this );
		}
	else {
		//Is selection needed to show first tag?
		parent->Reload(); //To highlighting current screen
		parent->UpdateCursorLocation( parent->HighlightArray.Item(0)->start );

		wxUpdateUIEvent eventx( SEARCH_CHANGE_EVENT );
		parent->GetEventHandler()->ProcessEvent( eventx );
		if( not internal ){
			this->Hide();
			OSXwxMessageBox(wxString::Format(_("Found %d matches."),parent->HighlightArray.GetCount()), _("Find All Done!"), wxOK, parent );
			Destroy();
			}
		}
	}

//This function will slow down searching process due overhead of OpenMP
inline int FindDialog::SearchAtBufferMultiThread( char *bfr, int bfr_size, char* search, int search_size, unsigned options ){
	//return SearchAtBufferSingleThread( bfr, bfr_size, search, search_size, options );
	return SearchAtBuffer( bfr, bfr_size, search, search_size, options );

///DualThread
//	int f1,f2;
//	#pragma omp parallel sections // starts a new team
//	{
//		#pragma omp section
//		{f1=SearchAtBuffer( bfr, bfr_size/2, search, search_size, options );}
//		#pragma omp section
//		{f2=SearchAtBuffer( bfr+ bfr_size-bfr_size/2-search_size+1, bfr_size/2+search_size-1, search, search_size, options );}
//	}
//	return f1 >= 0 ? f1 : bfr_size/2+f2;

///QuadThread
//	int ztep=bfr_size/4;
//	int f1,f2,f3,f4;
//
//	#ifndef OMP_H
//	return SearchAtBufferSingleThread( bfr, bfr_size, search, search_size, options );
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
	}

// TODO (death#9#): Implement better search algorithm instead of 1:1 comparison + (using OpenCL, OpenMP, SIMD) :)
//WARNING! THIS FUNCTION WILL CHANGE BFR and/or SEARCH strings if SEARCH_MATCHCASE not selected as an option!
inline int FindDialog::SearchAtBuffer( char *bfr, int bfr_size, char* search, int search_size, unsigned options ){	// Dummy search algorithm\ Yes yes I know there are better ones but I think this enought for now.
	if( bfr_size < search_size )
		return -1;

	///SEARCH_FINDALL operation supersedes SEARCH_BACKWARDS

	//UTF with no matched case handled here !!!SLOW!!!
	if(options & SEARCH_UTF8 and options & SEARCH_TEXT and not (options & SEARCH_MATCHCASE) ){
		//!!!DIRTY HACK HANDLING HERE
		/// Our "search" is now in "searchSEARCH" format.
		/// Compare code twice and viola!

		// TODO (death#1#): Have doubts if thish lead Segmentation fault or works OK every time? Need unit test for this fx
		//For UTF2, search_size must be > 2. Than, we are in the safe! :)
		if(1){
			wxString ucode;
			wxCharBuffer ubuf;
			if( options & SEARCH_BACKWARDS and not (options & SEARCH_FINDALL)){ //Backward Search!
				for(int i=bfr_size - search_size ; i >= 0 ; i-- ){
					if( ( bfr[i]==search[0] and bfr[i+1]==search[1] ) or
						 ( bfr[i]==UTF8SpeedHack[0] and bfr[i+1]==UTF8SpeedHack[1] )  // Safe due UTF8??
						){//if match found
						ucode = wxString::FromUTF8(bfr+i, search_size);
						ubuf = ucode.Lower().ToUTF8();
						if(! memcmp( ubuf, search, search_size))	//if match found
							return i;
						}
					}
				}
			else
				for(int i=0 ; i <= bfr_size - search_size ; i++ ){
					if( ( bfr[i]==search[0] and bfr[i+1]==search[1] ) or
						 ( bfr[i]==UTF8SpeedHack[0] and bfr[i+1]==UTF8SpeedHack[1] )  // Safe due UTF8??
						){//if match found
						ucode = wxString::FromUTF8(bfr+i, search_size);
						ubuf = ucode.Lower().ToUTF8();
						if(! memcmp( ubuf, search, search_size))	//if match found
							return i;
						}
					}
			}
		else{//Old and deadly slow code
			wxString ucode;
			wxCharBuffer ubuf;
			if(( options & SEARCH_BACKWARDS) and not (options & SEARCH_FINDALL)) //Backward Search!
				for(int i=bfr_size - search_size ; i >= 0 ; i-- ){
					ucode = wxString::FromUTF8(bfr+i, search_size);
					ubuf = ucode.Lower().ToUTF8();
					if(! memcmp( ubuf, search, search_size ))	//if match found
						return i;
					}
			else //Normal Operation
				for(int i=0 ; i <= bfr_size - search_size ; i++ ){
					ucode = wxString::FromUTF8(bfr+i, search_size);
					ubuf = ucode.Lower().ToUTF8();
					if(! memcmp( ubuf, search, search_size ))	//if match found
						return i;
					}
			}
		return -1;
		}


	else if(options & SEARCH_TEXT and not (options & SEARCH_MATCHCASE) ){
		if(1){//Speedy code
			//Search at no match case ASCII handled here
			///Search text already lowered at FindText()
//			char topSearch[search_size];
//			for( int i = 0 ; i < search_size; i++)
//				topSearch[i]=tolower(search[i]);
			if( options & SEARCH_BACKWARDS and not (options & SEARCH_FINDALL)) //Backward Search!
				for( int i=bfr_size - search_size ; i >= 0 ; i-- ){
					if(( bfr[i] == search[0] or bfr[i] == UTF8SpeedHack[0] )
						//and ( bfr[i+1] == search[1] or bfr[i+1] == UTF8SpeedHack[1] ) //You cant know if search_size >= 2
						){
						//partial lowering code
						for( int j = i ; (j < bfr_size) and (j-i<search_size); j++)
							bfr[j]=tolower(bfr[j]);
						if(! memcmp( bfr+i, search, search_size ))	//if match found
							return i;
						}
					}
			else //Normal Operation
				for(int i=0 ; i <= bfr_size - search_size ; i++ ){
					if(( bfr[i] == search[0] or bfr[i] == UTF8SpeedHack[0] )
						//and ( bfr[i+1] == search[1] or bfr[i+1] == UTF8SpeedHack[1] ) //You cant know if search_size >= 2
						){
						//partial lowering code
						for( int j = i ; (j < bfr_size) and (j-i<search_size); j++)
							bfr[j]=tolower(bfr[j]);
						if(! memcmp( bfr+i, search, search_size ))	//if match found
							return i;
						}
					}
			return -1;
			}
		else{	//Make buffer lower for comparing with standard comparison loop.
			///Search text already lowered at FindText()
//			for( int i = 0 ; i < search_size; i++)
//				search[i]=tolower(search[i]);

			//Make buffer low to match
			//#pragma omp parallel for schedule(static)// num_threads(4)
			for( int i = 0 ; i < bfr_size; i++)
				bfr[i]=tolower(bfr[i]);
			}
		}

	//Search at buffer
	if( options & SEARCH_BACKWARDS and not (options & SEARCH_FINDALL) ){ //Backward Search!
		for(int i=bfr_size - search_size ; i >= 0 ; i-- )
			if( bfr[i] == search[0] )
				if(! memcmp( bfr+i, search, search_size ))	//if match found
					return i;
		}
	else{
		//#pragma omp parallel for schedule(static)
		for(int i=0 ; i <= bfr_size - search_size ; i++ )
			if( bfr[i] == search[0] )
				if(! memcmp( bfr+i, search, search_size ))	//if match found
					return i;
		}
	return -1;
	}

ReplaceDialog::ReplaceDialog( wxWindow* parent, FAL *find_file, wxString title ):FindDialog( parent, find_file, title ){
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

int ReplaceDialog::OnReplace( bool internal ){
	if( not parent->select->GetState() ) {
		if( OnFind( internal ) == false )
			return 0;
		else
			return -1;
		}

	else{
		if( m_searchtype->GetSelection() == 0 ){//text search
			PrepareComboBox( true );
			if( parent->select->GetSize() == m_comboBoxReplace->GetValue().Len() ){
				parent->FileAddDiff( parent->CursorOffset(), m_comboBoxReplace->GetValue().ToAscii(), m_comboBoxReplace->GetValue().Len());
				parent->select->SetState( false );
				parent->Reload();
				wxUpdateUIEvent eventx( UNREDO_EVENT );
				parent->GetEventHandler()->ProcessEvent( eventx );
				return 1;
				}
			else{
				OSXwxMessageBox(_("Search and Replace sizes are not equal!\nReplacing with different size are not supported yet."), _("Error!"), wxOK, this);
				return 0;
				}
			}
		else{ //hex search
			//Hex Validation and Format
			wxString hexval = m_comboBoxReplace->GetValue();

			if( not HexVerifyAndPrepare( hexval, _("Replace"), this ) )
                return 0;

			m_comboBoxReplace->SetValue(hexval.Upper());

			PrepareComboBox( true );

			wxMemoryBuffer search_binary = wxHexCtrl::HexToBin( m_comboBoxReplace->GetValue());
			if( parent->select->GetSize() == search_binary.GetDataLen() ){
				parent->FileAddDiff( parent->CursorOffset(), static_cast<char*>(search_binary.GetData()) ,search_binary.GetDataLen() );
				parent->select->SetState( false );
				parent->Reload();
				wxUpdateUIEvent eventx( UNREDO_EVENT );
				parent->GetEventHandler()->ProcessEvent( eventx );
				return 1;
				}
			else{
				OSXwxMessageBox(_("Search and Replace sizes are not equal!\nReplacing with different size are not supported yet."), _("Error!"), wxOK, this);
				return 0;
				}
			}
		}
	}

void ReplaceDialog::OnReplaceAll( void ){
	//First search all file with find all to detect locations.
	OnFindAll( true );
	//Now Highlight array has matches. We could replace them with replace string.
	for( uint32_t i=0 ; i < parent->HighlightArray.Count() ; i++ ){
		if( m_searchtype->GetSelection() == 0 ){ //text search
				PrepareComboBox( true );
				parent->FileAddDiff( parent->HighlightArray.Item(i)->start,
											m_comboBoxReplace->GetValue().ToAscii(),
											m_comboBoxReplace->GetValue().Len());
			}
		else{ //hex search
			//Hex Validation and Format
			wxString hexval = m_comboBoxReplace->GetValue();
			if(not HexVerifyAndPrepare( hexval, _("Replace"), this ) )
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

	if( parent->HighlightArray.Count() > 0){
		parent->Goto( parent->HighlightArray.Item(0)->start );
		parent->Refresh();
		this->Hide();
		wxUpdateUIEvent eventx( UNREDO_EVENT );
		parent->GetEventHandler()->ProcessEvent( eventx );
		OSXwxMessageBox(wxString::Format(_("%d records changed."), parent->HighlightArray.Count() ), _("Info"), wxOK, parent);
		Destroy();
		}
	}

void ReplaceDialog::EventHandler( wxCommandEvent& event ){
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

void ReplaceDialog::PrepareComboBox( bool AddString ){
	int searchType = m_searchtype->GetSelection() == 0 ? SEARCH_TEXT : SEARCH_HEX;
	if( searchType == SEARCH_TEXT )
		ComboBoxFill( _T("ReplaceTextString"), m_comboBoxReplace, AddString);
	else
		ComboBoxFill( _T("ReplaceHexString"), m_comboBoxReplace, AddString);
	}

CopyAsDialog::CopyAsDialog( wxWindow* _parent, FAL *file, Select *select_ , ArrayOfTAG* MainTagArray_):CopyAsDialogGui( _parent , wxID_ANY){
	parent = static_cast< HexEditor* >( _parent );
	spnBytePerLine->SetValue( parent->BytePerLine() );
	select = select_;
	copy_file = file;
	MainTagArray=MainTagArray_;
	bool IsBigEndian;
	//parent->interpreter->chkBigEndian->GetValue(); Does this approach better than remembering old value?
	wxConfigBase::Get()->Read( _T("CopyAsBigEndian"), &IsBigEndian );
	chkBigEndian->SetValue( IsBigEndian );

	int CopyAsFunction;
	if (wxConfigBase::Get()->Read(_T("CopyAsSelectedFunction"), &CopyAsFunction))
		chcCopyAs->SetSelection( CopyAsFunction );
	PrepareOptions( CopyAsFunction );
	}

void CopyAsDialog::PrepareOptions( int SelectedFunction ){
//wxT("Full Text"), wxT("Special Hex"), wxT("HTML"), wxT("C/C++"), wxT("Assembler")
	chcOption->Enable(SelectedFunction > 0);
	chcOption->Clear();

	if( SelectedFunction == 0){
		chcOption->Insert(_("Not Available"),0);
		chkOffset->Enable(true);
		chkHex->Enable(true);
		chkText->Enable(true);
		}
	else if( SelectedFunction == 1){ // Hex
		chcOption->Insert(_("Raw Hex"),0);
		chcOption->Insert(_("With Space"),1);
		chcOption->Insert(_("Quad Hex"),2);
		chcOption->Insert(_("with 0x"),3);
		chcOption->Insert(_("with 0x and period"),4);
		chkOffset->Enable(false);
		chkHex->Enable(false);
		chkText->Enable(false);

		}
	else if( SelectedFunction == 2){ // HTML
		chcOption->Insert(_("HTML format"),0);
		chcOption->Insert(_("HTML with TAGs"),1);
		chcOption->Insert(_("phpBB forum style"),2);
		chcOption->Insert(_("WiKi format"),3);
		chcOption->Insert(_("WiKi with TAGs"),4);
		chkOffset->Enable(true);
		chkHex->Enable(true);
		chkText->Enable(true);
		}
	else if( SelectedFunction >= 3){ // C/C++/ASM Sources
		chcOption->Insert(_("8bit - Byte "),0);
		chcOption->Insert(_("16bit - Words"),1);
		chcOption->Insert(_("32bit - Dwords"),2);
		chcOption->Insert(_("64bit - Qwords"),3);
		chkOffset->Enable(false);
		chkHex->Enable(false);
		chkText->Enable(false);
		}
	int option=0;
	wxConfigBase::Get()->Read(_T("CopyAsSelectedOptionOfFunction") + wxString::Format(wxT("%d"),SelectedFunction), &option);
	chcOption->SetSelection( option );

	//Enable big endian checkbox if multi byte representation selected for C/C++/ASM sources.
	chkBigEndian->Enable( chcCopyAs->GetSelection() >=3 and chcOption->GetSelection() > 0 );

	wxYield();
	this->GetSizer()->Fit(this);
	this->GetSizer()->Layout();
	}

void CopyAsDialog::EventHandler( wxCommandEvent& event ){
	int id = event.GetId();
	if( id == wxID_CANCEL )
		Destroy();
	else if( id == wxID_OK ){
		Copy();
		Destroy();
		}
	else if( id == chcCopyAs->GetId() ){
		int SelectedFunction =  chcCopyAs->GetSelection();
		PrepareOptions(SelectedFunction);

		int option;
		//Adjustinf selection part
		if (wxConfigBase::Get()->Read(_T("CopyAsSelectedOptionOfFunction") + wxString::Format(wxT("%d"),SelectedFunction), &option))
			chcOption->SetSelection( option );
		else
			chcOption->SetSelection(0);

		wxConfigBase::Get()->Write( _T("CopyAsSelectedFunction"), SelectedFunction );
		}
	else if( id == chcOption->GetId() ){
		wxConfigBase::Get()->Write( _T("CopyAsSelectedOptionOfFunction") + wxString::Format(wxT("%d"), chcCopyAs->GetSelection()), chcOption->GetSelection() );

		//Enable big endian checkbox if multi byte representation selected for C/C++/ASM sources.
		chkBigEndian->Enable( chcCopyAs->GetSelection() >=3 and chcOption->GetSelection() > 0 );
		}
	else if( id == chkBigEndian->GetId() ){
		wxConfigBase::Get()->Write( _T("CopyAsBigEndian"), chkBigEndian->GetValue() );
		}

	}

void CopyAsDialog::PrepareFullText( wxString& cb, wxMemoryBuffer& buff ){
	unsigned BytePerLine = spnBytePerLine->GetValue();
	for(unsigned current_offset = 0; current_offset < select->GetSize() ; current_offset += BytePerLine){
		if(chkOffset->GetValue()){
			cb += parent->GetFormatedOffsetString( select->GetStart() + current_offset );
			cb += wxT("   ");
			}

		//Add 16 hex val
		if(chkHex->GetValue()){
			for(unsigned i = 0 ; i < BytePerLine ; i++){
				if( i + current_offset < select->GetSize())
					cb+= wxString::Format( wxT("%02X "), (unsigned char)buff[ current_offset + i] );
				else
					cb+= wxT("   "); //fill with zero to make text area at proper location
				}
			cb += wxT("  "); //Why only 2 ? Because we got extra 1 space from Hex
			}

		if(chkText->GetValue()){
		//Add 16 Ascii rep
			unsigned char chr;
			for(unsigned i = 0 ; i < BytePerLine ; i++){
				if( i + current_offset < select->GetSize()){
					//Char filter for ascii
					chr = buff[ current_offset + i];
					if( (chr !=173) && ( (chr>31 && chr<127) || chr>159) )
						cb+= wxString::FromAscii( buff[ current_offset + i] );
					else
						cb+= wxString::FromAscii( '.' );
					}
				}
			}
		cb += wxNewline;
		}
	}

void CopyAsDialog::PrepareFullTextWithTAGs( wxString& cb, wxMemoryBuffer& buff, wxString startup=wxEmptyString ){
	unsigned BytePerLine = spnBytePerLine->GetValue();
	wxString last_color_hex,last_color_text;
	cb += startup+wxT("TAG List:")+wxNewline;
	for( unsigned i =0 ; i < MainTagArray->Count() ; i++ ){
		TagElement *tag = MainTagArray->Item(i);
		if(( tag->start <  select->GetStart() and tag->end   >= select->GetStart() ) or
			( tag->start >= select->GetStart() and tag->start <= select->GetEnd() ) or
			( tag->end   >= select->GetStart() and tag->end   <= select->GetEnd() ) ){

			cb += startup+wxT("<span style=\"background-color:") + tag->SoftColour( tag->NoteClrData.GetColour() ).GetAsString(wxC2S_HTML_SYNTAX) +
					wxT(";color:") + tag->FontClrData.GetColour().GetAsString(wxC2S_HTML_SYNTAX) +  wxT(";\">") + tag->tag +wxT("</span>")+wxNewline;
			}
		}
	cb += startup+wxNewline;

	for(unsigned current_offset = 0; current_offset < select->GetSize() ; current_offset += BytePerLine){
		if(chkOffset->GetValue()){
			cb += startup + parent->GetFormatedOffsetString( select->GetStart() + current_offset );
			cb += wxT("   ");
			}

		if(chkHex->GetValue()){
			//Add 16 hex val
			//Check for middle TAG selection starts
			if( current_offset == 0 )
				for( unsigned j = 0 ; j< MainTagArray->Count() ; j++ ){
					TagElement *tg = MainTagArray->Item(j);
					if( tg->isCover( select->GetStart() ) )
						last_color_hex = last_color_hex = tg->SoftColour(tg->NoteClrData.GetColour()).GetAsString(wxC2S_HTML_SYNTAX);
				}

			if( last_color_hex.Len() )
				cb += wxT("<span style=\"background-color:") + last_color_hex + wxT(";\">");

			for(unsigned i = 0 ; i < BytePerLine ; i++){

				//TAG Paint Loop
				for( unsigned j = 0 ; j< MainTagArray->Count() ; j++ ){
					TagElement *tg = MainTagArray->Item(j);
					if( MainTagArray->Item(j)->start == i + current_offset + select->GetStart()){
						last_color_hex = tg->SoftColour(tg->NoteClrData.GetColour()).GetAsString(wxC2S_HTML_SYNTAX);
						cb += wxT("<span style=\"background-color:") + last_color_hex + wxT(";\">");
						}
					if( MainTagArray->Item(j)->end +1== i + current_offset + select->GetStart() ){
						cb += wxT("</span>");
						last_color_hex = wxEmptyString;
						}
					}

				if( i + current_offset < select->GetSize())
					cb+= wxString::Format( wxT("%02X "), (unsigned char)buff[ current_offset + i] );
				else{
					if(last_color_hex.Len() )
						cb += wxT("</span>");
					last_color_hex = wxEmptyString;
					cb+= wxT("   "); //fill with zero to make text area at proper location
					}
				//This avoid to paint text section.
				if(last_color_hex.Len() and i==BytePerLine-1)
					cb += wxT("</span>");
				}
		cb += wxT("  "); //Why only 2 ? Because we got extra 1 space from Hex
		}

		if(chkText->GetValue()){
			//Add 16 Ascii rep
			unsigned char chr;

			if( current_offset == 0 )
				for( unsigned j = 0 ; j< MainTagArray->Count() ; j++ ){
					TagElement *tg = MainTagArray->Item(j);
					if( tg->isCover( select->GetStart() ) )
						last_color_text = tg->SoftColour(tg->NoteClrData.GetColour()).GetAsString(wxC2S_HTML_SYNTAX);
				}

			if( last_color_text.Len() )
				cb += wxT("<span style=\"background-color:") + last_color_text + wxT(";\">");
			for(unsigned i = 0 ; i < BytePerLine ; i++){
				if( i + current_offset < select->GetSize()){

								//TAG Paint Loop
				for( unsigned j = 0 ; j< MainTagArray->Count() ; j++ ){
					TagElement *tg = MainTagArray->Item(j);
					if( MainTagArray->Item(j)->start == i + current_offset + select->GetStart()){
						last_color_text = tg->SoftColour( tg->NoteClrData.GetColour()).GetAsString(wxC2S_HTML_SYNTAX);
						cb += wxT("<span style=\"background-color:") + last_color_text + wxT(";\">");
						}
					if( MainTagArray->Item(j)->end +1== i + current_offset + select->GetStart()){
						cb += wxT("</span>");
						last_color_text = wxEmptyString;
						}
					}

					//Char filter for ascii
					chr = buff[ current_offset + i];
					if( (chr !=173) && ( (chr>31 && chr<127) || chr>159) )
						cb+= wxString::FromAscii( buff[ current_offset + i] );
					else
						cb+= wxString::FromAscii( '.' );
					}
				if(last_color_text.Len() and i==BytePerLine-1)
					cb += wxT("</span>");
				}
			}
		cb += wxNewline;
		}
	cb += startup + wxNewline;
}

void CopyAsDialog::Copy( void ){
	chcOption->GetSelection();
	if( select->GetState() ) {
		int BytePerLine = spnBytePerLine->GetValue();
		wxString cb;
		uint64_t RAM_limit = 10*MB;
		wxMemoryBuffer buff;
		if(select->GetSize() > RAM_limit) {
			OSXwxMessageBox( _("Selected block bigger than limit." ),_("Error!"), wxOK|wxICON_ERROR , this);
			return;
			}

		copy_file->Seek( select->GetStart(), wxFromStart );
		buff.UngetWriteBuf( copy_file->Read( static_cast<unsigned char*>(buff.GetWriteBuf(select->GetSize())), select->GetSize() ) );

		if( chcCopyAs->GetSelection() == 0 ){ //Full text copy
			PrepareFullText( cb, buff );
			}
		else if( chcCopyAs->GetSelection() == 1){
			wxString HexFormat;
			bool quad=false;
			switch( chcOption->GetSelection()){
				case 0: HexFormat=wxT("%02X");break; //Raw Hex
				case 1: HexFormat=wxT("%02X ");break; //Standard
				case 2: HexFormat=wxT("%02X"); quad=true; break; //Quad
				case 3: HexFormat=wxT("0x%02X ");break; //Ox
				case 4: HexFormat=wxT("0x%02X, ");break; //Ox with period
				}

			for(unsigned current_offset = 0; current_offset < select->GetSize() ; current_offset ++){
				cb+= wxString::Format( HexFormat, (unsigned char)buff[ current_offset ] );
				if( quad and ((current_offset+1)%2)==0)
					cb += wxT(" ");
				if(( (current_offset+1) % BytePerLine)==0 )
					cb += wxNewline;
				}
			}

		else if( chcCopyAs->GetSelection() == 2){//Internet
			if( chcOption->GetSelection() == 0 ){ //html
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

			else if( chcOption->GetSelection() == 1 ){ //HTML with Tags
				cb += wxT("<pre><code style=\"color:#000000;\">");
				PrepareFullTextWithTAGs( cb, buff );
				cb += wxT("</code></pre><font size=\"-3\">Generated by <a href=\"http://www.wxHexEditor.org\">wxHexEditor</a></font>");
				}

			else if( chcOption->GetSelection() == 2 ){ //phpBB Forum
				cb+= wxT("[code]");
				PrepareFullText( cb, buff );
				cb += wxT("[/code]Generated by [url=http://www.wxHexEditor.org]wxHexEditor[/url]" wxNewline);
				}

			else if( chcOption->GetSelection() == 3 ){ //WiKi format
				cb+= wxT("<pre>");
				PrepareFullText( cb, buff );
				cb += wxT("</pre> Generated by [http://www.wxHexEditor.org/ wxHexEditor]" wxNewline);
				}
			else if( chcOption->GetSelection() == 4 ){ //WiKi with TAGs
				PrepareFullTextWithTAGs( cb, buff, wxT(" ") );
				cb += wxT(" Generated by [http://www.wxHexEditor.org/ wxHexEditor]" wxNewline);
				}
			}
		else if( chcCopyAs->GetSelection() == 3){//C/C++
			unsigned HexSize = pow( 2, chcOption->GetSelection());
			unsigned count = select->GetSize()/HexSize;
			cb+=wxT("// Generated by wxHexEditor //" wxNewline);
			switch( HexSize ){
				case 1: cb += wxT("int8_t"); break;
				case 2: cb += wxT("int16_t"); break;
				case 4: cb += wxT("int32_t"); break;
				case 8: cb += wxT("int64_t"); break;
				}

			cb+=wxString::Format( wxT(" hexData[0x%x] = {" wxNewline "  "), count );
			bool bigEndianSwapReq = chkBigEndian->GetValue() and chcOption->GetSelection();//No big endian for 8 bit
			int b;
			int limit=(bigEndianSwapReq ? -1 : HexSize);
			int incr = (bigEndianSwapReq ? -1 : +1);
			for(unsigned current_offset = 0; current_offset < count ; current_offset ++){
				b = (bigEndianSwapReq ? HexSize-1 : 0);
				cb+= wxT("0x");
				for(; b not_eq limit ; b+=incr)
					cb+= wxString::Format( wxT("%02X"), reinterpret_cast<unsigned char*>( buff.GetData() )[current_offset*HexSize+b]);
				cb+= wxT(", ");
//				switch( HexSize ){
//					case 1: cb+= wxString::Format( wxT("0x%02X, "), *reinterpret_cast<unsigned char*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 2: cb+= wxString::Format( wxT("0x%04X, "), *reinterpret_cast<unsigned short*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 4: cb+= wxString::Format( wxT("0x%08X, "), *reinterpret_cast<unsigned int*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 8: cb+= wxString::Format( wxT("0x%016"wxLongLongFmtSpec"X, "), *reinterpret_cast<uint64_t*>( buff.GetData()+current_offset*HexSize )); break;
//					}
				if(( (current_offset+1) % (BytePerLine/HexSize)==0 ) and current_offset not_eq count)
						cb += wxT("" wxNewline "  ");
				}
			cb=cb.BeforeLast(',')+wxT(" }" wxNewline);
		   }
		else if( chcCopyAs->GetSelection() == 4){//ASM
			unsigned HexSize = pow( 2, chcOption->GetSelection());
			unsigned count = select->GetSize()/HexSize;
			cb+=wxT(";Generated by wxHexEditor");

			wxString HexFormat;
			switch( HexSize ){
				case 1: HexFormat += wxT("" wxNewline "db "); break;
				case 2: HexFormat += wxT("" wxNewline "dw "); break;
				case 4: HexFormat += wxT("" wxNewline "dd "); break;
				case 8: HexFormat += wxT("" wxNewline "dq "); break;
				}

			bool bigEndianSwapReq = chkBigEndian->GetValue() and chcOption->GetSelection();//No big endian for 8 bit
			int b;
			int limit=(bigEndianSwapReq ? -1 : HexSize);
			int incr = (bigEndianSwapReq ? -1 : +1);
			for(unsigned current_offset = 0; current_offset < count ; current_offset ++){
				if( current_offset % (BytePerLine/HexSize)==0 )
					cb += HexFormat;
				b = (bigEndianSwapReq ? HexSize-1 : 0);
				cb+= wxT("0");
				for(; b not_eq limit ; b+=incr)
					cb+= wxString::Format( wxT("%02X"), reinterpret_cast<unsigned char*>( buff.GetData() )[current_offset*HexSize+b]);
				cb+= wxT("h ");
//				switch( HexSize ){
//					case 1: cb+= wxString::Format( wxT("0%02Xh "), *reinterpret_cast<unsigned char*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 2: cb+= wxString::Format( wxT("0%04Xh "), *reinterpret_cast<unsigned short*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 4: cb+= wxString::Format( wxT("0%08Xh "), *reinterpret_cast<unsigned int*>(	buff.GetData()+current_offset*HexSize )); break;
//					case 8: cb+= wxString::Format( wxT("0%016"wxLongLongFmtSpec"Xh "), *reinterpret_cast<uint64_t*>( buff.GetData()+current_offset*HexSize )); break;
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

CompareDialog::CompareDialog( wxWindow* parent_, wxString File1, wxString File2 ):CompareDialogGui(parent_, wxID_ANY){
	parent = static_cast< HexEditorFrame* >(parent_);
	filePick1->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(CompareDialog::EventHandler2),NULL, this);
	filePick2->Connect(wxEVT_DROP_FILES, wxDropFilesEventHandler(CompareDialog::EventHandler2),NULL, this);

	if( File1 != wxEmptyString ){
		filePick1->SetPath( File1 );
		filePick2->SetPath( File2 );
		filePick1->Hide();
		filePick2->Hide();
		m_TextFile1->Hide();
		m_TextFile2->Hide();
		btnCompare->Enable(true);
		Fit();
		}

	int options=0;
	wxConfigBase::Get()->Read( _T("CompareOptions"), &options );

	checkMergeSection->SetValue( options & OPT_CMP_MERGE_SECTION );
	spinMergeSection->Enable( options & OPT_CMP_MERGE_SECTION );
	checkStopCompare->SetValue( options & OPT_CMP_STOP_AFTER );
	spinStopCompare->Enable( options & OPT_CMP_STOP_AFTER );
	checkSaveResults->SetValue( options & OPT_CMP_SAVE );
	filePickSave->Enable( options & OPT_CMP_SAVE );
	m_radioDifferent->SetValue( options & OPT_CMP_SEARCH_DIFF );
	m_radioSame->SetValue( not (options & OPT_CMP_SEARCH_DIFF) );

	int tmp;
	wxConfigBase::Get()->Read( _T("CompareOptionStopAfter"), &tmp );
	spinStopCompare->SetValue(tmp);

	wxConfigBase::Get()->Read( _T("CompareOptionMergeSection"), &tmp );
	spinMergeSection->SetValue(tmp);
	}

CompareDialog::~CompareDialog(void){
	filePick1->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler(CompareDialog::EventHandler2),NULL, this);
	filePick2->Disconnect( wxEVT_DROP_FILES, wxDropFilesEventHandler(CompareDialog::EventHandler2),NULL, this);
	}


bool CompareDialog::Compare( wxFileName fl1, wxFileName fl2, bool SearchForDiff, int StopAfterNMatch, wxFileName flsave ){
	if(not fl1.IsFileReadable()){
		wxMessageBox( _("File #1 is not readable."), _("Error") );
		return false;
		}
	if(not fl2.IsFileReadable() ){
		wxMessageBox( _("File #2 is not readable."), _("Error") );
		return false;
		}
//	if( flsave not_eq wxEmptyString ){
//		if(not flsave.IsFileWritable() )
//			wxMessageBox( _("Error, Save File is not writeable.") );
//			return false;
//		}

	wxFFile f1,f2,fs;
// TODO (death#1#): Why we dont use FAL at ALL?
	if( not f1.Open( fl1.GetFullPath(), wxT("rb") ) ){
		wxMessageBox( _("Error, File #1 cannot open." ) );
		return false;
		}
	if( not f2.Open( fl2.GetFullPath(), wxT("rb") ) ){
		wxMessageBox( _("Error, File #2 cannot open." ) );
		return false;
		}

	if( flsave not_eq wxEmptyString )
		if( not fs.Open( flsave.GetFullPath(), wxT("wb") ) ){
			wxMessageBox( _("Error, Save File cannot open." ) );
			return false;
			}
	wxProgressDialog pdlg( _("Compare Progress"), _("Comparison in progress"), 100, this, wxPD_AUTO_HIDE | wxPD_APP_MODAL|wxPD_CAN_ABORT/*|wxPD_CAN_SKIP*/|wxPD_REMAINING_TIME); //SKIP not ready and Buggy
	pdlg.Show();

	wxMemoryBuffer buff1,buff2;
	uint64_t diffBuff[1*MB]; //up to 500K differences.
	//Structure is : even indices for difference start offset, odd indices for end offsets
	int diffHit = 0;
	bool diff=false;
	uint64_t drange = wxMin( f1.Length() , f2.Length() );
	if(drange == 0)
		drange++; //to avoid Gauge zero div error

	//Section Merge Code
	int compare_range=0;
	if( checkMergeSection->GetValue() )
		compare_range=spinMergeSection->GetValue();

	bool BreakDoubleFor=false;
	for( uint64_t mb = 0 ; not (f1.Eof() or f2.Eof() or BreakDoubleFor) ; mb+=MB){
		buff1.UngetWriteBuf( f1.Read(buff1.GetWriteBuf( MB ),MB) );
		buff2.UngetWriteBuf( f2.Read(buff2.GetWriteBuf( MB ),MB) );
#ifdef _DEBUG_
		std::cout << "Diff Compare Offset: " << mb << std::endl;
#endif
		for( unsigned i = 0 ; i < wxMin( buff1.GetDataLen(), buff2.GetDataLen()); i ++ ){
			if(diffHit >= 500000){
				wxMessageBox( wxString(_("Sorry, this program supports up to 500K differences."))+wxT("\n")+_("Remaining differences not shown."), _("Error on Comparison"));
				BreakDoubleFor = true;
				break;
				}

			if((buff1[i] not_eq buff2[i]) == SearchForDiff){
				if(not diff){//Set difference start
#ifdef _DEBUG_
					std::cout << "Diff Start " << mb+i << " to " ;
#endif
					diff=true;
					if( ( (mb+i)-diffBuff[diffHit-1] ) <= compare_range and
						compare_range > 0 and diffHit>1){
						diffHit--; //re-push old diff ending to stack
						StopAfterNMatch++;//and count 2 difference as 1
						}
					else
						diffBuff[diffHit++]=mb+i;
					}

				//this adds latest diff stream to array if one file ends
				if(f1.Eof() or f2.Eof() )
					if(i+1 == wxMin( buff1.GetDataLen(), buff2.GetDataLen()))
						diffBuff[diffHit++]=mb+i;
				}

			else{//bytes are eq.
				if(diff){//Set difference end
#ifdef _DEBUG_
			std::cout << mb+i-1 << std::endl;
#endif
						diff=false;
						diffBuff[diffHit++]=mb+i-1;
//						}

					//Some says we do not use goto in a good program.
					//But I don't know better way to break double for loop.
					//Might be better to use f1.Seek() to end with break...
					if( --StopAfterNMatch == 0 ){
#ifdef _DEBUG_
						std::cout << "Break comparison due StopAfterNMatch." << std::endl ;
#endif
						BreakDoubleFor=true;
						break;
						}
					}
				}
			}
		bool skip=false;

		if( not pdlg.Update( (mb*100)/drange, wxEmptyString, &skip) ){
			f1.Close();
			f2.Close();
			return false;
			}

		if(skip)
			break;
		}//End of for loops / comparison

	pdlg.Show( false );

	if( flsave not_eq wxEmptyString ){
		wxString ln = _("File #1 : ") + fl1.GetFullPath() + wxT("\n")+_("File #2 : ") + fl2.GetFullPath() + wxT("\n\n");
		fs.Write( ln );
		wxString line;
		for(int i = 0 ; i < diffHit-1 ; i+=2){
			line = wxString::Format( wxT("%s found %"wxLongLongFmtSpec"u - %"wxLongLongFmtSpec"u \t Total : %"wxLongLongFmtSpec"u bytes.\n"), ( SearchForDiff ? wxT("Diff"):wxT("Match")), diffBuff[i] , diffBuff[i+1], diffBuff[i+1]-diffBuff[i]+1 );
			fs.Write( line );
			}

		if( f1.Length() not_eq f2.Length() ){
			if( f1.Length() > f2.Length() )
				line =  wxString::Format( wxT("\nFile #2 ends at offset %"wxLongLongFmtSpec"u. File #1 has extra %"wxLongLongFmtSpec"u bytes.\n"),f2.Length(), f1.Length() - f2.Length() );

			else
				line =  wxString::Format( wxT("\nFile #1 ends at offset %"wxLongLongFmtSpec"u. File #2 has extra %"wxLongLongFmtSpec"u bytes.\n"),f1.Length(), f2.Length() - f1.Length() );

			fs.Write( line );
			}
		}
	int file1size= f1.Length();
	int file2size= f2.Length();
	f1.Close();
	f2.Close();
	fs.Close();

	HexEditor* hexeditor1 = parent->OpenFile( fl1 );
	HexEditor* hexeditor2 = parent->OpenFile( fl2 );
	if(hexeditor1 != NULL and hexeditor2 != NULL){
		for(int i = 0 ; i < diffHit-1 ; i+=2){
			TagElement *mytag=new TagElement(diffBuff[i], diffBuff[i+1],wxEmptyString,*wxBLACK, *wxRED );
			hexeditor1->CompareArray.Add(mytag);
			hexeditor2->CompareArray.Add(mytag);
			}
		if( file1size not_eq file2size ){
			if( file1size > file2size ){
				TagElement *mytag=new TagElement(file2size, file1size,_("This part doesn't exist at compared file"),*wxBLACK, *wxCYAN );
				hexeditor1->CompareArray.Add(mytag);
				}
			else{
				TagElement *mytag=new TagElement(file1size, file2size,_("This part doesn't exist at compared file"),*wxBLACK, *wxCYAN );
				hexeditor2->CompareArray.Add(mytag);
				}
			}
		//Is selection needed to show first tag?
		hexeditor1->Reload(); //To highlighting current screen
		hexeditor2->Reload(); //To highlighting current screen
		if( hexeditor1->CompareArray.Count() > 0 )
			hexeditor1->UpdateCursorLocation( hexeditor1->CompareArray.Item(0)->start );
		wxUpdateUIEvent eventx( COMPARE_CHANGE_EVENT );
		hexeditor1->GetEventHandler()->ProcessEvent( eventx );
		}

	return true;
	}

void CompareDialog::OnFileChange( wxFileDirPickerEvent& event ){
	if( filePick1->GetPath() not_eq wxEmptyString and filePick2->GetPath() not_eq wxEmptyString)
		btnCompare->Enable(true);
	else
		btnCompare->Enable(false);
	}

void CompareDialog::EventHandler2( wxDropFilesEvent& event ){
	wxBell();
	if( filePick1->GetPath() not_eq wxEmptyString and filePick2->GetPath() not_eq wxEmptyString)
		btnCompare->Enable(true);
	else
		btnCompare->Enable(false);
	}

// TODO (death#1#): Drag Drop file change event!
void CompareDialog::EventHandler( wxCommandEvent& event ){
#ifdef _DEBUG_
	std::cout << "CompareDialog::EventHandler()" << std::endl;
#endif
	if(event.GetId() == wxID_CANCEL)
		Destroy();
	else if(event.GetId() == btnCompare->GetId()){
		if( not filePick1->GetPath().IsEmpty() and not filePick2->GetPath().IsEmpty()){
			if( checkSaveResults->GetValue() and filePickSave->GetPath().IsEmpty() ){
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
				Destroy();
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
	options|=checkMergeSection->GetValue() ? OPT_CMP_MERGE_SECTION : 0;
	options|=checkStopCompare->GetValue() ? OPT_CMP_STOP_AFTER : 0;
	options|=checkSaveResults->GetValue() ? OPT_CMP_SAVE : 0;
	options|=m_radioDifferent->GetValue() ? OPT_CMP_SEARCH_DIFF : 0;
	wxConfigBase::Get()->Write( _T("CompareOptions"), options );

	int optionStopAfter = spinStopCompare->GetValue();
	int optionMergeSection = spinMergeSection->GetValue();
	wxConfigBase::Get()->Write( _T("CompareOptionStopAfter"), optionStopAfter );
	wxConfigBase::Get()->Write( _T("CompareOptionMergeSection"), optionMergeSection );
	}

ChecksumDialog::ChecksumDialog( wxWindow* parent_ ):ChecksumDialogGui(parent_, wxID_ANY){
	parent = static_cast< HexEditorFrame* >(parent_);
	bool active_hex = parent->GetActiveHexEditor() not_eq NULL;
	chkFile->Enable(active_hex);
	chkFile->SetValue(active_hex);
	filePick->Enable(not active_hex);
	btnCalculate->Enable( active_hex );

	int options;
	wxConfigBase::Get()->Read( _T("SelectedChecksumFunctions"), &options );

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

void ChecksumDialog::EventHandler( wxCommandEvent& event ){
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

	wxConfigBase::Get()->Write( _T("SelectedChecksumFunctions"), options );

	if(event.GetId() == wxID_CANCEL)
		Destroy();

	else if(event.GetId() == btnCalculate->GetId()){
		wxString msg;
		FAL *F=NULL;
		if( chkFile->GetValue() )
			F=parent->GetActiveHexEditor()->myfile;

		else if( filePick->GetPath() not_eq wxEmptyString ){
			wxFileName fl( filePick->GetPath() );
			F= new FAL( fl );
			}

		msg = CalculateChecksum( *F, options );
		if(msg not_eq wxEmptyString)
			msg = _("File: ") + F->GetFileName().GetFullPath() + wxNewline
								+(chkFile->GetValue() and F->IsChanged() ? _("Notice: Checksum includes non-saved changes.") + wxString(wxNewline) + wxNewline : wxNewline )
								+ msg;

		if(not chkFile->GetValue() and (F not_eq NULL)){
			F->Close();
			delete F;
			}

		//Not looks so good due variable size of the text.
		wxMessageBox( msg + wxNewline + _("Results copied to the clipboard."), _("Checksum Results") );

		if(wxTheClipboard->Open()) {
//			if (wxTheClipboard->IsSupported( wxDF_TEXT )){
			wxTheClipboard->Clear();
			wxTheClipboard->SetData( new wxTextDataObject( msg ));
			wxTheClipboard->Flush();
			wxTheClipboard->Close();
//			}
			}
		}

	else if(event.GetId() == chkFile->GetId() ){
		filePick->Enable( not event.IsChecked() );
		}

	if( options == 0 or not ((filePick->GetPath() not_eq wxEmptyString) or chkFile->GetValue()) )
		btnCalculate->Enable(false);
	else
		btnCalculate->Enable(true);
	}

void ChecksumDialog::OnFileChange( wxFileDirPickerEvent& event ){
	wxCommandEvent e;
	EventHandler( e );
	}

wxString ChecksumDialog::CalculateChecksum(FAL& f, int options){
	f.Seek(0);
	wxString msg = _("Please wait while calculating checksum.");
	wxProgressDialog mypd(_("Calculating Checksum"), msg , 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
	mypd.Show();
	//checksum_options_strings = { "MD5","SHA1","SHA256","SHA384","SHA512" };

	unsigned NumBits=0;
	for( unsigned i = 0; i < 32 ; i ++ ){
		NumBits += (options>>i)&0x1;
		}
	MHASH *myhash=new MHASH[NumBits];
	unsigned i=0;

	hashid algs[]={MHASH_MD2,MHASH_MD4,MHASH_MD5,
						MHASH_SHA1,MHASH_SHA224,MHASH_SHA256,MHASH_SHA384,MHASH_SHA512,
						MHASH_RIPEMD128,MHASH_RIPEMD160,MHASH_RIPEMD256,MHASH_RIPEMD320,
						MHASH_HAVAL128,MHASH_HAVAL160,MHASH_HAVAL192,MHASH_HAVAL224,MHASH_HAVAL256,
						MHASH_TIGER128,MHASH_TIGER160,MHASH_TIGER192,
						MHASH_ADLER32,MHASH_CRC32,MHASH_CRC32B,MHASH_WHIRLPOOL,MHASH_GOST,MHASH_SNEFRU128,MHASH_SNEFRU256};
	for( unsigned j = 0 ; j < sizeof algs/sizeof algs[0]; j++)
		if( options & (1 << algs[j] ))
			myhash[i++]= mhash_init(algs[j]);

	unsigned rdBlockSz=20*128*1024;
	unsigned char buff[rdBlockSz];
	int rd=rdBlockSz;

	uint64_t readfrom=0,read_speed=0, range=f.Length();
	wxString emsg = msg;
	time_t ts,te;
	time (&ts);

	while(rd == rdBlockSz){
		rd = f.Read( buff, rdBlockSz );
		read_speed+=rd;
		readfrom+=rd;

		//Paralelize with OpenMP
		#pragma omp parallel for schedule(dynamic)
		for( i = 0 ; i < NumBits ; i++){
			mhash( myhash[i], buff, rd);
			}

		time(&te);
		if(ts != te ){
			ts=te;
			emsg = msg + wxT("\n") + wxString::Format(_("Hash Speed : %.2f MB/s"), 1.0*(read_speed)/MB);
			read_speed=0;
			}
		if(not mypd.Update((readfrom*1000)/range, emsg ))
		return wxEmptyString;
		}

	wxString results;
	i=0;

	unsigned char *hash;
	wxString AlgName;
	unsigned MaxAlgoName=0;
	for(unsigned i = 0 ; i < NumBits ; i++)
		MaxAlgoName = wxMax( MaxAlgoName, wxString::FromAscii( reinterpret_cast<const char*>(mhash_get_hash_name_static( mhash_get_mhash_algo(myhash[i]) ))).Len() );

	for(unsigned i = 0 ; i < NumBits ; i++){
		AlgName = wxString::FromAscii( reinterpret_cast<const char*>(mhash_get_hash_name_static( mhash_get_mhash_algo(myhash[i]) )));
		results << AlgName;
		results << wxT(":");

		//Instead of tab char, we got better formatter loop here
		for( unsigned j = 0 ; j < MaxAlgoName +1 - AlgName.Len(); j++ )
			results << wxT(" ");

		hash = static_cast<unsigned char *>( mhash_end(myhash[i]) );

		for (unsigned k = 0; k < mhash_get_block_size( mhash_get_mhash_algo(myhash[i]) ); k++)
			results << wxString::Format( wxT("%.2x"), hash[k]);

		results << wxNewline;
		}
	delete [] myhash;
	return results;//		checksum_options_strings = { "MD5","SHA1","SHA256","SHA384","SHA512" };
	}


XORViewDialog::XORViewDialog( wxWindow* parent, wxMemoryBuffer *XORKey_ ):XORViewDialogGui(parent, wxID_ANY){
	XORKey=XORKey_;
	XORtext->SetFocus();
	}

void XORViewDialog::EventHandler( wxCommandEvent& event ){
	if( event.GetId() == wxID_CANCEL ){
		EndModal( wxID_CANCEL );
		}
	else if( event.GetId() == wxID_OK){
		if( XORtext->GetValue()==wxEmptyString ){
			wxBell();
			return;
			}
		if( radioHex->GetValue() ){
			wxString hexval = XORtext->GetValue();
			if(not HexVerifyAndPrepare( hexval, wxT("XOR"), this ) )
				return;
			wxMemoryBuffer z = wxHexCtrl::HexToBin( hexval );
			XORKey->AppendData( z.GetData(), z.GetDataLen() );
			EndModal( wxID_OK );
			}
		else{
			XORKey->AppendData( XORtext->GetValue().ToAscii(), XORtext->GetValue().Len() );
			EndModal( wxID_OK );
			}
	   }
	else if( event.GetId() == XORtext->GetId())
		XORtext->SetFocus();
	event.Skip();
	}

PreferencesDialog::PreferencesDialog( wxWindow* parent ):PreferencesDialogGui(parent, wxID_ANY){
	GetInstalledLanguages( LangNames, LangIds );

	chcLang->Clear();
	chcLang->Append( LangNames );

	wxConfigBase *pConfig = wxConfigBase::Get();

	if ( ! pConfig->Read(_T("Language")).IsEmpty() ){
		int lang = pConfig->Read(_T("Language"), -1) ;
		if ( lang != -1 )
			for( unsigned i = 0 ; i < LangIds.Count() ; i++)
				if( lang == LangIds.Item(i)){
					chcLang->SetSelection(i);
					break;
					}
		}
	else
		chcLang->SetSelection(0);

	wxString TempString;
	bool TempBool;
	if( wxConfigBase::Get()->Read( _T("ColourHexForeground"), &TempString) )				clrPickerForeground->SetColour( TempString );
	if( wxConfigBase::Get()->Read( _T("ColourHexBackground"), &TempString) )				clrPickerBackground->SetColour( TempString );
	if( wxConfigBase::Get()->Read( _T("ColourHexBackgroundZebra"), &TempString) )			clrPickerBackgroundZebra->SetColour( TempString );
	if( wxConfigBase::Get()->Read( _T("ColourHexSelectionForeground"), &TempString) )	clrPickerSelectionForeground->SetColour(TempString);
	if( wxConfigBase::Get()->Read( _T("ColourHexSelectionBackground"), &TempString) )	clrPickerSelectionBackground->SetColour(TempString);
	if( wxConfigBase::Get()->Read( _T("UseCustomHexFormat"), &TempBool ) )					chkCustom->SetValue( TempBool );
	if( wxConfigBase::Get()->Read( _T("CustomHexFormat"), &TempString	)	)					comboCustomHexFormat->SetValue( TempString );
		comboCustomHexFormat->Enable( chkCustom->IsChecked() );
	if( wxConfigBase::Get()->Read( _T("useBytesPerLineLimit"), &TempBool	)	) 				chkBytePerLineLimit->SetValue( TempBool );
		spinBytePerLine->Enable( chkBytePerLineLimit->IsChecked() );

	int TempInt;
	wxConfigBase::Get()->Read( _T("BytesPerLineLimit"), &TempInt, 16);
	spinBytePerLine->SetValue( TempInt );

	wxConfigBase::Get()->Read( _T("FontSize"), &TempInt, 10 );
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

	wxConfigBase::Get()->Read( _T("CharacterEncodingFamily"), &TempString, wxT("DOS") );
	chcCharacterEncodingFamily->SetStringSelection( TempString );

	ExperimentalEncodingsList.Clear();
	for(int i=0; i< AvailableEncodings.Count(); i++){
		if(( AvailableEncodings.Item(i).Find( wxT("Industrial Standard") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("Arabic") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("Hebrew") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("ISCII") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("TSCII") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("ANSEL") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("AtariST") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("EUC") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("Big5") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("JIS") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("GBK") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("Thai") ) not_eq wxNOT_FOUND ) or
			( AvailableEncodings.Item(i).Find( wxT("Iran") ) not_eq wxNOT_FOUND )
			)
			ExperimentalEncodingsList.Add( AvailableEncodings.Item(i) );
			}

	wxCommandEvent e;
	e.SetString( TempString );
	e.SetId( chcCharacterEncodingFamily->GetId() );
	EventHandler( e );

	wxConfigBase::Get()->Read( _T("CharacterEncoding"), &TempString, wxT("OEM - IBM PC/DOS CP437 - MS-DOS Latin US" ));
	if(TempString==wxT("Extended Binary Coded Decimal Interchange Code"))
		TempString=wxT("EBCDIC");
	if( not chcCharacterEncoding->SetStringSelection( TempString ) )
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
	wxConfigBase::Get()->Write( _T("FontSize"), spinFontSize->GetValue() );
   wxConfigBase::Get()->Flush();
	wxUpdateUIEvent eventx( RESET_STYLE_EVENT );
	GetParent()->GetEventHandler()->ProcessEvent( eventx );
	}

void PreferencesDialog::EventHandler( wxCommandEvent& event ) {
	if(event.GetId()==chcCharacterEncodingFamily->GetId()){
		wxArrayString Encodings;
		if( event.GetString()==wxT("Experimental") )
			Encodings=ExperimentalEncodingsList;

		else if( event.GetString()==wxT("Other") ){
			Encodings=AvailableEncodings;
			for(int i=0; i< ExperimentalEncodingsList.Count(); i++)
				Encodings.Remove(ExperimentalEncodingsList.Item(i));

			wxArrayString families=chcCharacterEncodingFamily->GetStrings();
			for(int f=0; f< families.Count(); f++)
				for(int i=0; i< AvailableEncodings.Count(); i++){
					wxString family=families.Item(f);
					if(family==wxT("Extended Binary Coded Decimal Interchange Code"))
						family=wxT("EBCDIC");
					if( AvailableEncodings.Item(i).Find( family ) not_eq wxNOT_FOUND )
						Encodings.Remove( AvailableEncodings.Item(i) );
					}
			}
		else
			for(int i=0; i< AvailableEncodings.Count(); i++){
				wxString family=event.GetString();
				if(family==wxT("Extended Binary Coded Decimal Interchange Code"))
					family=wxT("EBCDIC");
				if(( AvailableEncodings.Item(i).Find( family ) not_eq wxNOT_FOUND ) and
					(	ExperimentalEncodingsList.Index( AvailableEncodings.Item(i) )==wxNOT_FOUND) )
					Encodings.Add( AvailableEncodings.Item(i) );
				}

		Encodings.Sort();
		chcCharacterEncoding->Clear();
		chcCharacterEncoding->Append( Encodings );
		wxString Selection;
		wxConfigBase::Get()->Read( _T("CharacterEncoding"), &Selection );
		if( not chcCharacterEncoding->SetStringSelection( Selection ) )
			chcCharacterEncoding->SetSelection( 0 );
		}

	wxConfigBase::Get()->Write( _T("CharacterEncodingFamily"), chcCharacterEncodingFamily->GetStringSelection() );
	wxConfigBase::Get()->Write( _T("CharacterEncoding"), chcCharacterEncoding->GetStringSelection() );
	wxConfigBase::Get()->Write( _T("FontSize"), spinFontSize->GetValue() );
   wxConfigBase::Get()->Flush();
	wxUpdateUIEvent eventx( RESET_STYLE_EVENT );
	GetParent()->GetEventHandler()->ProcessEvent( eventx );
	}

void PreferencesDialog::OnSave( wxCommandEvent& event ) {
   wxConfigBase::Get()->Write( _T("Language"), LangIds.Item(chcLang->GetSelection()) );

   wxConfigBase::Get()->Write( _T("ColourHexForeground"), clrPickerForeground->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );
   wxConfigBase::Get()->Write( _T("ColourHexBackground"), clrPickerBackground->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );
   wxConfigBase::Get()->Write( _T("ColourHexBackgroundZebra"), clrPickerBackgroundZebra->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );
   wxConfigBase::Get()->Write( _T("ColourHexSelectionForeground"), clrPickerSelectionForeground->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );
   wxConfigBase::Get()->Write( _T("ColourHexSelectionBackground"), clrPickerSelectionBackground->GetColour().GetAsString(wxC2S_HTML_SYNTAX) );

   wxConfigBase::Get()->Write( _T("UseCustomHexFormat"), chkCustom->GetValue() );
	wxConfigBase::Get()->Write( _T("CustomHexFormat"), comboCustomHexFormat->GetValue() );

	wxConfigBase::Get()->Write( _T("UseBytesPerLineLimit"), chkBytePerLineLimit->GetValue() );
	wxConfigBase::Get()->Write( _T("BytesPerLineLimit"), spinBytePerLine->GetValue());

	wxConfigBase::Get()->Write( _T("CharacterEncodingFamily"), chcCharacterEncodingFamily->GetStringSelection() );
	wxConfigBase::Get()->Write( _T("CharacterEncoding"), chcCharacterEncoding->GetStringSelection() );
	wxConfigBase::Get()->Write( _T("FontSize"), spinFontSize->GetValue() );

   wxConfigBase::Get()->Flush();

	wxUpdateUIEvent eventx( RESET_STYLE_EVENT );
	GetParent()->GetEventHandler()->ProcessEvent( eventx );
   Close();
   }

void PreferencesDialog::OnResetColours( wxCommandEvent& event ) {
	if( wxOK == wxMessageBox( _("Do you sure about resetting colours?"), _("Resetting Hex Colours"), wxOK|wxCANCEL) ){
		wxConfigBase::Get()->DeleteEntry(_T("ColourHexForeground"));
		wxConfigBase::Get()->DeleteEntry(_T("ColourHexBackground"));
		wxConfigBase::Get()->DeleteEntry(_T("ColourHexBackgroundZebra"));
		wxConfigBase::Get()->DeleteEntry(_T("ColourHexSelectionForeground"));
		wxConfigBase::Get()->DeleteEntry(_T("ColourHexSelectionBackground"));
		wxConfigBase::Get()->Flush();

		wxUpdateUIEvent eventx( RESET_STYLE_EVENT );
		GetParent()->GetEventHandler()->ProcessEvent( eventx );
		Close();
		}
	}

void PreferencesDialog::OnCustomHexCheck( wxCommandEvent& event ){
	if(event.GetId() == chkCustom->GetId() )
		comboCustomHexFormat->Enable( event.IsChecked() );
	if(event.GetId() == chkBytePerLineLimit->GetId() )
		spinBytePerLine->Enable( event.IsChecked() );

	}

extern wxArrayString GetDeviceList( bool=true );
DeviceBackupDialog::DeviceBackupDialog( wxWindow* parent ):DeviceBackupDialogGui(parent, wxID_ANY){
	wxArrayString disks = GetDeviceList();
	for( unsigned i =0 ; i < disks.Count() ; i++)
		disks[i]=disks.Item(i).AfterLast('/');
	disks.Sort();
	#ifdef __WXMSW__
	for( int i=0 ; i < disks.Count() ; i++ )
		if(disks.Item(i).StartsWith(wxT("\\Device")))
			disks.RemoveAt(i);
	#endif
	chcPartition->Append( disks );
	}


void DeviceBackupDialog::OnBackup( wxCommandEvent &event ){
	wxArrayString disks = GetDeviceList();
	disks.Sort();
	wxFileName src(disks.Item( chcPartition->GetSelection() ) );
	wxFileName dst(filePickBackup->GetPath());

	FAL src_fl(src);

	wxFFile dst_fl(dst.GetFullPath(), wxT("wb"));

	wxString msg = _("Please wait while backing up disk/partition image.");
	wxProgressDialog mypd(_("Disk/Partition Backup"), msg , 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
	mypd.Show();

	MHASH myhash = mhash_init( MHASH_MD5 );

	unsigned rdBlockSz=1024*1024;
	unsigned char buff[rdBlockSz];
	int rd=rdBlockSz;

	uint64_t readfrom=0,read_speed=0, range=src_fl.Length();
	wxString emsg = msg;
	time_t ts,te;
	time (&ts);

	while(rd == rdBlockSz){
		rd=src_fl.Read(buff, rdBlockSz);
		read_speed+=rd;
		readfrom+=rd;
		if( rd != dst_fl.Write( buff, rd ) ){
			wxMessageBox( _("Error on writing to backup image."), _("ERROR!") );
			return;
			}
		mhash( myhash, buff, rd);

		time(&te);
		if(ts != te ){
			ts=te;
			emsg = msg + wxT("\n") + wxString::Format(_("Backup Speed : %.2f MB/s"), 1.0*(read_speed)/MB);
			read_speed=0;
			}
		if(not mypd.Update((readfrom*1000)/range, emsg ))
			return;
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

DeviceRestoreDialog::DeviceRestoreDialog( wxWindow* parent ):DeviceRestoreDialogGui(parent, wxID_ANY){
	wxArrayString disks = GetDeviceList();
	for( unsigned i =0 ; i < disks.Count() ; i++)
		disks[i]=disks.Item(i).AfterLast('/');
	disks.Sort();
	#ifdef __WXMSW__
	for( int i=0 ; i < disks.Count() ; i++ )
		if(disks.Item(i).StartsWith(wxT("\\Device")))
			disks.RemoveAt(i);
	#endif
	chcPartition->Append( disks );
	}

void DeviceRestoreDialog::OnRestore( wxCommandEvent &event ){
	wxArrayString disks = GetDeviceList();
	disks.Sort();
	wxFileName dst(disks.Item( chcPartition->GetSelection() ) );
	wxFileName src(filePickBackup->GetPath());

	wxFFile src_fl(src.GetFullPath(), wxT("rb"));

	FAL dst_fl( dst, FAL::ReadWrite );

	if( wxCANCEL == wxMessageBox( _("WARNING! THIS OPERATION WILL DESTROY EVERY INFORMATION AT :\n")+dst.GetFullPath()+wxT("\n")+_("ARE YOU SURE?"), _("WARNING!"), wxOK|wxCANCEL|wxCENTRE, this ))
		return;

	if( src_fl.Length() > dst_fl.Length() ){
		wxMessageBox( _("ERROR!: Disk space is smaller than the image file."), _("ERROR!"), wxCANCEL|wxCENTRE, this );
		return;
		}

	if( src_fl.Length() < dst_fl.Length() ){
		if( wxCANCEL==wxMessageBox( _("WARNING!: Disk space is bigger than the image file. \nRemaining space will remain untouched."), _("WARNING!"), wxOK|wxCANCEL|wxCENTRE, this ) )
			return;
		}

	wxString msg = _("Please wait while restoring disk/partition.");
	wxProgressDialog mypd(_("Disk/Partition Restore"), msg , 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
	mypd.Show();

	unsigned rdBlockSz=1024*1024;
	unsigned char buff[rdBlockSz];
	int rd=rdBlockSz;

	uint64_t readfrom=0,read_speed=0, range=src_fl.Length();
	wxString emsg = msg;
	time_t ts,te;
	time (&ts);


	while(rd == rdBlockSz){
		rd=src_fl.Read(buff, rdBlockSz);
		read_speed+=rd;
		readfrom+=rd;

		dst_fl.BlockWrite( buff, rd );

		time(&te);
		if(ts != te ){
			ts=te;
			emsg = msg + wxT("\n") + wxString::Format(_("Restore Speed : %.2f MB/s"), 1.0*(read_speed)/MB);
			read_speed=0;
			}
		if(not mypd.Update((readfrom*1000)/range, emsg ))
			return;
		}
	src_fl.Close();
	dst_fl.Close();
	wxMessageBox( _("Restore operation completed."), _("Operation Complete") );
	}


DeviceEraseDialog::DeviceEraseDialog( wxWindow* parent ):DeviceEraseDialogGui(parent, wxID_ANY){
	wxArrayString disks = GetDeviceList();
	for( unsigned i =0 ; i < disks.Count() ; i++)
		disks[i]=disks.Item(i).AfterLast('/');
	disks.Sort();
	chcPartition->Append( disks );
	}


void DeviceEraseDialog::OnErase( wxCommandEvent &event ){
	wxArrayString disks = GetDeviceList();
	disks.Sort();
	wxFileName dst(disks.Item( chcPartition->GetSelection() ) );
	FAL dst_fl( dst, FAL::ReadWrite );

	if( wxCANCEL == wxMessageBox( _("WARNING! THIS OPERATION WILL DESTROY EVERY INFORMATION AT :\n")+dst.GetFullPath()+wxT("\n")+_("ARE YOU SURE?"), _("WARNING!"), wxOK|wxCANCEL|wxCENTRE, this ))
		return;

	wxString msg = _("Please wait while erasing disk/partition image.");
	wxProgressDialog mypd(_("Disk/Partition Erase"), msg , 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
	mypd.Show();

	unsigned rdBlockSz=1024*1024;
	unsigned char buff[rdBlockSz];
	int rd=rdBlockSz;

	uint64_t readfrom=0,read_speed=0, range=dst_fl.Length();
	wxString emsg = msg;
	time_t ts,te;
	time (&ts);

	int sel = radioErase->GetSelection();
	memset( buff, (sel == 0 ? 0x00 : 0xFF ), rdBlockSz);

	srand((unsigned)time(0));

	while(readfrom < range){

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
		if(ts != te ){
			ts=te;
			emsg = msg + wxT("\n") + wxString::Format(_("Erase Speed : %.2f MB/s"), 1.0*(read_speed)/MB);
			read_speed=0;
			}
		if(not mypd.Update((readfrom*1000)/range, emsg ))
			return;
		}

	wxMessageBox( _("Restore operation completed."), _("Operation Complete") );

	}

OSXMessageDialog::OSXMessageDialog( wxWindow* parent, wxString message, wxString Caption):OSXMessageDialogGui(parent, wxID_ANY){
		txtMessage->SetLabel( message );
		txtCaption->SetLabel( Caption );
		wxIcon wxHexEditor_ICON ( wxhex_xpm );
		icon_bitmap->SetIcon(wxHexEditor_ICON);
		this->Fit();
		this->Layout();
	}

void OSXwxMessageBox( wxString message, wxString Caption, int x, wxWindow* parent ){
	#ifndef __WXMAC__
		wxMessageBox( message,Caption,x, parent );
	#else
		OSXMessageDialog osxmsg(parent, message, Caption );
		osxmsg.ShowModal();
	#endif
	}
