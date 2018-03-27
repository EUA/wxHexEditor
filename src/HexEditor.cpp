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


#include "HexEditor.h"

HexEditor::HexEditor(	wxWindow* parent,
                        int id,
                        wxStatusBar *statbar_,
                        DataInterpreter *interpreter_,
                        InfoPanel *infopanel_,
                        TagPanel *tagpanel_,
                        DisassemblerPanel *dasmpanel_,
                        wxFileName* myfilename_,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style ):
	HexEditorCtrl(parent, id, pos, size, wxTAB_TRAVERSAL),
	statusbar(statbar_),
	interpreter(interpreter_),
	infopanel(infopanel_),
	tagpanel(tagpanel_),
	dasmpanel(dasmpanel_) {
	ComparatorHexEditor=NULL;
	// Here, code praying to the GOD for protecting our open file from wxHexEditor's bugs and other things.
	// This is really crucial step! Be adviced to not remove it, even if you don't believer.
	printf("Rahman ve Rahim olan Allah'ın adıyla.\n");
	myfile = NULL;
#ifndef DO_NOT_USE_THREAD_FOR_SCROLL
	myscrollthread = NULL;
#endif
	if( myfilename_ != NULL ) {
		if( !FileOpen( *myfilename_ ) ) {
			}
		}
	offset_scroll->Enable( true );
	Dynamic_Connector();
	copy_mark = new copy_maker( );
	BlockSelectOffset = -1;
	MouseCapture = false;
	}

void HexEditor::ConnectScroll(HexEditor* connection){
	ComparatorHexEditor=connection;
	connection->ComparatorHexEditor=this;
	}

void HexEditor::DisconnectScroll( void ){
	if(ComparatorHexEditor!=NULL){
		ComparatorHexEditor->ComparatorHexEditor=NULL;
		ComparatorHexEditor=NULL;
		}
	}

HexEditor::~HexEditor() {
	DisconnectScroll();
	//FileClose();
	Dynamic_Disconnector();
	// Free resources
#ifndef DO_NOT_USE_THREAD_FOR_SCROLL
   if(myscrollthread != NULL ){
		myscrollthread->Exit();
		delete myscrollthread;
		}
#endif
	delete copy_mark;
	}

void HexEditor::Dynamic_Connector() {
	Connect( wxEVT_COMMAND_TEXT_UPDATED, 	wxCommandEventHandler( HexEditor::ThreadPaint ), NULL, this );

	hex_ctrl ->Connect( wxEVT_KEY_DOWN,		wxKeyEventHandler(HexEditor::OnKeyboardInput),NULL, this);
	text_ctrl->Connect( wxEVT_KEY_DOWN,		wxKeyEventHandler(HexEditor::OnKeyboardInput),NULL, this);
	//this is not up arrow key, just means release of key press.
//	hex_ctrl ->Connect( wxEVT_KEY_UP,		wxKeyEventHandler(HexEditor::OnKeyboardSelectionEnd),NULL, this);
//	text_ctrl->Connect( wxEVT_KEY_UP,		wxKeyEventHandler(HexEditor::OnKeyboardSelectionEnd),NULL, this);
	hex_ctrl ->Connect( wxEVT_CHAR,			wxKeyEventHandler(HexEditor::OnKeyboardChar),NULL, this);
	text_ctrl->Connect( wxEVT_CHAR,			wxKeyEventHandler(HexEditor::OnKeyboardChar),NULL, this);
	hex_ctrl ->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditor::OnMouseLeft),NULL, this);
	text_ctrl->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditor::OnMouseLeft),NULL, this);
	hex_ctrl ->Connect( wxEVT_LEFT_UP,		wxMouseEventHandler(HexEditor::OnMouseSelectionEnd),NULL, this);
	text_ctrl->Connect( wxEVT_LEFT_UP,		wxMouseEventHandler(HexEditor::OnMouseSelectionEnd),NULL, this);
	offset_ctrl->Connect( wxEVT_RIGHT_DOWN,	wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	hex_ctrl ->Connect( wxEVT_RIGHT_DOWN,	wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	text_ctrl->Connect( wxEVT_RIGHT_DOWN,	wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	hex_ctrl ->Connect( wxEVT_MIDDLE_DOWN,	wxMouseEventHandler(HexEditor::OnMouseTest),NULL, this);
	text_ctrl->Connect( wxEVT_MIDDLE_DOWN,	wxMouseEventHandler(HexEditor::OnMouseTest),NULL, this);
	hex_ctrl ->Connect( wxEVT_MOTION,		wxMouseEventHandler(HexEditor::OnMouseMove),NULL, this);
	text_ctrl->Connect( wxEVT_MOTION,		wxMouseEventHandler(HexEditor::OnMouseMove),NULL, this);
	hex_ctrl ->Connect( wxEVT_MOUSEWHEEL,	wxMouseEventHandler(HexEditor::OnMouseWhell),NULL, this);
	text_ctrl->Connect( wxEVT_MOUSEWHEEL,	wxMouseEventHandler(HexEditor::OnMouseWhell),NULL, this);

	m_static_offset->Connect( wxEVT_LEFT_DCLICK,wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	m_static_offset->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	m_static_offset->Connect( wxEVT_RIGHT_DCLICK,wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	m_static_offset->Connect( wxEVT_RIGHT_DOWN,	wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	m_static_offset->Connect( wxEVT_MOTION,		wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	}

void HexEditor::Dynamic_Disconnector() {
	Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( HexEditor::ThreadPaint ), NULL, this );

	hex_ctrl ->Disconnect( wxEVT_KEY_DOWN,	wxKeyEventHandler(HexEditor::OnKeyboardInput),NULL, this);
	text_ctrl->Disconnect( wxEVT_KEY_DOWN,	wxKeyEventHandler(HexEditor::OnKeyboardInput),NULL, this);
	//this is not up arrow key, just means release of key press.
//	hex_ctrl ->Disconnect( wxEVT_KEY_UP,	wxKeyEventHandler(HexEditor::OnKeyboardSelectionEnd),NULL, this);
//	text_ctrl->Disconnect( wxEVT_KEY_UP,	wxKeyEventHandler(HexEditor::OnKeyboardSelectionEnd),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_CHAR,		wxKeyEventHandler(HexEditor::OnKeyboardChar),NULL, this);
	text_ctrl->Disconnect( wxEVT_CHAR,		wxKeyEventHandler(HexEditor::OnKeyboardChar),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditor::OnMouseLeft),NULL, this);
	text_ctrl->Disconnect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditor::OnMouseLeft),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditor::OnMouseSelectionEnd),NULL, this);
	text_ctrl->Disconnect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditor::OnMouseSelectionEnd),NULL, this);
	offset_ctrl->Disconnect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	text_ctrl->Disconnect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_MIDDLE_DOWN,wxMouseEventHandler(HexEditor::OnMouseTest),NULL, this);
	text_ctrl->Disconnect( wxEVT_MIDDLE_DOWN,wxMouseEventHandler(HexEditor::OnMouseTest),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_MOTION,	wxMouseEventHandler(HexEditor::OnMouseMove),NULL, this);
	text_ctrl->Disconnect( wxEVT_MOTION,	wxMouseEventHandler(HexEditor::OnMouseMove),NULL, this);

	m_static_offset->Disconnect( wxEVT_LEFT_DCLICK,	wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	m_static_offset->Disconnect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	m_static_offset->Disconnect( wxEVT_RIGHT_DCLICK,wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	m_static_offset->Disconnect( wxEVT_RIGHT_DOWN,	wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	m_static_offset->Disconnect( wxEVT_MOTION,		wxMouseEventHandler(HexEditor::OnOffsetMouse),NULL, this);
	}

int HexEditor::HashVerify(wxString hash_file, FAL* File){
	if( File==NULL )
		File=myfile;

	wxFFile f( hash_file, wxT("rt") );
	wxMemoryBuffer mbf;
	hashid hash_alg;
	if( hash_file.EndsWith( wxT(".md5") ))
		hash_alg=MHASH_MD5;
	else if( hash_file.EndsWith( wxT(".sha1") ))
		hash_alg=MHASH_SHA1;
	else//if( hash_file.EndsWith(wxT(".sha256" )) )
		hash_alg=MHASH_SHA256;
	unsigned hash_block_size=mhash_get_block_size( hash_alg );
	if( hash_block_size*2 != f.Read( mbf.GetWriteBuf( hash_block_size*2 ), hash_block_size*2 ) ){
		wxMessageBox(_("Cannot read hash file!"),_("Error!"));
		return -1;
		}
	mbf.UngetWriteBuf(hash_block_size*2);
	wxString MyHashStr = wxString::From8BitData( reinterpret_cast<char*>(mbf.GetData()), hash_block_size*2 );
	wxMemoryBuffer compare = hex_ctrl->HexToBin(MyHashStr);

	File->Seek(0);

	wxString msg = _("Please wait while calculating checksum.");
	wxString emsg = wxT("\n");
	wxProgressDialog mypd(_("Calculating Checksum"), msg+emsg , 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME);
	mypd.Show();

	MHASH myhash=mhash_init(hash_alg);

	enum { rdBlockSz=2*128*1024 };
	unsigned char buff[rdBlockSz];
	int rd=rdBlockSz;

	uint64_t readfrom=0,readspeed=0, range=File->Length();
	time_t ts,te;
	time (&ts);

	while(rd == rdBlockSz){
		rd = File->Read( buff, rdBlockSz );
		readfrom+=rd;
		mhash( myhash, buff, rd);
		time(&te);
		if(ts != te ){
			ts=te;
			emsg = msg + wxT("\n") + _("Hash Speed : ") + wxString::Format( wxT("%.2f "), 1.0*(readfrom-readspeed)/MB) + _("MB/s");
			readspeed=readfrom;
			}
		if(!mypd.Update((readfrom*1000)/range, emsg ))
			return -1;
		}

	wxString results;
	unsigned char *hash;

	hash = static_cast<unsigned char *>( mhash_end(myhash) );
	if( memcmp( compare.GetData(), hash, hash_block_size ) ){
		wxBell();
		wxString msg = wxString(_("File Corrupt!"))+wxT("\n")+_("File Hash:")+wxT("\t");
		for (unsigned k = 0; k < hash_block_size; k++)
			msg += wxString::Format( wxT("%.2x"), hash[k]);
		msg +=wxString( wxT("\n") ) +_("Hash File:")+wxT("\t") + MyHashStr;
		wxMessageBox(msg, _("Hash Result") );
		return 0;
		}
	else{
		wxMessageBox(_("File Verified."), _("Hash Result") );
		return 1;
		}
	}

bool HexEditor::FileOpen(wxFileName& myfilename ) {
	if(myfile!=NULL) {
		wxLogError(_("Critical Error. File pointer is not empty!"));
		return false;
		}

		//Windows Device Loader
#ifdef __WXMSW__
	//if File is Windows device file! Let pass it and process under FAM
	if( myfilename.GetFullPath().StartsWith( wxT(".:"))
		|| myfilename.GetFullPath().StartsWith( wxT("\\Device\\Harddisk") )){
		myfile = new FAL( myfilename ); //OpenDevice
		if(myfile->IsOpened()) {
#ifndef DO_NOT_USE_THREAD_FOR_SCROLL
			myscrollthread = new scrollthread(0,this);
#endif
//			copy_mark = new copy_maker();
			offset_ctrl->SetOffsetLimit( FileLength() );
			sector_size = FDtoBlockSize( GetFD() );//myfile->GetBlockSize();
			LoadFromOffset(0, true);
			SetLocalHexInsertionPoint(0);
			return true;
			}
		else {
			wxMessageBox(_("File cannot open."),_("Error"), wxOK|wxICON_ERROR, this);
			return false;
			}
		}
	else
#endif

	if ( myfilename.GetSize( ) < 50*MB && myfilename.IsFileWritable() )
		myfile = new FAL( myfilename, FAL::ReadWrite, 0 );
	else
		myfile = new FAL( myfilename, FAL::ReadOnly, 0 );

	if(myfile->IsOpened()) {
#ifndef DO_NOT_USE_THREAD_FOR_SCROLL
		myscrollthread = new scrollthread(0,this);
#endif
//			copy_mark = new copy_maker();

		if( myfile->IsProcess() ){
			///autogenerate Memory Map as Tags...
		#ifdef __WXGTK__
			//offset_scroll->Enable(false);
			std::cout << "PID MAPS loading..." << std::endl;
			wxString command( wxT("cat /proc/") );
			command << myfile->GetPID() << wxT("/maps");
			std::cout << command.ToAscii() << std::endl;
			wxArrayString output;

			wxExecute( command, output);
			//output has Count() lines process it

			for( unsigned i=0; i < output.Count() ; i++){
				TagElement *tmp = new TagElement();
				long long unsigned int x;
				output[i].BeforeFirst('-').ToULongLong(&x, 16);
				tmp->start = x;
				tmp->end = x;
				ProcessRAMMap.Add(x);
				output[i].AfterFirst('-').BeforeFirst(' ').ToULongLong(&x,16);
				ProcessRAMMap.Add(x);
				tmp->tag = output[i].AfterLast( wxT(' '));
				tmp->FontClrData.SetColour( *wxBLACK );
				tmp->NoteClrData.SetColour( *wxCYAN );
				MainTagArray.Add(tmp);
				}
		#endif
		#ifdef __WXMSW__
            MEMORY_BASIC_INFORMATION mymeminfo;
            LPCVOID addr, addr_old;
            addr=addr_old=0;
            wxChar bfrx[200];
			memset( bfrx, 0, 200);
			wxString name, name_old;
			while( true ){
				if( addr_old > addr )
					break;
				addr_old=addr;
				VirtualQueryEx( myfile->GetHandle(), addr, &mymeminfo, sizeof( MEMORY_BASIC_INFORMATION));
				GetMappedFileName(myfile->GetHandle(), (LPVOID)addr, bfrx, 200);
				name=wxString(bfrx);
				//std::cout << "Addr :" << addr << " - " << mymeminfo.RegionSize << "   \t State: " << \
				(mymeminfo.State & MEM_COMMIT  ? "Commit" : "") << \
				(mymeminfo.State & MEM_FREE  ? "Free" : "") << \
				(mymeminfo.State & MEM_RESERVE  ? "Reserve" : "") << \
				"  Type: " << \
				(mymeminfo.Type & MEM_IMAGE  ? "Image   " : "") << \
				(mymeminfo.Type & MEM_IMAGE  ? "Mapped  " : "") << \
				(mymeminfo.Type & MEM_IMAGE  ? "Private " : "") << \
				"\tName: " << wxString(bfrx).c_str() << std::endl;
				if( name!=name_old){
					name_old=name;
					TagElement *tmp = new TagElement();
					long long unsigned int x;
					tmp->start = reinterpret_cast<uint64_t>(addr);

					//tmp->end = reinterpret_cast<uint64_t>(addr+mymeminfo.RegionSize);
					//Just for indicate start of the DLL.
					tmp->end = reinterpret_cast<uint64_t>(addr+1);

					ProcessRAMMap.Add(x);
					tmp->tag = name;

					tmp->FontClrData.SetColour( *wxBLACK );
					tmp->NoteClrData.SetColour( *wxCYAN );
					MainTagArray.Add(tmp);
					}
				addr+=mymeminfo.RegionSize;
				}
		#endif
				}

		LoadTAGS( myfilename.GetFullPath().Append(wxT(".tags")) ); //Load tags to wxHexEditorCtrl

		tagpanel->Set(MainTagArray); //Sets Tags to Tag panel
		if(MainTagArray.Count() > 0){
			//TODO This tagpanel->Show() code doesn't working good
			//tagpanel->Show();
			}
		offset_ctrl->SetOffsetLimit(  FileLength() );
		sector_size = myfile->GetBlockSize(); //FDtoBlockSize( GetFD() );
		LoadFromOffset(0, true);
		SetLocalHexInsertionPoint(0);
		return true;
		}
	else {
		///Handled on FAM Layer...
		///wxMessageBox(_("File cannot open."),_("Error"), wxOK|wxICON_ERROR, this);
		return false;
		}
	}

bool HexEditor::IsFileUsingXORKey( void ){
	return myfile->GetXORKey().GetDataLen() != 0;
	}

wxString HexEditor::FileGetXORKey( void ){
	if( myfile->GetXORKey().GetDataLen() == 0 )
		return wxEmptyString;
	else
		return wxString::From8BitData( reinterpret_cast<char*>(myfile->GetXORKey().GetData()),myfile->GetXORKey().GetDataLen() );
	}

void HexEditor::FileSetXORKey( bool enable ){
	wxMemoryBuffer x;
	if( enable ){
		XORViewDialog a( this, &x );
		if( a.ShowModal() == wxID_CANCEL )
			return;

		if(x.GetDataLen() != 0){
 			FileReOpen();
			}
		}
	else{
		if( wxMessageBox( _("Switching to XORView Thru mode will reset Undo&Redo buffer. Unsaved changes will be discarded.\n Note also that you can't use methods that change file size (like delete and inject) while XORView Thru mode is active."), _("XORView Thru Warning"), wxOK | wxCANCEL | wxICON_EXCLAMATION ) == wxCANCEL)
			return;
		FileReOpen();
		}
	myfile->SetXORKey( x );

	infopanel->Set( GetFileName(), FileLength(), GetFileAccessModeString(), GetFD(), FileGetXORKey() );

	Reload();

	//This Triggers (XORView) Extension on Notebook bar...
	wxUpdateUIEvent new_event;
	new_event.SetId( XORVIEW_EVENT );
	new_event.SetString( ( x.GetDataLen() ? wxT("Checked") : wxT("UnChecked")) );
	GetEventHandler()->ProcessEvent( new_event );
	}

bool HexEditor::FileSave( bool question ) {
	if( myfile->IsChanged() ) {
		if ( myfile->GetAccessMode() == FAL::ReadOnly) {
			wxMessageBox( _( "File in Read Only mode. Cannot save file."), _("File Save"), wxOK|wxICON_EXCLAMATION, this );
			return false;
			}
		if ( myfile->IsInjected() ) {
			wxMessageBox( _( "File has some insertion/deletions. You cannot save this file-self (yet). Please use SaveAs."), _("File Save Error."), wxOK|wxICON_EXCLAMATION, this );
			return false;
			}
		int select = wxYES;
		if ( question )
			select=wxMessageBox( wxString(_( "Do you want to save ")).append( myfile->GetFileName().GetName() + "?\n"), _("File Save"), wxYES_NO|wxCANCEL|wxICON_QUESTION, this );

		switch( select ) {
			case(wxYES):
				if( !myfile->Apply() ) {
					wxMessageBox( _( "File cannot saved. Operation Cancelled"), _("File Save Error"), wxOK|wxICON_ERROR, this );
					return false;
					}
			case(wxNO):
				return true;
			case(wxCANCEL):
			default:
				return false;
			}
		}
	else {
		wxMessageBox( _( "File is not changed. Nothing to save.\n"), _("File Save"), wxOK|wxICON_EXCLAMATION, this );
		}
	return false;
	}

bool HexEditor::FileSave( wxString savefilename ) {
// TODO (death#1#): Protection to save opened file/samefile
	wxFFile savefile( savefilename, wxT("wb") );
	wxFileName saveflnm(savefilename); //to extract path for wxGetDiskSpace
	wxDiskspaceSize_t available, total;
	do{
		wxGetDiskSpace( saveflnm.GetPath(), &total, &available );
		if( FileLength() > available ){
			int state = wxMessageBox( wxString::Format( _( "There are not enough free disk space.\nRequired: %s\nAvailable: %s"),
															wxFileName::GetHumanReadableSize( wxULongLong(FileLength()) ),
															wxFileName::GetHumanReadableSize( wxULongLong(available.GetValue()) ) ), _("Not Enought Space"), wxCANCEL|wxOK|wxICON_QUESTION, this );
			if(state==wxCANCEL)
				return false;
			}
		}while(FileLength() > available);

	if(savefile.IsOpened()) {
		myfile->Seek( 0, wxFromStart);
		int64_t range = FileLength();
		wxString msg = _("File save in progress");
		wxString emsg = wxT("\n");
		wxProgressDialog mpd( _("Saving file"),msg+emsg, 1000, this, wxPD_APP_MODAL|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_REMAINING_TIME|wxPD_SMOOTH );
		mpd.Show();
		int BlockSz = 128*1024;
		char *buffer = new char[BlockSz];
		uint64_t readfrom=0,readspeed=0;
		unsigned rd;

		time_t ts,te;
		time (&ts);
		while( savefile.Tell() < FileLength() ) {
			rd=myfile->Read( buffer, BlockSz );
			readfrom+=rd;
			if( savefile.Write( buffer, rd ) != rd ){
				savefile.Close();
				wxRemoveFile( savefilename );
				return false;
				};
			memset(buffer,0,BlockSz);
			time(&te);
			if(ts != te ){
				ts=te;
				emsg = msg + wxT("\n") + _("Write Speed : ") + wxString::Format( wxT("%.2f "), 1.0*(readfrom-readspeed)/MB) + _("MB/s");
				readspeed=readfrom;
				}
			if( !mpd.Update( (readfrom*1000)/range, emsg) ){
				savefile.Close();
				wxRemoveFile( savefilename );
				wxMessageBox( wxString(_("File cannot save as ")).Append( savefilename ),_("Error"), wxOK|wxICON_ERROR, this );
				return false;
				}
			}
		return true;
		}
	else
		return false;
	}

bool HexEditor::FileClose( bool WithoutChange ) {
	if( myfile != NULL ) {
		if( myfile->IsChanged() && !WithoutChange) {
			int state = wxMessageBox( wxString(_( "Do you want to save ")).append( myfile->GetFileName().GetName() + "?\n") , _("File Has Changed!"), wxYES_NO|wxCANCEL|wxICON_QUESTION, this );
			switch(state) {
				case(wxYES):
					if( !FileSave( false ) )
						return false;
					break;
				case(wxNO):
					break;
				case(wxCANCEL):
					return false;
				default:
					wxBell();
				}
			}
		if( !SaveTAGS( myfile->GetFileName() ) && MainTagArray.Count() != 0 )
			while( wxYES == wxMessageBox( _( "TAG file cannot be saved to default location.\nDo you want to Export TAGs file?"), _("TAGs Cannot Be Saved!!"), wxYES_NO|wxYES_DEFAULT|wxICON_EXCLAMATION, this ) )
				{
				wxFileDialog filediag(this,_("Choose a file for export TAGs"),
													wxEmptyString,
													wxEmptyString,
													wxT("*.tags"),
													wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR,
													wxDefaultPosition);
				if(wxID_OK == filediag.ShowModal())
						if( SaveTAGS( wxFileNameFromPath(filediag.GetPath()) ) )
							break;
						else
							wxMessageBox( wxString(_( "TAGs cannot be exported to file : ")).Append( filediag.GetPath() ),_("Error"), wxOK|wxICON_ERROR, this );
				else
					break;
				}

		MainTagArray.Empty();
		CompareArray.Empty();
		HighlightArray.Empty();
		//myscrollthread->GetMyThread()->Delete();
		//myscrollthread->GetMyThread()->Wait();
//		delete myscrollthread;
//		delete copy_mark;
		Clear( true );
		DisconnectScroll(); //disconnects file from scroll link if available
		myfile->Close();
		delete myfile;
		myfile = NULL;
		tagpanel->Set(MainTagArray);

		}
	return true;
	}

bool HexEditor::FileReOpen( void ){
	wxFileName f = myfile->GetFileName();
	uint64_t last_offset = page_offset;
	unsigned last_cursor = GetLocalHexInsertionPoint();
	FileClose( true );
	bool success = FileOpen(f);
	LoadFromOffset( last_offset );
	SetLocalHexInsertionPoint(last_cursor);
	return success;
	};

void HexEditor::DoUndo( void ) {
	//TODO: Tag Movement
	const DiffNode* x = myfile->GetFirstUndoNodeOrLast();
	if( x != NULL )
		if( x->flag_inject )
			MoveTAGS( x->start_offset, -x->size );

	Goto( myfile->Undo() );

#ifdef _DEBUG_FILE_
	std::cout << "Send UnReDo Event" << std::endl;
#endif
	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	}

void HexEditor::DoRedo( void ) {
#ifdef _DEBUG_FILE_
	std::cout << "Send UnReDo Event" << std::endl;
#endif
	const DiffNode* x = myfile->GetFirstUndoNodeOrLast();
	if( x != NULL )
		if( x->flag_inject )
			MoveTAGS( x->start_offset, x->size );

	Goto( myfile->Redo() );

	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	}

void HexEditor::Goto( int64_t cursor_offset, bool set_focus ){
#ifdef _DEBUG_
	std::cout << "Goto: " << cursor_offset << std::endl;
#endif // _DEBUG_FILE_
	if(page_offset <= cursor_offset &&
	      page_offset+ByteCapacity() >= cursor_offset) {	//cursor_offset is in visible area
		Reload();					//Reload data needed for undo - redo
		SetLocalHexInsertionPoint( (cursor_offset - page_offset)*2 );
		}
	else { // out of view
		page_offset = cursor_offset;
		page_offset -= static_cast<int64_t>( ByteCapacity() * 0.20 ); // load some bytes behind of request for make lines at top side
		page_offset -= page_offset % BytePerLine();	//to allign offset
		wxMax(0,page_offset);
		if(page_offset < 0)
			page_offset = 0;
		else if(page_offset > FileLength() )
			page_offset = wxMax(0, FileLength() - ByteCapacity() + 2*BytePerLine());
		Reload();
		SetLocalHexInsertionPoint( (cursor_offset - page_offset)*2 );
		}
//	UpdateCursorLocation();
//	UpdateScrollOffset(); Moved to HexEditorCtrl::ReadFromBuffer code
	if( set_focus )
		hex_ctrl->SetFocus();
	}

void HexEditor::BlockSelect( void ){
	if(BlockSelectOffset == -1 )
		BlockSelectOffset=CursorOffset();
	else{
		Select(BlockSelectOffset, CursorOffset() );
		UpdateCursorLocation(); //Update statusbar
		BlockSelectOffset = -1;
		}
	}

bool HexEditor::FillSelection( void ){
	wxString hexval = wxGetTextFromUser(_( "Enter hex value(s) for fill"), _("Fill Dialog"), wxT("00"), this );
	if(!HexVerifyAndPrepare( hexval, _("Fill"), this ) )
		return false;
	wxMemoryBuffer pattern = hex_ctrl->HexToBin( hexval );
	wxMemoryBuffer buffer;
	for( unsigned i=0; i<select->GetSize() ; i++ )
		buffer.AppendByte( pattern[ i % pattern.GetDataLen() ] );

	myfile->Add( select->GetStart(), reinterpret_cast<const char*>( buffer.GetData()), buffer.GetDataLen(), false );

	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );

	Reload();
	return true;
	}

bool HexEditor::SaveAsDump( void ){
   wxFileDialog filediag(this,
   _("Choose a file for saving dump"), wxEmptyString, wxEmptyString, wxT("*"),
   wxFD_SAVE|wxFD_OVERWRITE_PROMPT|wxFD_CHANGE_DIR, wxDefaultPosition);

   if(wxID_OK == filediag.ShowModal()) {
// TODO (death#1#): Avoid overwrite of original file!
	wxFFile savefile( filediag.GetPath(), wxT("wb") );
      if(savefile.IsOpened()) {
         int rd;
         wxMemoryBuffer m_buffer;
         void* buff=NULL;
			buff = m_buffer.GetWriteBuf( select->GetSize() );

			myfile->Seek( select->GetStart(), wxFromStart);
			rd = myfile->Read( static_cast< char*>( buff ), select->GetSize() );

			m_buffer.UngetWriteBuf( rd );
			savefile.Write( m_buffer.GetData(), rd );
			savefile.Close();
			return true;
			}
	  else {
			wxMessageBox( wxString(_("Dump cannot saved as ")).Append( filediag.GetPath() ),_("Error"), wxOK|wxICON_ERROR, this );
			return false;
			}
      }
	return false;
   }

void HexEditor::OnOffsetScroll( wxScrollEvent& event ) {
	LoadFromOffset( static_cast<int64_t>(offset_scroll->GetThumbPosition()) * BytePerLine() );
	UpdateCursorLocation();
#if wxUSE_STATUSBAR
	if( statusbar != NULL )
		statusbar->SetStatusText(wxString::Format(_("Showing Page: %" wxLongLongFmtSpec "u"), page_offset/ByteCapacity() ), 0);
#endif
		wxYieldIfNeeded();
	}

void HexEditor::LoadFromOffset(int64_t position, bool cursor_reset, bool paint, bool from_comparator) {
#ifdef _DEBUG_FILE_
	std::cout << "\nLoadFromOffset() : " << position << std::endl;
#endif

	//For file compare mode
	if(ComparatorHexEditor!=NULL && !from_comparator)
		ComparatorHexEditor->LoadFromOffset(position, cursor_reset, true, true);

	myfile->Seek(position, wxFromStart);
	char *buffer = new char[ ByteCapacity() ];
	int readedbytes = myfile->Read(buffer, ByteCapacity());

	if( readedbytes == -1 ){
		wxMessageBox( _("File cannot read!"),_("Error"), wxOK|wxICON_ERROR );
		delete [] buffer;
		return;
		}
	ReadFromBuffer( position, readedbytes, buffer, cursor_reset, paint );
	delete [] buffer;
	}

void HexEditor::ThreadPaint(wxCommandEvent& event){
#ifdef _DEBUG_THREAD_SCROLL_
	std::cout << "ThreadPaint() Enter" << std::endl;
#endif // _DEBUG_THREAD_SCROLL_
	if( event.GetId()==THREAD_UPDATE_EVENT){
		LoadFromOffset( page_offset, false, false );
		SetLocalHexInsertionPoint(GetLocalHexInsertionPoint());
		Selector();
		PaintSelection();
		UpdateCursorLocation( true );

///	Offset scroll automated at HexEditorCTRL::ReadFromBuffer
//		if( parent->offset_scroll->GetThumbPosition() != parent->page_offset / parent->BytePerLine() )
//			parent->offset_scroll->SetThumbPosition( parent->page_offset / parent->BytePerLine() );

		event.Skip(false);
#ifndef DO_NOT_USE_THREAD_FOR_SCROLL
		myscrollthread->ThreadScrool.Unlock();
#endif
		}
	}

#if _FSWATCHER_
void HexEditor::OnFileModify(wxFileSystemWatcherEvent &event){
	#ifdef _DEBUG_
	bool x=(event.GetChangeType()&(wxFSW_EVENT_MODIFY|wxFSW_EVENT_RENAME|wxFSW_EVENT_ATTRIB))!=0;
	std::cout << "HexEditor::OnFileModify()... : 0x" << std::hex<< event.GetChangeType() << std::dec << " mask:" << x << std::endl;
	#endif // _DEBUG_
	//wxFSW_EVENT_MODIFY 0x08
	//wxFSW_EVENT_RENAME 0x04
	//wxFSW_EVENT_ATTRIB 0x20 -- Some programs like gedit return this. But AFAIK our opened file is different than actual.
	if( (event.GetChangeType()&(wxFSW_EVENT_MODIFY))!=0
		&& event.GetPath() == myfile->GetFileName().GetFullPath()){
		Reload();
		}
	event.Skip(true);
	}
#endif

void HexEditor::Reload( void ) {
	LoadFromOffset( page_offset, false, true, false );
//	myfile->Seek(page_offset, wxFromStart);
//	char *buffer = new char[ ByteCapacity() ];
//	int readedbytes = myfile->Read(buffer, ByteCapacity());
//	ReadFromBuffer( page_offset, readedbytes, buffer, false, true );
//	delete [] buffer;
	}

void HexEditor::ReDraw(){
#ifdef _DEBUG_
	std::cout << "HexEditor::ReDraw()" << std::endl ;
#endif // _DEBUG_
	wxSizeEvent ev(GetSize());
	OnResize(ev);
	}

void HexEditor::OnResize( wxSizeEvent &event ) {
#ifdef _DEBUG_SIZE_
	std::cout << "HexEditor::OnResize() Event Type:" << event.GetEventType() << std::endl ;
	std::cout << "ByteCapacity() Before:" << ByteCapacity() << std::endl;
#endif
	HexEditorCtrl::OnResize( event );
	//event.Skip( true );
	if(myfile != NULL && 0 < BytePerLine()){
		offset_scroll->SetScrollbar(page_offset / BytePerLine(),
		                            LineCount(),
		                            1 + FileLength() / BytePerLine(),
		                            LineCount(),true
		                           );
		Reload();
		}
	}

bool HexEditor::FileAddDiff( int64_t start_byte, const char* data, int64_t size, bool injection ) {
	return myfile->Add( start_byte, data, size, injection );
	}

void HexEditor::OnKeyboardSelector(wxKeyEvent& event) {
	#ifdef _DEBUG_SELECT_
	std::cout << "HexEditor::OnKeyboardSelector's event.ShiftDown()=" << event.ShiftDown() << std::endl;
	#endif
	//On GTK, only ShiftDown doesn't return event.ShiftDwn()
	#ifdef __WXMSW__
	if(	event.GetKeyCode()==WXK_UP || event.GetKeyCode()==WXK_NUMPAD_UP ||
			event.GetKeyCode()==WXK_DOWN || event.GetKeyCode()==WXK_NUMPAD_DOWN ||
			event.GetKeyCode()==WXK_LEFT || event.GetKeyCode()==WXK_NUMPAD_LEFT ||
			event.GetKeyCode()==WXK_RIGHT || event.GetKeyCode()==WXK_NUMPAD_RIGHT ||
			event.GetKeyCode()==WXK_HOME || event.GetKeyCode()==WXK_NUMPAD_HOME ||
			event.GetKeyCode()==WXK_END || event.GetKeyCode()==WXK_NUMPAD_END ||
			event.GetKeyCode()==WXK_PAGEUP || event.GetKeyCode()==WXK_NUMPAD_PAGEUP ||
			event.GetKeyCode()==WXK_PAGEDOWN || event.GetKeyCode()==WXK_NUMPAD_PAGEDOWN
		)
	#endif
	if( event.ShiftDown() ){
		Selector();
		}
	else if( event.ControlDown() || event.AltDown() ){
		//do nothing!
		}
	else if( select->GetState() ) //Only SetState if there is a selection to decrease OnUpdateUI event overhead!
		select->SetState( false );
	}

// TODO (death#1#): BUG: Remove Text Selection when UNDO (CTRL+SHIFT)
// TODO (death#5#): File Name star'in * when file changed & saved

void HexEditor::OnKeyboardInput( wxKeyEvent& event ) {
#ifdef _DEBUG_
		std::cout << "Received KeyCode : " << std::hex << event.GetKeyCode() << std::endl;
#endif
	if(myfile != NULL) {
		wxHexCtrl *myctrl = static_cast<wxHexCtrl*>(event.GetEventObject());
		//Keyboard Selection Code
		if( event.ShiftDown() &&
			(event.GetKeyCode()==WXK_UP || event.GetKeyCode()==WXK_NUMPAD_UP ||
			event.GetKeyCode()==WXK_DOWN || event.GetKeyCode()==WXK_NUMPAD_DOWN ||
			event.GetKeyCode()==WXK_LEFT || event.GetKeyCode()==WXK_NUMPAD_LEFT ||
			event.GetKeyCode()==WXK_RIGHT || event.GetKeyCode()==WXK_NUMPAD_RIGHT ||
			event.GetKeyCode()==WXK_HOME || event.GetKeyCode()==WXK_NUMPAD_HOME ||
			event.GetKeyCode()==WXK_END || event.GetKeyCode()==WXK_NUMPAD_END ||
			event.GetKeyCode()==WXK_PAGEUP || event.GetKeyCode()==WXK_NUMPAD_PAGEUP ||
			event.GetKeyCode()==WXK_PAGEDOWN || event.GetKeyCode()==WXK_NUMPAD_PAGEDOWN )
			)
                OnKeyboardSelector(event);	//Selection Starter call

		switch (event.GetKeyCode()) {
			case (WXK_UP):
			case (WXK_NUMPAD_UP):
				if (ActiveLine() == 1) {	//If cursor at first line
					if( page_offset == 0 )
						wxBell();					// there is no line over up!
					else {							// Illusion code
						LoadFromOffset( page_offset - BytePerLine() );	//offset decreasing one line & update text with new location, makes screen slide illusion
						UpdateCursorLocation();
						}
					}
				else
					SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() - HexPerLine() );
				break;

			case (WXK_DOWN):
			case (WXK_NUMPAD_DOWN):
				if ( ActiveLine() == LineCount() ) {			//If cursor at bottom of screen
					if(page_offset + ByteCapacity() < FileLength() ) { //detects if another line is present or not
						int temp = GetLocalHexInsertionPoint();	//preserving cursor location
						LoadFromOffset( page_offset + BytePerLine() );	//offset increasing one line and update text with new location, makes screen slide illusion
						SetLocalHexInsertionPoint(temp);			//restoring cursor location
						}
					else {
						wxBell();											//there is no line to slide bell
						}
					}
				else if( ActiveLine() * HexPerLine() > GetLastPosition() )
					wxBell();	//If cursor at last line but not at bottom of screen
				else
					SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() + HexPerLine() );
				break;
// TODO (death#1#): bug in left & text ctrl
			case (WXK_LEFT):
			case (WXK_NUMPAD_LEFT):
				if( GetLocalHexInsertionPoint() == 0) {
					if(page_offset == 0)
						wxBell();
					else {
						LoadFromOffset( page_offset - BytePerLine() );
						SetLocalHexInsertionPoint( HexPerLine() - ((myctrl == hex_ctrl) ? 1 : 2) );
						}
					}
				else
					SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() - ((myctrl == hex_ctrl) ? 1 : 2) );
				break;
			case (WXK_RIGHT):
			case (WXK_NUMPAD_RIGHT):{
				wxHexTextCtrl *mytxtctrl = dynamic_cast<wxHexTextCtrl*>(event.GetEventObject());
				int a=0, b=0;
				if( mytxtctrl ){
				   a = mytxtctrl->GetInsertionPoint();
				   b = mytxtctrl->GetLastPosition();
					}
				else{
					a = myctrl->GetInsertionPoint();
				   b = myctrl->GetLastPosition();
				   }

				if( a >= b ) {
					if(page_offset + ByteCapacity() < FileLength() ) {	//Checks if its EOF or not
						LoadFromOffset( page_offset + BytePerLine() );
						SetLocalHexInsertionPoint( (LineCount() - 1) * HexPerLine() );
						}
					else
						wxBell();
					}
				else
					SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() +  ((myctrl == hex_ctrl)? 1 : 2) );
				break;
				}
			case (WXK_HOME):
			case(WXK_NUMPAD_HOME): {
					SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() - GetLocalHexInsertionPoint() % HexPerLine() );
					break;
					}
			case (WXK_END):
			case(WXK_NUMPAD_END): {
					SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() + HexPerLine() - (GetLocalHexInsertionPoint() % HexPerLine()) - 1  );
					break;
					}
			case (WXK_PAGEUP):
			case (WXK_NUMPAD_PAGEUP):
				if(page_offset - ByteCapacity() > 0) {
//							int temp = GetHexInsertionPoint();
					LoadFromOffset(page_offset -  ByteCapacity());
					UpdateCursorLocation();
//							SetLocalHexInsertionPoint(temp);
					}
				else {
					int temp = GetLocalHexInsertionPoint() % HexPerLine();
					LoadFromOffset( 0 );
					SetLocalHexInsertionPoint(temp);
					wxBell();
					}
				break;
// TODO (death#5#): Add last byte problem. Also text ctrl has +1 issue
			case (WXK_PAGEDOWN):
			case (WXK_NUMPAD_PAGEDOWN):
				if(page_offset + ByteCapacity()*2 < FileLength()) { //*2 for cosmetic
					int temp = GetLocalHexInsertionPoint();
					LoadFromOffset( page_offset +  ByteCapacity() );
					SetLocalHexInsertionPoint( temp );
					}
				else {
					int temp = ( GetLocalHexInsertionPoint() %  HexPerLine() ) + ( LineCount()-1 ) * HexPerLine();
					page_offset = FileLength() - ByteCapacity();
					page_offset += BytePerLine() - page_offset % BytePerLine(); //cosmetic
					Reload();
					SetLocalHexInsertionPoint(temp);
					wxBell();
					}
				break;

			case (WXK_DELETE):
			case (WXK_NUMPAD_DELETE):
				/*if( myctrl->GetInsertionPoint() != myctrl->GetLastPosition() ){
					int hex_loc = myctrl->GetInsertionPoint();
					HexCharReplace(hex_loc,'0');
					myctrl->SetInsertionPoint(hex_loc);
					}
				else
				*/
				wxBell();
				break;

			case (WXK_BACK):
				/*
				if( myctrl->GetInsertionPoint()!=0 ){
					HexCharReplace(myctrl->GetInsertionPoint()-1,'0');
					myctrl->SetInsertionPoint(myctrl->GetInsertionPoint()-1);
					}
				else
					if(page_offset == 0)
						wxBell();
					else{	//Has to be a line over up
				// TODO (death#3#): if BytePerLine() changes, current offset gona be mad, like taking minus argument because it thinks a compleete line over up... spend caution about it.
							page_offset -= myctrl->BytePerLine();
							LoadFromOffset( page_offset );
							HexCharReplace(myctrl->HexPerLine()-1,'0');
							myctrl->SetInsertionPoint(myctrl->HexPerLine()-1);
							}
					*/
				break;

            //case (WXK_CONTROL_T): //Not working?
            case (0x54): //! Normal T
                if( event.ControlDown() )
					HexEditorCtrl::TagCreator( !event.ShiftDown() );
				else
					event.Skip();
                break;

			default: {
//This part handled by wxMenuItem shortcuts.
//					if( event.ControlDown() )
//						switch( event.GetKeyCode() ) {
//							case( 0x5a ):		// CTRL+Z = UNDO
//								if(event.ShiftDown())
//									DoRedo();	// UNDO with shift = REDO
//								else
//									DoUndo();
//								break;
//							case( 0x59 ):		// CTRL+Y = REDO
//								DoRedo();
//								break;
//							case( 0x53 ): {	// CTRL+S = SAVE
//									FileSave();
//									// TODO (death#1#): File Name star'in * when file changed & saved
//									}
//								break;
//							case( 0x41 ):		// CTRL+A = ALL
//								Select(0, FileLength());
//								break;
//							case( 0x58 ):		// CTRL+X = CUT
//								wxBell();
//								break;
//							case( 0x43 ):		// CTRL+C = COPY
//								CopySelection();
//								break;
//							case( 0x56 ):		// CTRL+V = PASTE
//								PasteFromClipboard();
//								break;
//							case( 0x46 ):		// CTRL+F = FIND
//							   FindDialog();
//								break;
//							case( 0x4f ):		// CTRL+O = OPEN
//								wxBell();
//								break;
//							default:
//								event.Skip();// ->OnKeyboardChar( event );
//								break;
//							}
//					else if ( not event.AltDown() )
						event.Skip();// ->OnKeyboardChar( event );
					}


			}//switch end
		//UpdateScrollOffset(); Moved to HexEditorCtrl::ReadFromBuffer code
		OnKeyboardSelector(event);//Keeps Keyboard Selection proper
		PaintSelection( );
		}
	}

void HexEditor::OnKeyboardChar( wxKeyEvent& event ) {
	if( event.AltDown() || event.ControlDown() ){
		//Control if CTRL or ALT true.
		//This will throw event for wxMenu options only if ALT or CTRL modifiers.
		//otherwise event.skip() will also run wxHex::OnChar which makes artefacts.
		event.Skip(); //This required for ALT+F or CTRL+X...
		return;
		}
	if(myfile != NULL ) {
		wxChar chr = event.GetKeyCode();

		if( event.GetEventObject() == hex_ctrl ) {
			if( isxdigit(chr) ) {
				(chr>='a'&&chr<='f')?(chr-=('a'-'A')):(chr=chr);		// Upper() for Char
				HexCharReplace( GetLocalHexInsertionPoint(), chr);		// write to screen

				char rdchr = hex_ctrl->ReadByte( GetLocalHexInsertionPoint()/2 );	// read from screen
				FileAddDiff( CursorOffset(), &rdchr ,1);				// add node to file

				if( hex_ctrl->GetInsertionPoint() >= hex_ctrl->GetLastPosition() ) {
					if( CursorOffset() + ByteCapacity() <= FileLength() ) {	//Checks if its EOF or not
						LoadFromOffset( page_offset + BytePerLine() );
						hex_ctrl->LastLine();
						hex_ctrl->Home();
						}
					else
						wxBell();
					}
				else
					hex_ctrl->NextChar();
				}
			else
				wxBell();
			}
		else if( event.GetEventObject() == text_ctrl ) {
			if( wxString((wxChar)event.GetKeyCode()).IsAscii() &&
			      event.GetKeyCode()!=WXK_BACK &&
			      event.GetKeyCode()!=WXK_DELETE &&
			      !event.ControlDown()
			  ) {

				/*				if( text_ctrl->GetInsertionPoint() >= text_ctrl->GetLastPosition()){
									if(current_offset+hex_ctrl->ByteCapacity() <= myTempFile->Length() ){	//Checks if its EOF or not
										current_offset += hex_ctrl->BytePerLine();
										LoadFromOffset(current_offset);
										text_ctrl->LastLine();	// HexPerLine = BytePerLine()*2
										}
									}
				*/
				int GLIP = GetLocalInsertionPoint();	//this required because TextCharReplace() calls HexCtrl->Replace that alters entering point
				TextCharReplace(GLIP, event.GetKeyCode());
				char rdchr = hex_ctrl->ReadByte( GLIP );	// read from screen
				FileAddDiff( GLIP + page_offset, &rdchr ,1);						// add node to file

				if( text_ctrl->GetInsertionPoint() >= text_ctrl->GetLastPosition() ) {
					if( page_offset + ByteCapacity() <= FileLength() ) {	//Checks if its EOF or not
						LoadFromOffset(page_offset + BytePerLine());
						text_ctrl->LastLine();
						text_ctrl->Home();
						}
					else
						wxBell();
					}
				else
					text_ctrl->NextChar();
				}
			else
				wxBell();
			}
		}
#ifdef _DEBUG_FILE_
	std::cout << "Send UnReDo Event" << std::endl;
#endif
	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	}

void HexEditor::SetLocalHexInsertionPoint( int hex_location, bool from_comparator ) {
	if(ComparatorHexEditor!=NULL && !from_comparator)
		ComparatorHexEditor->SetLocalHexInsertionPoint( hex_location, true);

	HexEditorCtrl::SetLocalHexInsertionPoint( hex_location );
	UpdateCursorLocation();
	}

void HexEditor::OnMouseLeft(wxMouseEvent& event) {
	HexEditorCtrl::OnMouseLeft( event );
#if wxUSE_STATUSBAR
	if ( statusbar ) {
		statusbar->SetStatusText(_("Selected Block: N/A"), 3);
		statusbar->SetStatusText(_("Block Size: N/A") ,4);
		}
#endif // wxUSE_STATUSBAR
	UpdateCursorLocation();
#ifdef _DEBUG_MOUSE_
	std::cout << "CaptureMouse()\n" ;
#endif
	if( MouseCapture == false ) {
		if(FindFocus() == hex_ctrl ) {
			hex_ctrl->CaptureMouse();
			MouseCapture = true;
			}
		else if( FindFocus() == text_ctrl ) {
			text_ctrl->CaptureMouse();
			MouseCapture = true;
			}
		}
	}

void HexEditor::OnMouseRight( wxMouseEvent& event ) {
	if(event.GetEventObject() == hex_ctrl)
		LastRightClickAt = page_offset + hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() )/2;
	else if(event.GetEventObject() == text_ctrl)
		LastRightClickAt = page_offset + text_ctrl->PixelCoordToInternalPosition( event.GetPosition() );
	else if( event.GetEventObject() == offset_ctrl){

		offset_ctrl->OnMouseRight( event );
		int x,y;
		DoGetSize(&x,&y);
		wxSizeEvent mevent(wxSize(x,y));
		OnResize(mevent);
		UpdateCursorLocation();
		return;//to avoid ShowContextMenu
		}

	//Adjust required operations enable/disable
	ShowContextMenu( event );

	event.Skip(false); //Disables HexEditorCtrl::OnMouseRight
	}

void HexEditor::ShowContextMenu( const wxMouseEvent& event ) {
	wxMenu menu;
	uint64_t TagPosition=0;
	if( event.GetEventObject() == hex_ctrl )
		TagPosition = page_offset + (hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) / 2);
	else if( event.GetEventObject() == text_ctrl )
		TagPosition = page_offset + text_ctrl->PixelCoordToInternalPosition( event.GetPosition() );

	menu.Append(wxID_COPY, _("Copy"));
	menu.Append(idCopyAs, wxString( _("CopyAs") ) + wxChar('\t') + wxT("CTRL+SHIFT+C"));
	menu.Append(wxID_PASTE, _("Paste"));
	if(!IsFileUsingXORKey()){
		menu.AppendSeparator();
		menu.Append(wxID_CUT, _("Cut"));
		menu.Append(wxID_DELETE, _("Delete"));
		menu.Append(idInjection, _("Insert"));
		}
//	if(XORKey == wxEmptyString){//Disable injection on XORkey
//		menu.Enable( idInjection, select->IsState( select->SELECT_FALSE) );
//		menu.Enable( wxID_CUT, select->IsState( select->SELECT_END) );
//		menu.Enable( wxID_DELETE, select->IsState( select->SELECT_END) );
//		}

	menu.AppendSeparator();
	menu.Append(idSaveAsDump, wxString( _("Save As Dump") )+wxChar('\t')+wxT("CTRL+ALT+S"));
	menu.Append(idFillSelection, _("Fill Selection"));
	if( BlockSelectOffset == -1 )
		menu.Append(idBlockSelect, _("Set Selection Block Start"));
	else
		menu.Append(idBlockSelect, _("Set Selection Block End"));
	menu.AppendSeparator();
	menu.Append(idTagQuick,        wxString( _("Quick Tag") ) + wxChar('\t') + wxT("CTRL+T"), _("Creates empty tag with Random Color."));
	menu.Append(idTagAddSelection, wxString( _("New Tag") )+ wxChar('\t') + wxT("CTRL+SHIFT+T") );
	menu.Append(idTagEdit,          _("Tag Edit"));
//	menu.AppendSeparator();
//	menu.Append(idClose, 			 _("Close File")); Results Seg-error

	menu.Enable( idTagEdit, false );
	for( unsigned i = 0 ; i < MainTagArray.Count() ; i++ ) {
		TagElement *TAG = MainTagArray.Item(i);
		if( TAG->isCover(TagPosition) ) {
			menu.Enable( idTagEdit, true );
			break;
			}
		}

	menu.Enable( idTagQuick, select->GetState() );
	menu.Enable( idTagAddSelection, select->GetState() );
	menu.Enable( wxID_COPY, select->GetState() );
	menu.Enable( idCopyAs, select->GetState() );
	menu.Enable( idSaveAsDump, select->GetState() );
	menu.Enable( idFillSelection, select->GetState() );
	menu.Enable( wxID_PASTE, !select->GetState() );

	if( !IsBlockDevice() ){
		menu.Enable( wxID_DELETE, select->GetState());
		menu.Enable( idInjection, !select->GetState());
		menu.Enable( wxID_CUT, select->GetState());
		}
	else{
		menu.Enable( wxID_DELETE, false);
		menu.Enable( idInjection, false);
		menu.Enable( wxID_CUT, false);
		}

	if( FileLength() == 0 ){
		unsigned ids[]={wxID_DELETE, idTagQuick, idTagAddSelection, idTagEdit, wxID_COPY, idCopyAs, idSaveAsDump, idFillSelection, wxID_PASTE, wxID_DELETE, wxID_CUT};
		for( unsigned i=0; i< sizeof(ids)/4 ; i++ ){
			menu.Enable( ids[i], false );
			printf("i: %d\n",i);
			}
		menu.Enable( idInjection, false );
		}

	wxPoint pos = event.GetPosition();
	wxWindow *scr = static_cast<wxWindow*>( event.GetEventObject() );
	pos += scr->GetPosition();
	PopupMenu(&menu, pos);
	// test for destroying items in popup menus
#if 0 // doesn't work in wxGTK!
// TODO (death#1#): inspect here	menu.Destroy(Menu_Popup_Submenu);
	PopupMenu( &menu, event.GetX(), event.GetY() );
#endif // 0
	}

void HexEditor::OnMouseWhell( wxMouseEvent& event ) {
#ifdef _DEBUG_MOUSE_
	std::cout << "MouseWhell Rotation = " << event.GetWheelRotation() << "\t Delta = " << event.GetWheelDelta()
	          << "\tLinesPerAction: " << event.GetLinesPerAction() << std::endl;

#endif
	if(event.GetWheelRotation() > 0) {		// Going to UP
		page_offset -= BytePerLine()*event.GetLinesPerAction(); 	//offset decreasing
		if( page_offset <= 0 ) {
			page_offset = 0;
			wxBell();					// there is no line over up!
			}
		Reload();	//update text with new location, makes screen slide illusion
		if( page_offset != 0 ) {
			if( ActiveLine() + event.GetLinesPerAction() <= LineCount() )	//cursor at bottom
				SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() + HexPerLine() * event.GetLinesPerAction() );	//restoring cursor location
			else
				SetLocalHexInsertionPoint( GetLocalHexInsertionPoint()%HexPerLine() + HexPerLine()*(LineCount()-1) );
			}
		}
	else if(event.GetWheelRotation() < 0 ) {	// Going to BOTTOM
		if(page_offset + ByteCapacity() < FileLength() ) { //detects if another line is present or not
			LoadFromOffset( page_offset + BytePerLine() * event.GetLinesPerAction());	//offset increasing line as mouse whell rolls
			if( ActiveLine() > event.GetLinesPerAction() )	//cursor at top GetLinesPerAction
				SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() - HexPerLine() * event.GetLinesPerAction() );	//restoring cursor location
			else
				SetLocalHexInsertionPoint( GetLocalHexInsertionPoint()%HexPerLine() );
			}
		else {
			wxBell();							//there is no line to slide bell
			}
		}
	//UpdateScrollOffset(); Moved to HexEditorCtrl::ReadFromBuffer code
	}

void HexEditor::OnMouseMove( wxMouseEvent& event ) {
#ifdef _DEBUG_MOUSE_
	std::cout << "HexEditor::OnMouseMove Coordinate X:Y = " << event.GetX()	<< " " << event.GetY()
	          << "\tLeft mouse button:" << event.LeftIsDown() << std::endl;
#endif
	if( event.LeftIsDown() ) {
		int spd=0;
		if( event.GetY() < 0 && page_offset != 0) {
			spd = static_cast<int>(0 - pow(2, abs(event.GetY()) / 25));
			(spd < -1024) ? (spd = -1024):(spd=spd);
			}
		else if(event.GetY()> hex_ctrl->GetSize().GetHeight() && page_offset + ByteCapacity() < FileLength()) {
			int pointer_diff = event.GetY() - hex_ctrl->GetSize().GetHeight();
			spd = static_cast<int>(pow(2, pointer_diff / 25));
			(spd > 1024) ? (spd = 1024):(spd=spd);
			}
#ifdef DO_NOT_USE_THREAD_FOR_SCROLL //WXMSW Stuck sometimes if thread on
		ScrollNoThread( spd );
	#ifdef _DEBUG_MOUSE_
		std::cout << "Scroll (Non-Thread) Speed = " << spd << std::endl;
	#endif
#else
	//This is workaround.
	//Reading from RAM doesn't like read from "thread" and return "0xFF" somehow.
	if( myfile->IsProcess() )
		ScrollNoThread( spd );
	else
		myscrollthread->UpdateSpeed(spd);
	#ifdef _DEBUG_MOUSE_
		std::cout << "Scroll (Thread) Speed = " << spd << std::endl;
	#endif
#endif
		HexEditorCtrl::OnMouseMove( event );//Also makes selection in it
		UpdateCursorLocation();//Dont remember why did I put this here? -- Remembered, for make selection block status text proper.
		}
	else
		event.Skip(); //need for tag views
	}

void HexEditor::ScrollNoThread( int speed ) {
	if( ((speed > 0) && (page_offset + ByteCapacity() < FileLength()))
		|| ( (speed < 0) && (page_offset > 0) )
	  ) {
#ifdef _DEBUG_MOUSE_
		std::cout << "Loop Scroll speed  :" << speed << std::endl;
		std::cout << "Loop Pending Event :" << wxTheApp->Pending() << std::endl;
#endif
		page_offset += BytePerLine() * speed;
		if( page_offset < 0 )
			page_offset = 0;
		else if( page_offset + ByteCapacity() >= FileLength() ) {
			page_offset = FileLength() - ByteCapacity();
			page_offset += BytePerLine() - (page_offset % BytePerLine()) ; //cosmetic
			}
		LoadFromOffset( page_offset, false, false );
	//	SetLocalHexInsertionPoint(cursor); //to update cursor location on WXMSW!
		Selector();
		PaintSelection();
		UpdateCursorLocation( true );
		if( offset_scroll->GetThumbPosition() != page_offset / BytePerLine() )
			offset_scroll->SetThumbPosition( page_offset / BytePerLine() );
		}
	return;
	///REMAINING CODE IS DISABLED
	//Old and which has errors in newer versions of wx
	while( (
			!wxTheApp->Pending() &&
			speed != 0
			)
	     && ( ((speed > 0) && (page_offset + ByteCapacity() < FileLength()))
			|| ( (speed < 0) && (page_offset > 0) ))
	     ) {
#ifdef _DEBUG_MOUSE_
		std::cout << "Loop Scroll speed  :" << speed << std::endl;
		std::cout << "Loop Pending Event :" << wxTheApp->Pending() << std::endl;
#endif
		page_offset += BytePerLine() * speed;
		if( page_offset < 0 )
			page_offset = 0;
		else if( page_offset + ByteCapacity() >= FileLength() ) {
			page_offset = FileLength() - ByteCapacity();
			page_offset += BytePerLine() - (page_offset % BytePerLine()) ; //cosmetic
			}
		LoadFromOffset( page_offset, false, false );
	//	SetLocalHexInsertionPoint(cursor); //to update cursor location on WXMSW!
		Selector();
		PaintSelection();
		UpdateCursorLocation( true );
//		if( offset_scroll->GetThumbPosition() != page_offset / ByteCapacity() )
//			offset_scroll->SetThumbPosition( page_offset / ByteCapacity() );
		if( offset_scroll->GetThumbPosition() != page_offset / BytePerLine() )
			offset_scroll->SetThumbPosition( page_offset / BytePerLine() );

		if( page_offset == 0 || page_offset + ByteCapacity() >= FileLength() )
			break;

		//break;
		}
	}

void HexEditor::OnMouseSelectionEnd( wxMouseEvent& event ) {
	HexEditorCtrl::OnMouseSelectionEnd( event );
#ifndef DO_NOT_USE_THREAD_FOR_SCROLL
	myscrollthread->UpdateSpeed( 0 );
#endif
	if( MouseCapture ) {
#if defined( _DEBUG_ )
		std::cout << "GetCapture()->ReleaseMouse()\n" ;
#endif
      //ReleaseMouse(); //this one popup dragging issues, program crash under windows.
		//GetCapture()->ReleaseMouse();//this is proper one but breaks -O3 optimizations.

		//This avoid assetions and do not break the optimizations.
		if(event.GetEventObject()==hex_ctrl)
			hex_ctrl->ReleaseMouse();
		else if (event.GetEventObject()==text_ctrl)
			text_ctrl->ReleaseMouse();
		MouseCapture = false;
		}
	}

void HexEditor::UpdateCursorLocation( bool force ) {
	static wxMutex update;
// TODO (death#1#): Search if speedup available
//	static int64_t lastPoint=-1;				//? Speed up Van goh
//	if( !force )
//		if( lastPoint == CursorOffset() )
//			return;
//	lastPoint = CursorOffset();
#ifdef _DEBUG_MUTEX_
	std::cout << "mutex Update Locking..." << std::endl;
#endif
	if( update.TryLock()!=wxMUTEX_NO_ERROR ){
		std::cout << "mutex update cannot lock..." << std::endl;
		return;
		}

/// This leads recursion!
//		if( GetLocalHexInsertionPoint()/2+page_offset > FileLength() ) {
//			SetLocalHexInsertionPoint( (FileLength() - page_offset)*2 - 1 );
//			}
	wxMemoryBuffer bfr;
	myfile->Seek( CursorOffset(), wxFromStart );
#ifdef _DEBUG_FILE_
	std::cout << "UpdateCursorLocation() read file for panels" << std::endl;
#endif

	int size = myfile->Read( reinterpret_cast<char*>(bfr.GetWriteBuf( 8 )), 8);
	if(size>0)
		bfr.UngetWriteBuf( size );

	if(size>0){
		if( interpreter != NULL ) {
			interpreter->Set( bfr );
			}
		if( dasmpanel != NULL ) {
			if( ! select->GetState() ){ //If there is a NO selection, take 8 bytes from cursor location
				dasmpanel->Set( bfr );
				}
			else{  //If there is a selection, use selection up to 100 bytes.
				myfile->Seek( select->GetStart(), wxFromStart );
				//Take just first 128 bytes!
				int sz = select->GetSize() > 128 ? 128 : select->GetSize();
			#ifdef _DEBUG_FILE_
				std::cout << "UpdateCursorLocation() read file for dasm" << std::endl;
			#endif
				int size = myfile->Read( reinterpret_cast<char*>(bfr.GetWriteBuf( sz )), sz);
				bfr.UngetWriteBuf( size );
				dasmpanel->Set( bfr );
				}
			}
	}
#if wxUSE_STATUSBAR
	if( statusbar != NULL ) {
		statusbar->SetStatusText(wxString::Format(_("Showing Page: %" wxLongLongFmtSpec "u"), page_offset/ByteCapacity() ), 0);
		statusbar->SetStatusText(wxString::Format(_("Cursor Offset: ") +  offset_ctrl->GetFormatedOffsetString( CursorOffset(), true )), 1);
		statusbar->SetStatusText(wxString::Format(_("Cursor Value: %u"), reinterpret_cast<uint8_t*>(bfr.GetData())[0]), 2);
		if( !select->GetState() ) {
			statusbar->SetStatusText(_("Selected Block: N/A"), 3);
			statusbar->SetStatusText(_("Block Size: N/A") ,4);
			}
		else {
			statusbar->SetStatusText(wxString::Format(_("Selected Block: %" wxLongLongFmtSpec "u -> %" wxLongLongFmtSpec "u"),select->GetStart(),select->GetEnd()), 3);
			statusbar->SetStatusText(wxString::Format(_("Block Size: %" wxLongLongFmtSpec "u"), select->GetSize()), 4);
			}
		}
#endif // wxUSE_STATUSBAR
#ifdef _DEBUG_MUTEX_
	std::cout << "mutex Update UnLocking..." << std::endl;
#endif
	update.Unlock();
	}

void HexEditor::OnMouseTest( wxMouseEvent& event ) {
	myfile->ShowDebugState();
	}

void HexEditor::FindDialog( void ) {
	::FindDialog myfind( this, myfile );
	myfind.ShowModal();

	//myfind.Show(true);
//	::FindDialog *myfind = new ::FindDialog( this, myfile );
//	myfind->ShowModal();
//	#ifndef __WXOSX__ // TODO: This might leak memory but OSX magically give error if I Destroy this.. Really Weird. Please help to fix this.
//	myfind->Destroy();
//	#endif
	}

void HexEditor::ReplaceDialog( void ) {
	::ReplaceDialog myfind( this, myfile );
	myfind.ShowModal();
//	::ReplaceDialog *myfind = new ::ReplaceDialog( this, myfile );
//	myfind->ShowModal();
//	#ifndef __WXOSX__ // TODO: This might leak memory but OSX magically give error if I Destroy this.. Really Weird. Please help to fix this.
//	myfind->Destroy();
//	#endif
	}

void HexEditor::CopyAsDialog( void ) {
	::CopyAsDialog mycopyas( this, myfile, HexEditorCtrl::select, &MainTagArray );
	mycopyas.ShowModal();
//	::CopyAsDialog *mycopyas = new ::CopyAsDialog( this, myfile, HexEditorCtrl::select, &MainTagArray );
//	mycopyas->ShowModal();
//	#ifndef __WXOSX__ // TODO: This might leak memory but OSX magically give error if I Destroy this manually... Really Weird...
//	mycopyas->Destroy();
//	#endif
	}

void HexEditor::GotoDialog( void ) {
	uint64_t newoffset;
	::GotoDialog mygoto( this, newoffset, CursorOffset(), FileLength(), FDtoBlockSize( GetFD() ) );
	if( mygoto.ShowModal() == wxID_OK ) {
		Goto( newoffset );
		}
	}

bool HexEditor::InsertBytes( void ) {
#ifdef _DEBUG_
	std::cout << "Insert Bytes!" << std::endl;
#endif
    int dsize=0;
    if (wxTheClipboard->Open()){
		if (wxTheClipboard->IsSupported( wxDF_TEXT )){
			wxTextDataObject data;
			wxTheClipboard->GetData( data );
			dsize=data.GetDataSize();
			}
		wxTheClipboard->Close();
		}
	long injection_size = wxGetNumberFromUser( wxString::Format(_("Notice!: This command will increase the file size and will generate too much overhead on file save.\n" \
	                      "How many bytes do you want to inject to location to offset location %lld?"), CursorOffset()), _("Bytes"), _("Injection!"), dsize, 1, 0x7fffffff ); //Max long up to 2 GB insertion.
	std::cout << "insert " << injection_size << " bytes " << std::endl;
	if( injection_size == -1 )
		return false;

	char* zerostream = new char[injection_size];
	if( zerostream == NULL)
		return false;
	for(int i=0; i < injection_size ; i++) zerostream[i]=0; //Fill stream with 0
	bool success=myfile->Add( CursorOffset(), zerostream, injection_size, true );

	if(success)
		MoveTAGS( CursorOffset(), injection_size );
	select->SetState( false );

	delete [] zerostream;

	infopanel->Set( GetFileName(), FileLength(), GetFileAccessModeString(), GetFD(), FileGetXORKey() );

	Reload();
	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	return success;
	}

bool HexEditor::CutSelection( void ) {
#ifdef _DEBUG_
	std::cout << "CutSelection!" << std::endl;
#endif
	bool success=false;
	if( CopySelection() ) {
		success=DeleteSelection();
		infopanel->Set( GetFileName(), FileLength(), GetFileAccessModeString(), GetFD(), FileGetXORKey() );
		}
	return success;
	}

bool HexEditor::DeleteSelection( void ) {
#ifdef _DEBUG_
	std::cout << "DeleteSelection!" << std::endl;
#endif
	bool success=false;
	if( select->GetState() ) {
		success = myfile->Add( std::min(select->StartOffset , select->EndOffset), NULL, -select->GetSize(), true );
		if(success)
			MoveTAGS( std::min(select->StartOffset , select->EndOffset), -select->GetSize() );

		select->SetState( false );
		}
	else {
		wxBell();
		return false;
		}
	Reload();
	infopanel->Set( GetFileName(), FileLength(), GetFileAccessModeString(), GetFD(), FileGetXORKey() );
	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	return success;
	}


bool HexEditor::CopySelection( void ) {
	if( select->GetState()) {
		uint64_t RAM_limit = 10*MB;
		if(select->GetSize() < RAM_limit) {								//copy to clipboard if < 10 MB
			myfile->Seek( select->GetStart(), wxFromStart );
			void* buff=NULL;
			buff = copy_mark->m_buffer.GetWriteBuf( select->GetSize() );
			if( buff != NULL ) {
				myfile->Read( static_cast< char*>( buff ), select->GetSize() );
				copy_mark->m_buffer.UngetWriteBuf( select->GetSize() );
				wxString CopyString;
				if( focus == HEX_CTRL ) {
					for( unsigned i=0 ; i<select->GetSize() ; i++ )
						CopyString << wxString::Format(wxT("%02X "),static_cast<unsigned char>(copy_mark->m_buffer[i]));
					CopyString.Trim();	//remove last ' '
					}
				else if( focus == TEXT_CTRL )
					{
					copy_mark->m_buffer.AppendByte('\0');

					//CopyString << wxString::FromAscii( static_cast<const char*>(copy_mark->m_buffer.GetData()) );
					CopyString << wxString::From8BitData( static_cast<const char*>(copy_mark->m_buffer.GetData()) );

					if( select->GetSize() > CopyString.size() ) {
						wxMessageBox(_( "WARNING:\n"\
						                "The text you are copying includes a null character\n"\
						                "(00 in the hex pane) and will be truncated!\n\n"\
						                "If copying to another file in wxHexEditor you\n"\
						                "should copy from the Hex pane, not the text pane"),
						             _("Warning"), wxOK|wxICON_INFORMATION, this);
						}
					}
				return copy_mark->SetClipboardData( CopyString );
				}
			else {
				wxMessageBox( wxString(_( "You have no RAM to copy this data.")) + wxT("\n") + _("Operation cancelled!"), _("Copy To Clipboard Error"), wxOK|wxICON_ERROR, this);
				return false;
				}
			}
		else {
			wxMessageBox(_( "You have tried to copy more than 10 MB of data.\n"\
			                "Copying above 10 MB to clipboard is not allowed.\n"\
			                "Only internal copy buffer used!"),
			             _("Info"), wxOK|wxICON_INFORMATION, this);

			void* buff=NULL;
			buff = copy_mark->m_buffer.GetWriteBuf( select->GetSize() );
			if( buff != NULL ) {
				myfile->Seek( select->GetStart() , wxFromStart );
				myfile->Read( static_cast<char*>(buff), select->GetSize());
				copy_mark->m_buffer.UngetWriteBuf( select->GetSize() );
				wxString CopyString = wxT("wxHexEditor Internal Buffer Object : ");
				CopyString += wxString::Format(wxT("%p"), (copy_mark->m_buffer.GetData()) );
				copy_mark->SetClipboardData( CopyString );
				return true;
				}
			else {
// TODO (death#1#): If there is no ram, use HDD temp file
				return false;
				}
			}
		}
	else {
		wxBell();
		return false;
		}
	}

bool HexEditor::PasteFromClipboard( void ) {
	bool ret = false;
	wxString str = copy_mark->GetClipboardData();
		if( str.IsEmpty() )
			wxMessageBox(_("No data available at clipboad!"));
		else if( str.StartsWith(wxT("wxHexEditor Internal Buffer Object : "))){
			wxMessageBox(_("Note: Used internal binary copy buffer at paste operation."));
			myfile->Add( CursorOffset(), static_cast<const char*>(copy_mark->m_buffer.GetData()), copy_mark->m_buffer.GetDataLen(), 0 );
			Select(CursorOffset(), CursorOffset()+copy_mark->m_buffer.GetDataLen()-1);
			Goto( CursorOffset() + copy_mark->m_buffer.GetDataLen());
			ret = true;
			}
		else if( focus==HEX_CTRL ) {
			wxMemoryBuffer mymem = wxHexCtrl::HexToBin( str );
			FileAddDiff( CursorOffset(), static_cast<char*>(mymem.GetData()), mymem.GetDataLen() );
			//select->SetState( false );
			Select(CursorOffset(), CursorOffset()+mymem.GetDataLen() -1);
			Goto( CursorOffset() + mymem.GetDataLen());
			ret = true;
			}
		else if ( focus==TEXT_CTRL ) {
			char *ch = new char [str.Len()];
			for( unsigned i=0; i<str.Len(); i++ )
				ch[i] = str[i];
			FileAddDiff( CursorOffset(), ch, str.Len() );
			//select->SetState( false );
			Select(CursorOffset(), CursorOffset()+str.Len() -1);
			Goto( CursorOffset() + str.Len() );
			ret = true;
			}
		else
			wxMessageBox(wxT( "There is no focus!"), wxT("Paste Error"), wxOK|wxICON_ERROR, this);

#ifdef _DEBUG_FILE_
	std::cout << "Send UnReDo Event" << std::endl;
#endif
	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	return ret;
	}

void HexEditor::OnOffsetMouse(wxMouseEvent& event){
	if( event.LeftIsDown() ){
#ifdef _DEBUG_
	std::cout << "OnOffsetMouse +1" << std::endl;
#endif // _DEBUG_
		LoadFromOffset( wxMin(page_offset +1, FileLength() ));
		}
	if( event.RightIsDown() ){
#ifdef _DEBUG_
	std::cout << "OnOffsetMouse -1" << std::endl;
#endif // _DEBUG_
		LoadFromOffset( wxMax(page_offset -1,0) );
		}
	}

#ifdef __linux__
	#include <linux/i2c-dev.h>
	#define I2C_SMBUS_READ    1
	#define I2C_SMBUS_WRITE   0
	#define I2C_SMBUS_BYTE    1
	#define I2C_SMBUS_BYTE_DATA 2
	#define I2C_SMBUS_WORD_DATA 3
	#define I2C_SMBUS_BLOCK_DATA 5
	#define I2C_SMBUS_I2C_BLOCK_BROKEN 6
	#define I2C_SMBUS_I2C_BLOCK_DATA 8
	#define I2C_SMBUS_BLOCK_MAX 32

union i2c_smbus_data {
    __u8 byte;
    __u16 word;
    __u8 block[I2C_SMBUS_BLOCK_MAX + 2]; /* block[0] is used for length */
                                                /* and one more for PEC */
};

static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data){
        struct i2c_smbus_ioctl_data args;
        args.read_write = read_write;
        args.command = command;
        args.size = size;
        args.data = data;
        return ioctl(file, I2C_SMBUS, &args);
}

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command){
    union i2c_smbus_data data;
    if (i2c_smbus_access(file, I2C_SMBUS_READ, command, I2C_SMBUS_BYTE_DATA, &data))
        return -1;
    else
        return 0x0FF & data.byte;
}

static inline __s32 i2c_smbus_write_byte_data(int file, __u8 command, __u8 value){
    union i2c_smbus_data data;
    data.byte = value;
    return i2c_smbus_access(file, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &data);
}

static inline __s32 i2c_smbus_write_byte(int file, __u8 value){
    return i2c_smbus_access(file, I2C_SMBUS_WRITE, value, I2C_SMBUS_BYTE, NULL);
}

static inline __s32 i2c_smbus_read_block_data(int file, __u8 command, __u8 *values){
  union i2c_smbus_data data;
  int i;
  if (i2c_smbus_access(file,I2C_SMBUS_READ,command,I2C_SMBUS_BLOCK_DATA,&data))
    return -1;
  else {
    for (i = 1; i <= data.block[0]; i++)
      values[i-1] = data.block[i];
    return data.block[0];
  }
}

static inline __s32 i2c_smbus_read_i2c_block_data(int file, __u8 command, __u8 length, __u8 *values){
  union i2c_smbus_data data;
  int i;

  if (length > 32)
    length = 32;
  data.block[0] = length;
  if (i2c_smbus_access(file,I2C_SMBUS_READ,command, length == 32 ? I2C_SMBUS_I2C_BLOCK_BROKEN : I2C_SMBUS_I2C_BLOCK_DATA,&data))
    return -1;
  else {
    for (i = 1; i <= data.block[0]; i++)
      values[i-1] = data.block[i];
    return data.block[0];
  }
}




void HexEditor::OpenMemorySPDDevice( int addr ){
	int file;
	char filename[40];

	sprintf(filename,"/dev/i2c-0");
	if ((file = open(filename,O_RDWR)) < 0) {
		wxMessageBox( _("Failed to open the bus."), _("Error"), wxOK|wxICON_ERROR) ;
		return;
		}

	if (ioctl(file, I2C_SLAVE, addr) < 0){
		printf("ioctl() Error! %s\n", strerror(errno));
		return;
		}

	uint8_t buf[512] = {0};
	printf("\r\n   : ");
	for(int i = 0; i<=0x0F; i++)
		printf("%02X ",i);

#define i2xtest
#ifdef i2xtest
	for(int i = 0; i<256; i++) {
		if( i%0x10==0 ) printf("\r\n%03X: ",i);
		buf[i]=i2c_smbus_read_byte_data(file, i);
		printf( "%02X ", buf[i] );
		}
#else
	for(int i = 0; i<256; i+=32) {
		i2c_smbus_read_block_data(file, i, buf+i);
		}
#endif

	//Flip to page 1
	ioctl(file, I2C_SLAVE, 0x37);
	i2c_smbus_write_byte(file, 0x0);
	ioctl(file, I2C_SLAVE, addr);

#ifdef i2xtest
	for(int i = 0; i<256; i++) {
		if(  i%0x10==0 ) printf("\r\n%03X: ",i+0x100);
		buf[i+0xFF]=i2c_smbus_read_byte_data(file, i);
		printf( "%02X ", buf[i] );
		}
#else
	for(int i = 0; i<256; i+=32) {
		i2c_smbus_read_block_data(file, i, buf+i+0xFF);
		}
#endif
	printf("\r\n");

	//Flip to page 0 back
	ioctl(file, I2C_SLAVE, 0x36);
	i2c_smbus_write_byte(file, 0x0);
	ioctl(file, I2C_SLAVE, addr);

	wxFileName xfl=wxFileNameFromPath(wxString("-buf"));
	//FAL *bufile = new FAL(xfl , FAL::ReadWrite, 0 );
	//bufile->Add(0,reinterpret_cast<char*>(buf),512,true);
	//bufile->Apply();
	//bufile->Close();
	//delete bufile;
	//OpenFile( xfl );

	myfile->Add(0,reinterpret_cast<char*>(buf),512,true);
	myfile->Apply();
	ReDraw();
	}
#else
void HexEditor::OpenMemorySPDDevice( int addr ){}
#endif // __WXGTK__
