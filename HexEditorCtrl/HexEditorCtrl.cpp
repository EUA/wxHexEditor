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
#include "HexEditorCtrl.h"
//???
//BEGIN_EVENT_TABLE(,wxScrolledWindow )
//	EVT_CHAR( wxHexCtrl::OnChar )
//  EVT_PAINT(wxHexCtrl::OnPaint )
//    EVT_SIZE( HexEditorCtrl::OnResize )
//    EVT_RIGHT_DOWN( wxHexCtrl::OnMouseRight )
//    EVT_SET_FOCUS( wxHexCtrl::OnFocus )
//    EVT_KILL_FOCUS( wxHexCtrl::OnKillFocus )
//END_EVENT_TABLE()

HexEditorCtrl::HexEditorCtrl(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style):
    HexEditorCtrlGui(parent, id, pos, size, wxTAB_TRAVERSAL){
	m_static_offset->SetFont(wxFont(10, wxMODERN, wxNORMAL, wxNORMAL, 0, wxT("")));
	m_static_adress->SetFont(wxFont(10, wxMODERN, wxNORMAL, wxNORMAL, 0, wxT("")));
	m_static_byteview->SetFont(wxFont(10, wxMODERN, wxNORMAL, wxNORMAL, 0, wxT("")));
	Dynamic_Connector();
	selection.start_offset = selection.end_offset = 0;
	selection.state = selector::SELECTION_FALSE;
	page_offset=0;
    }
HexEditorCtrl::~HexEditorCtrl( void ){
	Dynamic_Disconnector();
	Clear();
	MainTagArray.Clear();
	}

void HexEditorCtrl::Dynamic_Connector(){
	this->Connect( idTagSelect, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagSelection ), NULL, this );
	this->Connect( idTagEdit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagEdit ), NULL, this );

	hex_ctrl	->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	text_ctrl	->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	hex_ctrl	->Connect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditorCtrl::OnMouseSelectionEnd),NULL, this);
	text_ctrl	->Connect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditorCtrl::OnMouseSelectionEnd),NULL, this);
	hex_ctrl	->Connect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditorCtrl::OnMouseRight),NULL, this);
	text_ctrl	->Connect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditorCtrl::OnMouseRight),NULL, this);
	hex_ctrl	->Connect( wxEVT_MOTION,	wxMouseEventHandler(HexEditorCtrl::OnMouseMove),NULL, this);
	text_ctrl	->Connect( wxEVT_MOTION,	wxMouseEventHandler(HexEditorCtrl::OnMouseMove),NULL, this);
	}

void HexEditorCtrl::Dynamic_Disconnector(){
	this->Disconnect( idTagSelect, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagSelection ), NULL, this );
	this->Disconnect( idTagEdit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagEdit ), NULL, this );

	hex_ctrl	->Disconnect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	text_ctrl	->Disconnect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	hex_ctrl	->Disconnect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditorCtrl::OnMouseSelectionEnd),NULL, this);
	text_ctrl	->Disconnect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditorCtrl::OnMouseSelectionEnd),NULL, this);
	hex_ctrl	->Disconnect( wxEVT_RIGHT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseRight),NULL, this);
	text_ctrl	->Disconnect( wxEVT_RIGHT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseRight),NULL, this);
	hex_ctrl	->Disconnect( wxEVT_MOTION,	wxMouseEventHandler(HexEditorCtrl::OnMouseMove),NULL, this);
	text_ctrl	->Disconnect( wxEVT_MOTION,	wxMouseEventHandler(HexEditorCtrl::OnMouseMove),NULL, this);
	}
//-----READ/WRITE FUNCTIONS-------//

void HexEditorCtrl::ReadFromBuffer( int64_t position, int lenght, char *buffer, bool cursor_reset, bool paint ){
	static wxMutex MyBufferMutex;
	MyBufferMutex.Lock();
	if( lenght != ByteCapacity() ){
		//last line could be NULL;
		}
	Clear( false, cursor_reset );
	wxString text_string;
// Optimized Code
	for( int i=0 ; i<lenght ; i++ ){
		text_string << text_ctrl->Filter(buffer[i]);
		}
	hex_ctrl->SetBinValue(buffer, lenght, false );
	text_ctrl->ChangeValue(text_string, false);
	offset_ctrl->SetValue( position, hex_ctrl->BytePerLine() );
	if( paint ){
		PaintSelection();
		}
	MyBufferMutex.Unlock();
	}

void HexEditorCtrl::TextCharReplace( long char_location, const wxChar chr){
	text_ctrl->Replace( char_location, chr, true );
	char_location *= 2; //Converting Byte location to Hex Location;
	wxString temp = wxString::Format(wxT("%02X"),chr);
	hex_ctrl->Replace(char_location,char_location+2, temp);
	}

void HexEditorCtrl::HexCharReplace(long hex_location, const wxChar chr){
	hex_ctrl->Replace( hex_location, chr );
	hex_location /=2;	// Hex location is now Byte location
	char rdchr = hex_ctrl->ReadByte(hex_location);
	text_ctrl->Replace(	hex_location, rdchr, true );
	}
//-----MENUS--------//

void HexEditorCtrl::ShowContextMenu( const wxMouseEvent& event ){
	wxMenu menu;
	unsigned TagPosition;
	if( event.GetEventObject() == hex_ctrl )
		TagPosition = page_offset + (hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) / 2);
	if( event.GetEventObject() == text_ctrl )
		TagPosition = page_offset + text_ctrl->PixelCoordToInternalPosition( event.GetPosition() );

	TagElement *TAG;
	for( unsigned i = 0 ; i < MainTagArray.Count() ; i++ ){
		TAG = MainTagArray.Item(i);
		if( (TagPosition >= TAG->start ) && (TagPosition <= TAG->end ) ){	//end not included!
			menu.Append(idTagEdit, _T("Tag Edit"));
			break;
			}
		}

	if( selection.state == selector::SELECTION_END ){
		menu.Append(idTagSelect, _T("Tag Selection"));
		}
//  menu.AppendSeparator();
	wxPoint pos = event.GetPosition();
	wxWindow *scr = (wxWindow*) event.GetEventObject();
	pos += scr->GetPosition();
    PopupMenu(&menu, pos);
    // test for destroying items in popup menus
#if 0 // doesn't work in wxGTK!
    menu.Destroy(Menu_Popup_Submenu);
    PopupMenu( &menu, event.GetX(), event.GetY() );
#endif // 0
	}

//-----VISUAL FUNCTIONS------//

bool HexEditorCtrl::Selector( bool mode ){
	static bool polarity_possitive;
	if( FindFocus() == hex_ctrl || FindFocus() == text_ctrl )
		selection.end_offset = page_offset + GetLocalHexInsertionPoint()/2;
	else{
		wxLogError(wxT("Selector without focuse captured"));
		return false;
		}

	bool first_selection = false;
	if( selection.state != selector::SELECTION_TRUE ){			// If no selection available,
		selection.state	= selector::SELECTION_TRUE;				// then selection start procedure code
		selection.start_offset	= selection.end_offset;
		polarity_possitive		= false;
		first_selection			= true;
		}
	return true;
	}

bool HexEditorCtrl::Select ( int64_t start_offset, int64_t end_offset ){
	if( start_offset < 0 || end_offset < 0
//	|| start_offset > myfile->Length() ||  end_offset > myfile->Length() //??
		){
		wxBell();
		return false;
		}
	selection.state	= selector::SELECTION_END;
	selection.start_offset = page_offset;
	selection.end_offset  = end_offset;
	PaintSelection( );
	return true;
	}

void inline HexEditorCtrl::ClearPaint( void ){
	hex_ctrl ->ClearSelection();
	text_ctrl->ClearSelection();
	}

void HexEditorCtrl::PreparePaintTAGs( void ){//TagElement& TAG ){
	TagElement *TAG;
	TagElement *TAX;
	hex_ctrl->TagArray.Clear();
	text_ctrl->TagArray.Clear();
	for( unsigned i = 0 ; i < MainTagArray.Count() ; i ++ ){
		TAG = MainTagArray.Item(i);
		int64_t start_byte = TAG->start;
		int64_t end_byte = TAG->end;

		if(start_byte > end_byte){							// swap if start > end
			int64_t temp = start_byte;
			start_byte = end_byte;
			end_byte = temp;
			}

		if( start_byte >= page_offset + GetByteCount() )	// ...[..].TAG...
			continue;
		else if( end_byte < page_offset )					// ..TAG..[...]...
			continue;

		if( start_byte <= page_offset )						// ...TA[G..]....
			start_byte = page_offset;

		if( end_byte >= page_offset + GetByteCount() )		//...[..T]AG...
			end_byte = GetByteCount() + page_offset;

		start_byte	-= page_offset;
		end_byte	-= page_offset;

		TAX = new TagElement( start_byte, end_byte+1, TAG->tag, TAG->FontClrData, TAG->NoteClrData );
		text_ctrl->TagArray.Add( TAX );
		TAX = new TagElement( start_byte*2, (end_byte+1)*2, TAG->tag, TAG->FontClrData, TAG->NoteClrData );
		hex_ctrl->TagArray.Add( TAX );
		}
	}

void HexEditorCtrl::PaintSelection( void ){
	PreparePaintTAGs();
	if(selection.state != selector::SELECTION_FALSE ){
		int64_t start_byte = selection.start_offset;
		int64_t end_byte = selection.end_offset;

		if(start_byte > end_byte){	// swap if start > end
			int64_t temp = start_byte;
			start_byte = end_byte;
			end_byte = temp;
			}

		if( start_byte >= page_offset + GetByteCount() ){	// ...[..].TAG...
			ClearPaint();
			return;
			}
		else if( start_byte <= page_offset )				// ...TA[G..]....
			start_byte = page_offset;

		if( end_byte < page_offset ){						// ..TAG..[...]...
			ClearPaint();
			return;
			}
		else if( end_byte >= page_offset + GetByteCount() )	//...[..T]AG...
			end_byte = GetByteCount() + page_offset;

		start_byte	-= page_offset;
		end_byte	-= page_offset;

		text_ctrl->SetSelection(start_byte, end_byte+1);
		hex_ctrl ->SetSelection(start_byte*2, (end_byte+1)*2);
		}
	else
		ClearPaint();
	}

void inline HexEditorCtrl::MyFreeze(){
	hex_ctrl->Freeze();
	text_ctrl->Freeze();
	offset_ctrl->Freeze();
	}

void inline HexEditorCtrl::MyThaw(){
	hex_ctrl->Thaw();
	text_ctrl->Thaw();
	offset_ctrl->Thaw();
	}

void HexEditorCtrl::Clear( bool RePaint, bool cursor_reset ){
	hex_ctrl->Clear( RePaint, cursor_reset );
	text_ctrl->Clear( RePaint, cursor_reset );
	offset_ctrl->Clear( RePaint, cursor_reset );
	}

void HexEditorCtrl::OnResize( wxSizeEvent &event){
	int x = event.GetSize().GetX();
	int y = event.GetSize().GetY();

    x -= offset_ctrl->GetSize().GetX() + offset_scroll->GetSize().GetX() + 4 ;	//Remove Offset Control box X because its changeable +4 for borders
    y -= m_static_byteview->GetSize().GetY();	//Remove Head Text Y
    int charx = hex_ctrl->m_Char.x;
	int i=2;
	for(;;i++){
		if( ((charx*3*i)+(charx*i)) > x)
			break;
		}
	i--;

	int textx = charx*i + charx/2;
	int hexx = charx*3*i;

	wxString adress;
	for( int j = 0 ; j < i ; j++ )
		adress << wxString::Format( wxT("%02X "), j );
 	m_static_adress->SetLabel(adress);

	hex_ctrl->SetMinSize( wxSize(hexx, y ));
	hex_ctrl->SetSize( wxSize( hexx, y ));
	text_ctrl->SetMinSize( wxSize( textx, y ));
	text_ctrl->SetSize( wxSize( textx, y ));

	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer1->Add( m_static_offset, 0, wxALIGN_CENTER|wxALL, 0 );
	fgSizer1->Add( m_static_adress, 0, wxLEFT, 3 );
	fgSizer1->Add( m_static_byteview, 0, wxALIGN_CENTER|wxALL, 0 );
	fgSizer1->Add( m_static_null, 0, wxALIGN_CENTER, 3 );
	fgSizer1->Add( offset_ctrl, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 0 );
	fgSizer1->Add( hex_ctrl, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 0 );
	fgSizer1->Add( text_ctrl, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 0 );
	fgSizer1->Add( offset_scroll, 0, wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
//	offset_ctrl->BytePerLine = BytePerLine(); //Not needed, Updated via ReadFromBuffer
    }
//------EVENTS---------//
void HexEditorCtrl::OnMouseLeft(wxMouseEvent& event){
	selection.state=selector::SELECTION_FALSE;
	ClearPaint();
	if( event.GetEventObject() == hex_ctrl ){
		hex_ctrl->SetFocus();
		SetLocalHexInsertionPoint( hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) );
		}
	else if( event.GetEventObject() == text_ctrl ){
		text_ctrl->SetFocus();
		SetLocalHexInsertionPoint( 2 * text_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) - 1);
		}
	}

void HexEditorCtrl::OnMouseMove( wxMouseEvent& event ){
	if(event.m_leftDown){
		int new_location;
		if( event.GetEventObject() == hex_ctrl )
			new_location = hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() );
		else if ( FindFocus() == text_ctrl )
			new_location = 2*(text_ctrl->PixelCoordToInternalPosition( event.GetPosition() ));
		int old_location = GetLocalHexInsertionPoint();
		if( new_location != old_location ){
			if(selection.state != selector::SELECTION_TRUE)		// At first selection
				Selector();
			SetLocalHexInsertionPoint( new_location );
			Selector();
			PaintSelection();
			}
		}
	else{
		event.Skip(); //enable tags but problems with paint?
		}
	}

void HexEditorCtrl::OnMouseSelectionEnd( wxMouseEvent& event ){
	if(selection.state == selector::SELECTION_TRUE)
		selection.state = selector::SELECTION_END;
	event.Skip();
	}

void HexEditorCtrl::OnMouseRight( wxMouseEvent& event ){
	if(event.GetEventObject() == hex_ctrl)
		LastRightClickAt = hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() )/2;
	else if(event.GetEventObject() == text_ctrl)
		LastRightClickAt = text_ctrl->PixelCoordToInternalPosition( event.GetPosition() );
	else
		std::cout << "Right click captured without ctrl!\n";
	ShowContextMenu( event );
	}

void HexEditorCtrl::OnTagSelection( wxCommandEvent& event ){
	if(selection.state == selector::SELECTION_END){
		TagElement *TE = new TagElement;
		TE->start=selection.start_offset;
		TE->end=selection.end_offset;
		TagDialog *x=new TagDialog( *TE, this );
		if( x->ShowModal() == wxID_SAVE ){
			MainTagArray.Add( TE );
			PreparePaintTAGs();
			ClearPaint();
			text_ctrl->RePaint();
			hex_ctrl ->RePaint();
			}
		x->Destroy();
		}
	}
void HexEditorCtrl::OnTagEdit( wxCommandEvent& event ){
	TagElement *TAG;
	int64_t pos = LastRightClickAt;
	for( unsigned i = 0 ; i < MainTagArray.Count() ; i++ ){
		TAG = MainTagArray.Item(i);
		if( pos >= TAG->start && pos <= TAG->end ){
			TagHideAll();	//Hide first, or BUG by double hide...
			TagElement TAGtemp = *TAG;
			TagDialog *x=new TagDialog( TAGtemp, this );
			switch( x->ShowModal() ){
				case wxID_SAVE:
					*TAG = TAGtemp;
					PreparePaintTAGs();
					ClearPaint();
					text_ctrl->RePaint();
					hex_ctrl ->RePaint();
					break;
				case wxID_DELETE:
					delete TAG;
					MainTagArray.Remove(TAG);
					PreparePaintTAGs();
					ClearPaint();
					text_ctrl->RePaint();
					hex_ctrl ->RePaint();
					break;
				default:
					break;
				}
			}
		break;
		}
	}

void HexEditorCtrl::TagHideAll( void ){
	hex_ctrl->OnTagHideAll();
	text_ctrl->OnTagHideAll();
	}

//------ADAPTERS----------//
int HexEditorCtrl::GetLocalHexInsertionPoint(){					//returns position of Hex Cursor
	return hex_ctrl->GetInsertionPoint();
	}
int HexEditorCtrl::GetLocalInsertionPoint(){					//returns position of Text Cursor
	return text_ctrl->GetInsertionPoint();
	}
void HexEditorCtrl::SetLocalHexInsertionPoint( int hex_location ){	//Sets position of Hex Cursor
	text_ctrl->SetInsertionPoint( hex_location/2 );
	hex_ctrl->SetInsertionPoint( hex_location );
	}
int64_t HexEditorCtrl::CursorOffset( void ){
	return GetLocalHexInsertionPoint()/2 + page_offset;
	}
