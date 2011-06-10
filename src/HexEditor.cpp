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
*               email : spamjunkeater at gmail dot com                  *
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
	printf("Rahman ve Rahim olan Allah'ın adıyla.\n"); // Praying to GOD neccessary
	myfile = NULL;
	if( myfilename_ != NULL ) {
		if( !FileOpen( *myfilename_ ) ) {
			}
		}
	offset_scroll->Enable( true );
	Dynamic_Connector();
	copy_mark = new copy_maker( );
	myDialogVector = new DialogVector;
	myDialogVector->goto_hex=0;
	myDialogVector->goto_branch=0;
	myDialogVector->goto_input=0;
	}

HexEditor::~HexEditor() {
	//FileClose();
	Dynamic_Disconnector();

	// Free resources
	delete myDialogVector;
	delete copy_mark;

	}

void HexEditor::Dynamic_Connector() {
	hex_ctrl ->Connect( wxEVT_KEY_DOWN,	wxKeyEventHandler(HexEditor::OnKeyboardInput),NULL, this);
	text_ctrl->Connect( wxEVT_KEY_DOWN,	wxKeyEventHandler(HexEditor::OnKeyboardInput),NULL, this);
	//this is not up arrow key, just means release of key press.
//	hex_ctrl ->Connect( wxEVT_KEY_UP,	wxKeyEventHandler(HexEditor::OnKeyboardSelectionEnd),NULL, this);
//	text_ctrl->Connect( wxEVT_KEY_UP,	wxKeyEventHandler(HexEditor::OnKeyboardSelectionEnd),NULL, this);
	hex_ctrl ->Connect( wxEVT_CHAR,		wxKeyEventHandler(HexEditor::OnKeyboardChar),NULL, this);
	text_ctrl->Connect( wxEVT_CHAR,		wxKeyEventHandler(HexEditor::OnKeyboardChar),NULL, this);
	hex_ctrl ->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditor::OnMouseLeft),NULL, this);
	text_ctrl->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditor::OnMouseLeft),NULL, this);
	hex_ctrl ->Connect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditor::OnMouseSelectionEnd),NULL, this);
	text_ctrl->Connect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditor::OnMouseSelectionEnd),NULL, this);
	hex_ctrl ->Connect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	text_ctrl->Connect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	hex_ctrl ->Connect( wxEVT_MIDDLE_DOWN,wxMouseEventHandler(HexEditor::OnMouseTest),NULL, this);
	text_ctrl->Connect( wxEVT_MIDDLE_DOWN,wxMouseEventHandler(HexEditor::OnMouseTest),NULL, this);
	hex_ctrl ->Connect( wxEVT_MOTION,	wxMouseEventHandler(HexEditor::OnMouseMove),NULL, this);
	text_ctrl->Connect( wxEVT_MOTION,	wxMouseEventHandler(HexEditor::OnMouseMove),NULL, this);
	hex_ctrl ->Connect( wxEVT_MOUSEWHEEL,wxMouseEventHandler(HexEditor::OnMouseWhell),NULL, this);
	text_ctrl->Connect( wxEVT_MOUSEWHEEL,wxMouseEventHandler(HexEditor::OnMouseWhell),NULL, this);
	}

void HexEditor::Dynamic_Disconnector() {
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
	hex_ctrl ->Disconnect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	text_ctrl->Disconnect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditor::OnMouseRight),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_MIDDLE_DOWN,wxMouseEventHandler(HexEditor::OnMouseTest),NULL, this);
	text_ctrl->Disconnect( wxEVT_MIDDLE_DOWN,wxMouseEventHandler(HexEditor::OnMouseTest),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_MOTION,	wxMouseEventHandler(HexEditor::OnMouseMove),NULL, this);
	text_ctrl->Disconnect( wxEVT_MOTION,	wxMouseEventHandler(HexEditor::OnMouseMove),NULL, this);
	}

bool HexEditor::FileOpen(wxFileName& myfilename ) {
	if(myfile!=NULL) {
		wxLogError(_("Critical Error. File pointer is not empty!"));
		return false;
		}
		//Windows Device Loader
#ifdef __WXMSW__
	if( myfilename.GetFullPath().StartsWith( wxT(".:"))
		or myfilename.GetFullPath().StartsWith( wxT("\\Device\\Harddisk") )){ //Windows device file! Let pass it and process under FAM
		myfile = new FAL( myfilename ); //OpenDevice
		if(myfile->IsOpened()) {
			myscroll = new scrollthread(0,this);
//			copy_mark = new copy_maker();
			offset_ctrl->SetOffsetLimit( myfile->Length() );
			LoadFromOffset(0, true);
			SetLocalHexInsertionPoint(0);
			return true;
			}
		else {
			wxMessageBox(_("File cannot open!"),_("Error"), wxOK|wxICON_ERROR, this);
			return false;
			}
		}
	else
#endif
	if ( myfilename.GetSize( ) < 50*MB && myfilename.IsFileWritable() )
		myfile = new FAL( myfilename, FAL::ReadWrite );
	else
		myfile = new FAL( myfilename );
	if(myfile->IsOpened()) {
		myscroll = new scrollthread(0,this);
//			copy_mark = new copy_maker();
		LoadTAGS( myfilename.GetFullPath().Append(wxT(".tags")) ); //Load tags to wxHexEditorCtrl
		tagpanel->Set(MainTagArray); //Sets Tags to Tag panel
		if(MainTagArray.Count() > 0){
			//TODO This doesn't working good
			//tagpanel->Show();
			}
		offset_ctrl->SetOffsetLimit(  myfile->Length() );
		LoadFromOffset(0, true);
		SetLocalHexInsertionPoint(0);
		return true;
		}
	else {
		///Handled on FAM...
		//wxMessageBox(_("File cannot open!"),_("Error"), wxOK|wxICON_ERROR, this);
		return false;
		}
	}

void HexEditor::FileSetXORKey( bool enable ){
	wxMemoryBuffer x;
	if( enable ){
		XORKey = wxGetTextFromUser( _("Note: For switching XORView Thru mode, all Undo&Redo buffer will be reset and non-saved changes will discarded.\n"\
												"Also you can't use methods that changes file size (like delete and inject) with XORView Thru mode enabled.\n\n"\
												"Please Enter XOR key."), _("XORView Thru Warning!") );
		x.AppendData( XORKey.ToAscii(), XORKey.Len() );
		if(XORKey != wxEmptyString){
 			FileReOpen();
			}
		}
	else{
		if( wxMessageBox( _("For switching XORView Thru mode, all Undo&Redo buffer will be reset and non-saved changes will discarded.\n"), _("XORView Thru Warning"), wxOK | wxCANCEL | wxICON_EXCLAMATION ) == wxCANCEL)
			return;
		XORKey=wxEmptyString;
		FileReOpen();
		}
	myfile->SetXORKey( x );
	infopanel->Set( GetFileName(), FileLength(), GetFileAccessModeString(), GetFD(), XORKey );

	Reload();
	wxUpdateUIEvent new_event;
	new_event.SetId( XORVIEW_EVENT );
	new_event.SetString( ( x.GetDataLen() ? wxT("Checked") : wxT("UnChecked")) );
	GetEventHandler()->ProcessEvent( new_event );
	}

bool HexEditor::FileSave( bool question ) {
	if( myfile->IsChanged() ) {
		if ( myfile->GetAccessMode() == FAL::ReadOnly) {
			wxMessageBox( _( "File in Read Only mode. Cannot save file.\n"), _("File Save"), wxOK|wxICON_EXCLAMATION, this );
			return false;
			}
		if ( myfile->IsInjected() ) {
			wxMessageBox( _( "File has some insertion/deletions. You cannot save this file-self (yet). Please use SaveAs.\n"), _("File Save Error."), wxOK|wxICON_EXCLAMATION, this );
			return false;
			}
		int select = wxYES;
		if ( question )
			select=wxMessageBox( _( "Do you want to save this file?\n"), _("File Save"), wxYES_NO|wxCANCEL|wxICON_QUESTION, this );

		switch( select ) {
			case(wxYES):
				if( !myfile->Apply() ) {
					wxMessageBox( _( "File cannot saved. Operation Cancelled\n"), _("File Save Error"), wxOK|wxICON_ERROR, this );
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
	wxFFile savefile( savefilename, _("w") );
	if(savefile.IsOpened()) {
		myfile->Seek( 0, wxFromStart);
		char *buffer = new char[MB];
		while( savefile.Tell() < myfile->Length() ) {
			savefile.Write( buffer, myfile->Read( buffer, MB ) );
			// TODO (death#3#): Progressbar here?
			}
		return true;
		}
	else
		return false;
	}

bool HexEditor::FileClose( bool WithoutChange ) {
	if( myfile != NULL ) {
		if( myfile->IsChanged() and not WithoutChange) {
			int state = wxMessageBox( _( "Do you want to save file?\n"), _("File Has Changed!"), wxYES_NO|wxCANCEL|wxICON_QUESTION, this );
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
		SaveTAGS( myfile->GetFileName() );
		MainTagArray.Empty();
		//myscroll->GetMyThread()->Delete();
//		myscroll->GetMyThread()->Wait();
//		delete myscroll;
//		delete copy_mark;
		Clear( true );
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
	const DiffNode* x = myfile->GetFirstUndoNode();
	if( x != NULL )
		if( x->flag_inject ) {
			//wxMessageBox( _( "Do you want move tags too with this undo?\n"), _("Tag Movement"), wxYES_NO|wxCANCEL|wxICON_QUESTION, this );
			}

	Goto( myfile->Undo() );
#ifdef _DEBUG_FILE_
	std::cout << "Send UnReDo Event" << std::endl;
#endif
	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	}

void HexEditor::DoRedo( void ) {

	Goto( myfile->Redo() );
#ifdef _DEBUG_FILE_
	std::cout << "Send UnReDo Event" << std::endl;
#endif
	const DiffNode* x = myfile->GetFirstUndoNode();
	if( x != NULL )
		if( x->flag_inject ) {
			//TODO: Tag Movement
			//wxMessageBox( _( "Do you want move tags too with this redo?\n"), _("Tag Movement"), wxYES_NO|wxCANCEL|wxICON_QUESTION );
			}
	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	}

void HexEditor::Goto( int64_t cursor_offset, bool set_focus ) {
	if( cursor_offset == -1 ) {
		LoadFromOffset( page_offset, false, true );	//Refresh
		return;
		}
	if(page_offset <= cursor_offset &&
	      page_offset+ByteCapacity() >= cursor_offset) {	//cursor_offset is in visible area
		Reload();					//Reload data needed for undo - redo
		SetLocalHexInsertionPoint( (cursor_offset - page_offset)*2 );
		}
	else { // out of view
		page_offset = cursor_offset;
		page_offset -= static_cast<int64_t>( ByteCapacity() * 0.20 ); // load some bytes behind of request for make lines at top side
		page_offset -= page_offset % BytePerLine();	//to allign offset
		if(page_offset < 0)
			page_offset = 0;
		else if(page_offset > FileLength() )
			page_offset = FileLength() - ByteCapacity() + 2*BytePerLine();
		Reload();
		SetLocalHexInsertionPoint( (cursor_offset - page_offset)*2 );
		}
//	UpdateCursorLocation();
	UpdateOffsetScroll();
	if( set_focus)
		hex_ctrl->SetFocus();
	}

void HexEditor::UpdateOffsetScroll( void ) {
	if( offset_scroll->GetRange() != (myfile->Length() / ByteCapacity()) ||
	      offset_scroll->GetThumbPosition() != page_offset / ByteCapacity() )
		offset_scroll->SetScrollbar(page_offset / ByteCapacity(), 1, (FileLength() / ByteCapacity())+1, 1 );//Adjusting slider to page size
	}

void HexEditor::OnOffsetScroll( wxScrollEvent& event ) {
	if((event.GetEventType() == wxEVT_SCROLL_CHANGED) or (event.GetEventType() == wxEVT_SCROLL_THUMBTRACK)) {
		LoadFromOffset( static_cast<int64_t>(offset_scroll->GetThumbPosition()) * ByteCapacity() );
		UpdateCursorLocation();
#if wxUSE_STATUSBAR
		if( statusbar != NULL )
			statusbar->SetStatusText(wxString::Format(_("Showing Page: %"wxLongLongFmtSpec"u"), wxULongLong(page_offset/ByteCapacity()) ), 0);
#endif
		wxYieldIfNeeded();
		}
	}

void HexEditor::LoadFromOffset(int64_t position, bool cursor_reset, bool paint) {
#ifdef _DEBUG_FILE_
	std::cout << "LoadFromOffset() : " << position << std::endl;
#endif
	myfile->Seek(position, wxFromStart);
	char *buffer = new char[ ByteCapacity() ];
	int readedbytes = myfile->Read(buffer, ByteCapacity());
	if ( readedbytes == -1 ){
		wxMessageBox( _("File cannot read!"),_("Error"), wxOK|wxICON_ERROR );
		delete [] buffer;
		return;
		}
	ReadFromBuffer( position, readedbytes, buffer, cursor_reset, paint );
	delete [] buffer;
	}

void HexEditor::Reload( void ) {
	myfile->Seek(page_offset, wxFromStart);
	char *buffer = new char[ ByteCapacity() ];
	int readedbytes = myfile->Read(buffer, ByteCapacity());
	ReadFromBuffer( page_offset, readedbytes, buffer, false, true );
	delete [] buffer;
	}

void HexEditor::OnResize( wxSizeEvent &event) {
#ifdef _DEBUG_SIZE_
	std::cout << "HexEditor::OnResize() Event Type:" << event.GetEventType() << std::endl ;
	std::cout << "ByteCapacity() Before:" << ByteCapacity() << std::endl;
#endif
	HexEditorCtrl::OnResize( event );
	//event.Skip( true );
	if(myfile != NULL && 0 < ByteCapacity()) {
		offset_scroll->SetScrollbar(page_offset / ByteCapacity(),
		                            1,
		                            1+(myfile->Length() / ByteCapacity()),
		                            1,true
		                           );
		Reload();
		}
	}

bool HexEditor::FileAddDiff( int64_t start_byte, const char* data, int64_t size, bool injection ) {
	return myfile->Add( start_byte, data, size, injection );
	}

void HexEditor::OnKeyboardSelector(wxKeyEvent& event) {
	if(! event.ShiftDown() ) {
		if( select->IsState( select->SELECT_TRUE ) )
			select->SetState( select->SELECT_END );
		}
	else
		Selector();
	}


// TODO (death#1#): BUG: Remove Text Selection when UNDO (CTRL+SHIFT)
// TODO (death#1#): BUG: Hex-Text Selection at release shift action
// TODO (death#5#): File Name star'in * when file changed & saved

void HexEditor::OnKeyboardInput( wxKeyEvent& event ) {
	if(myfile != NULL) {
		wxHexCtrl *myctrl = static_cast<wxHexCtrl*>(event.GetEventObject());
		//Keyboard Selection Code
		if(	event.GetKeyCode()==WXK_UP || event.GetKeyCode()==WXK_NUMPAD_UP ||
		      event.GetKeyCode()==WXK_DOWN || event.GetKeyCode()==WXK_NUMPAD_DOWN ||
		      event.GetKeyCode()==WXK_LEFT || event.GetKeyCode()==WXK_NUMPAD_LEFT ||
		      event.GetKeyCode()==WXK_RIGHT || event.GetKeyCode()==WXK_NUMPAD_RIGHT ||
		      event.GetKeyCode()==WXK_HOME || event.GetKeyCode()==WXK_NUMPAD_HOME ||
		      event.GetKeyCode()==WXK_END || event.GetKeyCode()==WXK_NUMPAD_END ||
		      event.GetKeyCode()==WXK_PAGEUP || event.GetKeyCode()==WXK_NUMPAD_PAGEUP ||
		      event.GetKeyCode()==WXK_PAGEDOWN || event.GetKeyCode()==WXK_NUMPAD_PAGEDOWN
		  )
			OnKeyboardSelector(event);	//Selection Starter call

#ifdef _DEBUG_
		std::cout << "Received KeyCode : " << std::hex << event.GetKeyCode() << std::endl;
#endif

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
					if(page_offset + ByteCapacity() < myfile->Length() ) { //detects if another line is present or not
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
						SetLocalHexInsertionPoint( HexPerLine() - 1 );
						}
					}
				else
					SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() - ((myctrl == hex_ctrl) ? 1 : 2) );
				break;
			case (WXK_RIGHT):
			case (WXK_NUMPAD_RIGHT):
				if( myctrl->GetInsertionPoint() >= myctrl->GetLastPosition() ) {
					if(page_offset + ByteCapacity() < myfile->Length() ) {	//Checks if its EOF or not
						LoadFromOffset( page_offset + BytePerLine() );
						SetLocalHexInsertionPoint( (LineCount() - 1) * HexPerLine() );
						}
					else
						wxBell();
					}
				else
					SetLocalHexInsertionPoint( GetLocalHexInsertionPoint() +  ((myctrl == hex_ctrl)? 1 : 2) );
				break;

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
				if(page_offset + ByteCapacity()*2 < myfile->Length()) { //*2 for cosmetic
					int temp = GetLocalHexInsertionPoint();
					LoadFromOffset( page_offset +  ByteCapacity() );
					SetLocalHexInsertionPoint( temp );
					}
				else {
					int temp = ( GetLocalHexInsertionPoint() %  HexPerLine() ) + ( LineCount()-1 ) * HexPerLine();
					page_offset = myfile->Length() - ByteCapacity();
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

			default: {
					if( event.ControlDown() )
						switch( event.GetKeyCode() ) {
							case( 0x5a ):		// CTRL+Z = UNDO
								if(event.ShiftDown())
									DoRedo();	// UNDO with shift = REDO
								else
									DoUndo();
								break;
							case( 0x59 ):		// CTRL+Y = REDO
								DoRedo();
								break;
							case( 0x53 ): {	// CTRL+S = SAVE
									FileSave();
									// TODO (death#1#): File Name star'in * when file changed & saved
									}
								break;
							case( 0x41 ):		// CTRL+A = ALL
								Select(0, FileLength());
								break;
							case( 0x58 ):		// CTRL+X = CUT
								wxBell();
								break;
							case( 0x43 ):		// CTRL+C = COPY
								CopySelection();
								break;
							case( 0x56 ):		// CTRL+V = PASTE
								PasteFromClipboard();
								break;
							case( 0x46 ):		// CTRL+F = FIND
								//finddlg();
								break;
							case( 0x4f ):		// CTRL+O = OPEN
								wxBell();
								break;
							default:
								event.Skip();// ->OnKeyboardChar( event );
								break;
							}
					else
						event.Skip();// ->OnKeyboardChar( event );
					}


			}//switch end
		UpdateOffsetScroll();
		//OnKeyboardSelector(event);
		PaintSelection( );
		}
	}

void HexEditor::OnKeyboardChar( wxKeyEvent& event ) {
	if(myfile != NULL) {
		wxChar chr = event.GetKeyCode();

		if( event.GetEventObject() == hex_ctrl ) {
			if( isxdigit(chr) ) {
				(chr>='a'&&chr<='f')?(chr-=('a'-'A')):(chr=chr);		// Upper() for Char
				HexCharReplace( GetLocalHexInsertionPoint(), chr);		// write to screen

				char rdchr = hex_ctrl->ReadByte( GetLocalHexInsertionPoint()/2 );	// read from screen
				FileAddDiff( CursorOffset(), &rdchr ,1);				// add node to file

				if( hex_ctrl->GetInsertionPoint() >= hex_ctrl->GetLastPosition() ) {
					if( CursorOffset() + ByteCapacity() <= myfile->Length() ) {	//Checks if its EOF or not
						LoadFromOffset( page_offset + BytePerLine() );
						hex_ctrl->LastLine();
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
					if( page_offset + ByteCapacity() <= myfile->Length() ) {	//Checks if its EOF or not
						LoadFromOffset(page_offset + BytePerLine());
						text_ctrl->LastLine();
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

void HexEditor::SetLocalHexInsertionPoint( int hex_location ) {
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
	else
		std::cout << "Right click captured without ctrl!\n";
	ShowContextMenu( event );
	event.Skip(false);
	}

void HexEditor::ShowContextMenu( const wxMouseEvent& event ) {
	wxMenu menu;
	unsigned TagPosition=0;
	if( event.GetEventObject() == hex_ctrl )
		TagPosition = page_offset + (hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) / 2);
	else if( event.GetEventObject() == text_ctrl )
		TagPosition = page_offset + text_ctrl->PixelCoordToInternalPosition( event.GetPosition() );

	menu.Append(idTagEdit, _T("Tag Edit"));
	menu.Append(idTagAddSelection, _T("New Tag"));
	menu.Append(wxID_COPY, _T("Copy"));
	menu.Append(wxID_PASTE, _T("Paste"));

	if(XORKey == wxEmptyString){
		menu.AppendSeparator();
		menu.Append(wxID_DELETE, _T("Delete"));
		menu.Append(idInjection, _T("Insert"));
		menu.Append(wxID_CUT, _T("Cut"));
		}


	menu.Enable( idTagEdit, false );
	for( unsigned i = 0 ; i < MainTagArray.Count() ; i++ ) {
		TagElement *TAG = MainTagArray.Item(i);
		if( TAG->isCover(TagPosition) ) {
			menu.Enable( idTagEdit, true );
			break;
			}
		}

	menu.Enable( idTagAddSelection, select->IsState( select->SELECT_END) );

	if(XORKey == wxEmptyString){//Disable injection on XORkey
		menu.Enable( idInjection, select->IsState( select->SELECT_FALSE) );
		menu.Enable( wxID_CUT, select->IsState( select->SELECT_END) );
		menu.Enable( wxID_DELETE, select->IsState( select->SELECT_END) );
		}

	menu.Enable( wxID_COPY, select->IsState( select->SELECT_END) );

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
		if(page_offset + ByteCapacity() < myfile->Length() ) { //detects if another line is present or not
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
	UpdateOffsetScroll();
	}

void HexEditor::OnMouseMove( wxMouseEvent& event ) {
#ifdef _DEBUG_MOUSE_
	std::cout << "HexEditor::OnMouseMove Coordinate X:Y = " << event.GetX()	<< " " << event.GetY()
	          << "\tLeft mouse button:" << event.LeftIsDown() << std::endl;
#endif
	if( event.LeftIsDown() ) {
		int spd=0;
		if( event.GetY() < 0 and page_offset != 0) {
			spd = static_cast<int>(0 - pow(2, abs(event.GetY()) / 25));
			(spd < -1024) ? (spd = -1024):(spd=spd);
			}
		else if(event.GetY()> hex_ctrl->GetSize().GetHeight() and page_offset + ByteCapacity() < myfile->Length()) {
			int pointer_diff = event.GetY() - hex_ctrl->GetSize().GetHeight();
			spd = static_cast<int>(pow(2, pointer_diff / 25));
			(spd > 1024) ? (spd = 1024):(spd=spd);
			}
#if defined( __WXMAC__ ) || defined ( __WXMSW__ )
		ScrollNoThread( spd );			//MAC has problem with GuiMutex so useing safe scroll funtion
#ifdef _DEBUG_MOUSE_
		std::cout << "Scroll (Non-Thread) Speed = " << spd << std::endl;
#endif
#else
		myscroll->UpdateSpeed(spd);
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
	while( (!wxTheApp->Pending() and speed != 0 )
	       and ( ((speed > 0) and (page_offset + ByteCapacity() < FileLength()))
	             or ( (speed < 0) and (page_offset > 0) ))
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
//		SetLocalHexInsertionPoint(cursor);
		Selector();
		PaintSelection();
		UpdateCursorLocation( true );
		if( offset_scroll->GetThumbPosition() != page_offset / ByteCapacity() )
			offset_scroll->SetThumbPosition( page_offset / ByteCapacity() );

		if( page_offset == 0 || page_offset + ByteCapacity() >= FileLength() )
			break;
		}
	}

void HexEditor::OnMouseSelectionEnd( wxMouseEvent& event ) {
	HexEditorCtrl::OnMouseSelectionEnd( event );
	myscroll->UpdateSpeed( 0 );
	if( MouseCapture ) {
#ifdef _DEBUG_MOUSE_
		std::cout << "ReleaseMouse()\n" ;
		GetCapture()->ReleaseMouse();//this is proper one but breaks optimizations! Also make problem on debug mode.
#else
		ReleaseMouse();
#endif
		MouseCapture = false;
		}
	}

void HexEditor::UpdateCursorLocation( bool force ) {
	static wxMutex update;

//	static int64_t lastPoint=-1;				//? Speed up Van goh
//	if( !force )
//		if( lastPoint == CursorOffset() )
//			return;
//	lastPoint = CursorOffset();
#ifdef _DEBUG_MUTEX_
	std::cout << "mutex Update Locking..." << std::endl;
#endif
	if( update.TryLock()==wxMUTEX_NO_ERROR ) {
		if( GetLocalHexInsertionPoint()/2+page_offset > FileLength() ) {
			SetLocalHexInsertionPoint( (FileLength() - page_offset)*2 - 1 );
			}

		if( interpreter != NULL ) {
			myfile->Seek( GetLocalHexInsertionPoint()/2+page_offset, wxFromStart );
			wxMemoryBuffer bfr;
			int size = myfile->Read( reinterpret_cast<char*>(bfr.GetWriteBuf( 8 )), 8);
			bfr.UngetWriteBuf( size );
			interpreter->Set( bfr );
			}

		if( dasmpanel != NULL ) {
			wxMemoryBuffer bfr;
			if( not select->IsState( select->SELECT_FALSE ) ){ //If there is a selection, use selection
				myfile->Seek( select->GetStart(), wxFromStart );
				//Take just first 100 bytes!
				int sz = select->GetSize() > 100 ? 100 : select->GetSize();
				int size = myfile->Read( reinterpret_cast<char*>(bfr.GetWriteBuf( sz )), sz);
				bfr.UngetWriteBuf( size );
				dasmpanel->Set( bfr );
				}
			else{ //Take 8 bytes from cursor location
				myfile->Seek( GetLocalHexInsertionPoint()/2+page_offset, wxFromStart );
				int size = myfile->Read( reinterpret_cast<char*>(bfr.GetWriteBuf( 8 )), 8);
				bfr.UngetWriteBuf( size );
				dasmpanel->Set( bfr );
				}
			}

#if wxUSE_STATUSBAR
		if( statusbar != NULL ) {
			statusbar->SetStatusText(wxString::Format(_("Showing Page: %" wxLongLongFmtSpec "u"), wxULongLong(page_offset/ByteCapacity()) ), 0);
			if( offset_ctrl->hex_offset )
				statusbar->SetStatusText(wxString::Format(_("Cursor Offset: 0x%" wxLongLongFmtSpec "X"), wxULongLong(CursorOffset()) ), 1);
			else
				statusbar->SetStatusText(wxString::Format(_("Cursor Offset: %"wxLongLongFmtSpec"u"), wxULongLong(CursorOffset() )), 1);
			uint8_t ch;
			myfile->Seek( CursorOffset() );
			myfile->Read( reinterpret_cast<char*>(&ch), 1);
			statusbar->SetStatusText(wxString::Format(_("Cursor Value: %u"), ch), 2);

			if( select->IsState( select->SELECT_FALSE ) ) {
				statusbar->SetStatusText(_("Selected Block: N/A"), 3);
				statusbar->SetStatusText(_("Block Size: N/A") ,4);
				}
			else {
				statusbar->SetStatusText(wxString::Format(_("Selected Block: %"wxLongLongFmtSpec"u -> %"wxLongLongFmtSpec"u"),wxULongLong(select->GetStart()),wxULongLong(select->GetEnd())), 3);
				statusbar->SetStatusText(wxString::Format(_("Block Size: %"wxLongLongFmtSpec"u"), wxULongLong(select->GetSize())), 4);
				}
			}
#endif // wxUSE_STATUSBAR
#ifdef _DEBUG_MUTEX_
		std::cout << "mutex Update UnLocking..." << std::endl;
#endif
		update.Unlock();
		}
	else
		std::cout << "mutex update cannot lock..." << std::endl;
	}

void HexEditor::OnMouseTest( wxMouseEvent& event ) {
	myfile->ShowDebugState();
	}

void HexEditor::FindDialog( void ) {
	::FindDialog *myfind = new ::FindDialog( this, myfile );
	myfind->ShowModal();
	myfind->Destroy();
	}

void HexEditor::ReplaceDialog( void ) {
	::ReplaceDialog *myfind = new ::ReplaceDialog( this, myfile );
	myfind->ShowModal();
	myfind->Destroy();
	}

void HexEditor::GotoDialog( void ) {
	uint64_t newoffset;
	::GotoDialog *mygoto = new ::GotoDialog( this, newoffset, CursorOffset(), FileLength(), myDialogVector );
	if( mygoto->ShowModal() == wxID_OK ) {
		Goto( newoffset );
		}
	}

bool HexEditor::DeleteSelection( void ) {
#ifdef _DEBUG_
	std::cout << "DeleteSelection!" << std::endl;
#endif
	bool success=false;
	if( not select->IsState( select->SELECT_FALSE )) {
		success = myfile->Add( std::min(select->StartOffset , select->EndOffset), NULL, -select->GetSize(), true );
		if(success)
			MoveTAGS( std::min(select->StartOffset , select->EndOffset), -select->GetSize() );
		select->SetState( select->SELECT_FALSE );
		}
	else {
		wxBell();
		return false;
		}
	Reload();
	infopanel->Set( GetFileName(), FileLength(), GetFileAccessModeString(), GetFD(), XORKey );
	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	return success;
	}

bool HexEditor::InsertBytes( void ) {
#ifdef _DEBUG_
	std::cout << "Insert Bytes!" << std::endl;
#endif
	long injection_size = wxGetNumberFromUser( wxString::Format(_("Notice!: This command will increase the file size and will generate too much overhead on file save.\n" \
	                      "How many bytes do you want to inject to location to offset location %lld?"), CursorOffset()), _("Bytes"), _("Injection!"), 0, 0, 0x7fffffff ); //Max long up to 2 GB insertion.
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
	select->SetState( select->SELECT_FALSE );

	delete [] zerostream;

	infopanel->Set( GetFileName(), FileLength(), GetFileAccessModeString(), GetFD(), XORKey );

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
		infopanel->Set( GetFileName(), FileLength(), GetFileAccessModeString(), GetFD(), XORKey );
		}
	return success;
	}

bool HexEditor::CopySelection( void ) {
	if( not select->IsState( select->SELECT_FALSE )) {
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
					CopyString << wxString::FromAscii( static_cast<const
					                                   char*>(copy_mark->m_buffer.GetData()) );
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
				wxMessageBox(_( "You have no RAM to copy this data.\nOperation cancelled!"), _("Copy To Clipboard Error"), wxOK|wxICON_ERROR, this);
				return false;
				}
			}
		else {
			wxMessageBox(_( "You are tried to copy data more than 10 MB.\n"\
			                "Copying above 10 MB to clipboard is not allowed.\n"\
			                "Only internal copy buffer used!"),
			             _("Info"), wxOK|wxICON_INFORMATION, this);

			void* buff=NULL;
			buff = copy_mark->m_buffer.GetWriteBuf( select->GetSize() );
			if( buff != NULL ) {
				myfile->Seek( select->GetStart() , wxFromStart );
				myfile->Read( static_cast<char*>(buff), select->GetSize());
				copy_mark->m_buffer.UngetWriteBuf( select->GetSize() );
				return true;
				}
			else {
// TODO (death#1#): If there is no ram, use HDD temp file				wxMessageBox(_( "You have no RAM to copy this data.\nOperation cancelled!"), _("Copy To Clipboard Error"), wxOK|wxICON_ERROR, this);
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
	if( focus==HEX_CTRL ) {
		wxString str = copy_mark->GetClipboardData();
		if( ! str.IsEmpty() ) {
			wxMemoryBuffer mymem = wxHexCtrl::HexToBin( str );
			FileAddDiff( CursorOffset(), static_cast<char*>(mymem.GetData()), mymem.GetDataLen() );
			select->SetState( select->SELECT_FALSE );
			Goto( CursorOffset() + str.Len()/3 + 1);
			ret = true;
			}
		}
	else if ( focus==TEXT_CTRL ) {
		wxString str = copy_mark->GetClipboardData();
		if( ! str.IsEmpty() ) {
			char *ch = new char [str.Len()];
			for( unsigned i=0; i<str.Len(); i++ )
				ch[i] = str[i];
			FileAddDiff( CursorOffset(), ch, str.Len() );
			select->SetState( select->SELECT_FALSE );
			Goto( CursorOffset() + str.Len() );
			ret = true;
			}
		}
	else
		wxMessageBox(_( "There is no focus!"), _("Paste Error"), wxOK|wxICON_ERROR, this);

#ifdef _DEBUG_FILE_
	std::cout << "Send UnReDo Event" << std::endl;
#endif
	wxUpdateUIEvent eventx( UNREDO_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	return ret;
	}

