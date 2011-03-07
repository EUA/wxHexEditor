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
	select = new class Select( GetEventHandler() );

	m_static_offset->SetLabel( _("Offset: DEC") );

#ifdef __WXOSX__
	stdfont = wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, 0, wxT("Monaco"));// Fonts are too small on wxOSX 2.9.x series.
#else
	stdfont = wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, 0, wxT(""));
#endif
	SetFont( stdfont );

	Dynamic_Connector();
	offset_scroll = new wxHugeScrollBar( offset_scroll_real );
	TAGMutex = false;
	hex_ctrl->TagMutex = &TAGMutex;
	text_ctrl->TagMutex = &TAGMutex;
   }

HexEditorCtrl::~HexEditorCtrl( void ){
	Dynamic_Disconnector();
	Clear();
	MainTagArray.Clear();
	delete select;
	delete offset_scroll;
	}

void HexEditorCtrl::Dynamic_Connector(){
	this->Connect( idTagAddSelection, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagAddSelection ), NULL, this );
	this->Connect( idTagEdit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagEdit ), NULL, this );
	this->Connect( __idOffsetHex__, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorCtrl::UpdateUI ) );
	this->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler(HexEditorCtrl::OnKillFocus),NULL, this);
	hex_ctrl->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler(HexEditorCtrl::OnKillFocus),NULL, this);
	text_ctrl->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler(HexEditorCtrl::OnKillFocus),NULL, this);
	offset_ctrl->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	hex_ctrl ->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	text_ctrl->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	hex_ctrl ->Connect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditorCtrl::OnMouseSelectionEnd),NULL, this);
	text_ctrl->Connect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditorCtrl::OnMouseSelectionEnd),NULL, this);
	hex_ctrl ->Connect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditorCtrl::OnMouseRight),NULL, this);
	text_ctrl->Connect( wxEVT_RIGHT_DOWN,wxMouseEventHandler(HexEditorCtrl::OnMouseRight),NULL, this);
	hex_ctrl ->Connect( wxEVT_MOTION,	wxMouseEventHandler(HexEditorCtrl::OnMouseMove),NULL, this);
	text_ctrl->Connect( wxEVT_MOTION,	wxMouseEventHandler(HexEditorCtrl::OnMouseMove),NULL, this);
	}

void HexEditorCtrl::Dynamic_Disconnector(){
	this->Disconnect( idTagAddSelection, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagAddSelection ), NULL, this );
	this->Disconnect( idTagEdit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagEdit ), NULL, this );
	this->Disconnect( __idOffsetHex__, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( HexEditorCtrl::UpdateUI ) );
   this->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler(HexEditorCtrl::OnKillFocus),NULL, this);
   hex_ctrl->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler(HexEditorCtrl::OnKillFocus),NULL, this);
   text_ctrl->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler(HexEditorCtrl::OnKillFocus),NULL, this);

	offset_ctrl->Disconnect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	text_ctrl->Disconnect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditorCtrl::OnMouseSelectionEnd),NULL, this);
	text_ctrl->Disconnect( wxEVT_LEFT_UP,	wxMouseEventHandler(HexEditorCtrl::OnMouseSelectionEnd),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_RIGHT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseRight),NULL, this);
	text_ctrl->Disconnect( wxEVT_RIGHT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseRight),NULL, this);
	hex_ctrl ->Disconnect( wxEVT_MOTION,	wxMouseEventHandler(HexEditorCtrl::OnMouseMove),NULL, this);
	text_ctrl->Disconnect( wxEVT_MOTION,	wxMouseEventHandler(HexEditorCtrl::OnMouseMove),NULL, this);
	}

//-----READ/WRITE FUNCTIONS-------//

void HexEditorCtrl::ReadFromBuffer( uint64_t position, unsigned lenght, char *buffer, bool cursor_reset, bool paint ){
	static wxMutex MyBufferMutex;
	MyBufferMutex.Lock();
	page_offset = position;
	if( lenght != ByteCapacity() ){
		//last line could be NULL;
		}
	Clear( false, cursor_reset );
	wxString text_string;
// Optimized Code
	for( unsigned i=0 ; i<lenght ; i++ ){
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
	hex_ctrl->Replace(char_location, char_location+2, temp);
	}

void HexEditorCtrl::HexCharReplace(long hex_location, const wxChar chr){
	hex_ctrl->Replace( hex_location, chr, true );
	hex_location /=2;	// Hex location is now Byte location
	char rdchr = hex_ctrl->ReadByte(hex_location);
	text_ctrl->Replace(	hex_location, rdchr, true );
	}
//-----MENUS--------//

void HexEditorCtrl::ShowContextMenu( const wxMouseEvent& event ){
	wxMenu menu;
	unsigned TagPosition=0;
	if( event.GetEventObject() == hex_ctrl )
		TagPosition = page_offset + (hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) / 2);
	if( event.GetEventObject() == text_ctrl )
		TagPosition = page_offset + text_ctrl->PixelCoordToInternalPosition( event.GetPosition() );

	TagElement *TAG;
	for( unsigned i = 0 ; i < MainTagArray.Count() ; i++ ){
		TAG = MainTagArray.Item(i);
		if( TAG->isCover( TagPosition ) ){	//end not included!
			menu.Append(idTagEdit, _T("Tag Edit"));
			break;
			}
		}

	if( select->IsState( select->SELECT_END ) ){
		menu.Append(idTagAddSelection, _T("Tag Selection"));
		}
//  menu.AppendSeparator();
	wxPoint pos = event.GetPosition();
	wxWindow *scr = static_cast<wxWindow*>( event.GetEventObject() );
	pos += scr->GetPosition();
    PopupMenu(&menu, pos);
	}

//-----VISUAL FUNCTIONS------//
void HexEditorCtrl::SetFont( wxFont f ){
	stdfont = f;
	m_static_offset->SetFont( f );
	m_static_adress->SetFont( f );
	m_static_byteview->SetFont( f );

	wxTextAttr Style( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ),
					wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ),
					f);

	offset_ctrl->SetSelectionStyle( Style );
	hex_ctrl->SetSelectionStyle( Style );
	text_ctrl->SetSelectionStyle( Style );

	Style = wxTextAttr( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ),
										wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ),
										f);

   offset_ctrl->SetDefaultStyle( Style );
   hex_ctrl->SetDefaultStyle( Style );
   text_ctrl->SetDefaultStyle( Style );

	}

bool HexEditorCtrl::Selector(){
	if( FindFocus() == hex_ctrl || FindFocus() == text_ctrl )				//If selecton from hex or text control
		select->EndOffset = page_offset + GetLocalHexInsertionPoint()/2;	//Than make selection
	else{
		std::cout << "Selector without focuse captured" << std::endl;
		return false;
		}

	if( not select->IsState( select->SELECT_TRUE ) ){	// If no select available,
		select->SetState( select->SELECT_TRUE );			// then set start selection procedure
		select->StartOffset = select->EndOffset;
		}
	return true;
	}

bool HexEditorCtrl::Select ( uint64_t start_offset, uint64_t end_offset ){
	if( start_offset < 0 || end_offset < 0
//	|| start_offset > myfile->Length() ||  end_offset > myfile->Length() //??
		){
		wxBell();
		return false;
		}
	select->SetState( select->SELECT_END );
	select->StartOffset = start_offset;
	select->EndOffset  = end_offset;
	PaintSelection( );
	return true;
	}

void inline HexEditorCtrl::ClearPaint( void ){
	hex_ctrl ->ClearSelection();
	text_ctrl->ClearSelection();
	}

void HexEditorCtrl::PreparePaintTAGs( void ){//TagElement& TAG ){
	TagHideAll();
	hex_ctrl->TagArray.Clear();
	text_ctrl->TagArray.Clear();

	MainTagArray.Sort( TagElement::TagCompare );
	TagElement *TAG;
	for( unsigned i = 0 ; i < MainTagArray.Count() ; i ++ ){	//Painting all TAGs here.
		TAG = MainTagArray.Item(i);// For debugging
		PushTAGToControls(TAG);
		}

	HighlightArray.Sort( TagElement::TagCompare );
	for( unsigned i = 0 ; i < HighlightArray.Count() ; i ++ ){	//Just highlighting required sections.
		TAG = HighlightArray.Item(i);// For debugging
		PushTAGToControls(TAG);
		}
	}

//This functions move tags to local hex and text ctrls.
void HexEditorCtrl::PushTAGToControls( TagElement* TAG){
	int64_t start_byte = TAG->start;
	int64_t end_byte = TAG->end;

	if(start_byte > end_byte){							// swap if start > end
		int64_t temp = start_byte;
		start_byte = end_byte;
		end_byte = temp;
		}

	if( start_byte >= page_offset + GetByteCount() )	// ...[..].TAG...
		return;
	else if( end_byte < page_offset )					// ..TAG..[...]...
		return;

	if( start_byte <= page_offset )						// ...TA[G..]....
		start_byte = page_offset;

	if( end_byte >= page_offset + GetByteCount() )		//...[..T]AG...
		end_byte = GetByteCount() + page_offset;

	start_byte	-= page_offset;
	end_byte		-= page_offset;

	TagElement *TAX;//For debugging
	TAX = new TagElement( start_byte, end_byte+1, TAG->tag, TAG->FontClrData, TAG->NoteClrData );
	text_ctrl->TagArray.Add( TAX );
	TAX = new TagElement( start_byte*2, (end_byte+1)*2, TAG->tag, TAG->FontClrData, TAG->NoteClrData );
	hex_ctrl->TagArray.Add( TAX );
	}

void HexEditorCtrl::PaintSelection( void ){
	PreparePaintTAGs();
	if( !select->IsState( select->SELECT_FALSE ) ){
		int64_t start_byte = select->StartOffset;
		int64_t end_byte = select->EndOffset;

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

		start_byte -= page_offset;
		end_byte   -= page_offset;

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
	int charx = hex_ctrl->GetCharSize().GetX();
	int offset_x = offset_ctrl->GetCharSize().GetX()*12 + 4;
    x -= offset_x;			//Remove Offset Control box X because its changeable
    x -= offset_scroll_real->GetSize().GetX();		//Remove Offset scroll size
    x -= 4*2;									//+x 4 pixel external borders (dark ones, 2 pix each size)
    y -= m_static_byteview->GetSize().GetY();	//Remove Head Text Y
	int i = x/(4*charx);	//2 hex + 1 whitespace & 1 byte view
	int text_x = charx*i +2 +4;// +2 for internal space pixel and +4 for external ones, 4 reduced at around SetSize function.
	int hex_x = charx*3*i +2 +4 - charx; //no need for last gap

#ifdef _DEBUG_SIZE_
	std::cout<< "HexEditorCtrl::OnResize()" << std::endl
			<< "HexEditorCtrl SizeEvent ReSize Command=(" << event.GetSize().GetX() << ',' << event.GetSize().GetY() << ")\n"
			<< "Offset Scrll: \t(" << offset_scroll->GetSize().GetX() << ',' << event.GetSize().GetY() <<")\n"
			<< "Offset Ctrl: \t(" << offset_ctrl->GetSize().GetX() << ',' << event.GetSize().GetY() <<")\n"
			<< "Hex Ctrl: \t(" << hex_x << ',' << event.GetSize().GetY() << ")\n"
			<< "Text Ctrl: \t(" << text_x << ',' << event.GetSize().GetY() << ")\n"
			<< "Hex Char: " << charx << std::endl;
#endif
	wxString adress,byteview;
	for( int j = 0 ; j < i ; j++ ){
		adress << wxString::Format( wxT("%02X "), j );
		byteview << wxString::Format( wxT("%01X"), j%16 );
		}
	adress.RemoveLast();	//Remove last ' ' for unwrap
 	m_static_adress->SetLabel(adress);
 	m_static_byteview->SetLabel( byteview );

	offset_ctrl->SetMinSize( wxSize( offset_x , y ) );
//	offset_ctrl->SetSize( wxSize( offset_x , y ) ); //Not needed, Layout() Makes the job well.

	hex_ctrl->SetMinSize( wxSize(hex_x, y ));
//	hex_ctrl->SetSize( wxSize( hex_x, y ));

	text_ctrl->SetMinSize( wxSize( text_x, y ));
//	text_ctrl->SetSize( wxSize( text_x, y ));

	m_static_offset->SetMinSize( wxSize(offset_x, m_static_offset->GetSize().GetY()) );
	m_static_adress->SetMinSize( wxSize(hex_x, m_static_offset->GetSize().GetY()) ) ;
	m_static_byteview->SetMinSize( wxSize( text_x, m_static_offset->GetSize().GetY()) );

	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer( 2, 4, 0, 0 );
	fgSizer1->Add( m_static_offset, 0, wxALIGN_CENTER|wxLEFT, 5 );
	fgSizer1->Add( m_static_adress, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT, 2 );
	fgSizer1->Add( m_static_byteview, 0, wxALIGN_CENTER|wxALL, 0 );
	fgSizer1->Add( m_static_null, 0, wxALIGN_CENTER, 3 );
	fgSizer1->Add( offset_ctrl, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 0 );
	fgSizer1->Add( hex_ctrl, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 0 );
	fgSizer1->Add( text_ctrl, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 0 );
	fgSizer1->Add( offset_scroll_real, 0, wxEXPAND, 0 );

	this->SetSizer( fgSizer1 );
	this->Layout();

#ifdef __WXMSW__
///At windows, OnResize cannot update required things immeditialy, this hack fixes this behaviour.
	hex_ctrl->ChangeSize();
	text_ctrl->ChangeSize();
	offset_ctrl->ChangeSize();
#endif
//	offset_ctrl->BytePerLine = BytePerLine(); //Not needed, Updated via ReadFromBuffer

#ifdef _DEBUG_SIZE_
	std::cout<< "HexEditorCtrl After ReSize=(" << x << ',' << y << ")\n"
			<< "Offset Scrll: \t(" << offset_scroll->GetSize().GetX() << ',' << offset_scroll->GetSize().GetY()<<")\n"
			<< "Offset Ctrl: \t(" << offset_ctrl->GetSize().GetX() << ',' << offset_ctrl->GetSize().GetY()<<")\n"
			<< "Hex Ctrl: \t(" << hex_ctrl->GetSize().GetX() << ',' << hex_ctrl->GetSize().GetY()<<")\n"
			<< "Text Ctrl: \t(" << text_ctrl->GetSize().GetX() << ',' << text_ctrl->GetSize().GetY()<<")\n";
#endif
}

//------EVENTS---------//
void HexEditorCtrl::OnMouseLeft(wxMouseEvent& event){
	select->SetState( select->SELECT_FALSE );
	ClearPaint();
	if( event.GetEventObject() == hex_ctrl ){
		hex_ctrl->SetFocus();
		focus=HEX_CTRL;
		SetLocalHexInsertionPoint( hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) );
		}
	else if( event.GetEventObject() == text_ctrl ){
		text_ctrl->SetFocus();
		focus=TEXT_CTRL;
		SetLocalHexInsertionPoint( 2 * text_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) + 1);
		}
	else// if( event.GetEventObject() == offset_ctrl)
		{
		m_static_offset->SetLabel( offset_ctrl->hex_offset==true ? _("Offset: DEC") : _("Offset: HEX"));
		event.Skip(true);
		}
	}

void HexEditorCtrl::OnMouseMove( wxMouseEvent& event ){
	if(event.m_leftDown){									//if left button is pressed
		int new_hex_location;										// initialize new_hex_location variable
		if( event.GetEventObject() == hex_ctrl ) 		// if this event from hex_ctrl area
			new_hex_location = hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() ); //than take it's location on hex chars
		else if ( event.GetEventObject() == text_ctrl ) //if we got this event from text area
			new_hex_location = 2*(text_ctrl->PixelCoordToInternalPosition( event.GetPosition() )); //Than we needed to multiply with 2 for take it's hex location.
		int old_hex_location = GetLocalHexInsertionPoint();	//requesting old hex location
		if( new_hex_location != old_hex_location ){				//if hex selection addresses are different, start selection routine

			if( not select->IsState( select->SELECT_TRUE) )	//if this is new selection start
				if( Selector() == false )								//and  select without focus
					return;													//don't make anything.
			SetLocalHexInsertionPoint( new_hex_location );		//Moving cursor to new location.
			Selector();														//Making actual selection.
			PaintSelection();
			}
		}
	else{
		if( event.GetEventObject() == hex_ctrl or
			event.GetEventObject() == text_ctrl or
			event.GetEventObject() == offset_ctrl ){
			TagElement* tg = static_cast<wxHexCtrl*>(event.GetEventObject())->GetTagByPix( event.GetPosition() );
			if( (tg == NULL and TAGMutex==true) or 	 //If there is no Tag at under and tag mutex available
				(tg != NULL and not tg->visible) )		// or Changed to new tag
				TagHideAll();
			}
		event.Skip(); //enable tags but problems with paint?
		}
	}

void HexEditorCtrl::OnMouseSelectionEnd( wxMouseEvent& event ){
	if(select->IsState( select->SELECT_TRUE ) )
		select->SetState( select->SELECT_END );
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

void HexEditorCtrl::OnKillFocus( wxFocusEvent& event){
#ifdef _DEBUG_MOUSE_
	std::cout << "HexEditorCtrl::OnKillFocus( wxMouseEvent& event ) \n" ;
#endif
	TagHideAll();
	event.Skip();
	}

void HexEditorCtrl::OnTagAddSelection( wxCommandEvent& event ){
	if( select->IsState(select->SELECT_END) ){
		TagElement *TE = new TagElement;
		TE->start=select->StartOffset;
		TE->end=select->EndOffset;
		static wxColour last_tag_color = *wxRED;
		static wxColour last_font_color = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
		TE->NoteClrData.SetColour( last_tag_color );
		TE->FontClrData.SetColour( last_font_color );


		TagDialog *x=new TagDialog( *TE, this );

		if( x->ShowModal() == wxID_SAVE ){
			last_tag_color = TE->NoteClrData.GetColour();
			last_font_color = TE->FontClrData.GetColour();
			MainTagArray.Add( TE );
			PreparePaintTAGs();
			ClearPaint();
			text_ctrl->RePaint();
			hex_ctrl ->RePaint();
			}
		x->Destroy();
		delete x;
		//delete TE; Don't delete this tags!
		}
	//event.Skip( true );
	wxUpdateUIEvent eventx( TAG_CHANGE_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	}

void HexEditorCtrl::OnTagEdit( wxCommandEvent& event ){
	TagElement *TAG;
	uint64_t pos = LastRightClickAt;
#ifdef _DEBUG_TAG_
	std::cout << " Tag Edit on " << pos << std::endl;
#endif
	for( unsigned i = 0 ; i < MainTagArray.Count() ; i++ ){
		TAG = MainTagArray.Item(i);
		if( TAG->isCover(pos) ){
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
		}
	wxUpdateUIEvent eventx( TAG_CHANGE_EVENT );
	GetEventHandler()->ProcessEvent( eventx );
	}

void HexEditorCtrl::TagHideAll( void ){
	hex_ctrl->OnTagHideAll();
	text_ctrl->OnTagHideAll();
	TAGMutex = false;
	}

void HexEditorCtrl::LoadTAGS( wxFileName flnm ){
	wxXmlDocument doc;
	if( flnm.IsFileReadable() )
		if( doc.Load( flnm.GetFullPath(), wxT("UTF-8")) )
			if (doc.GetRoot()->GetName() == wxT("wxHexEditor_XML_TAG")){
				wxXmlNode *child = doc.GetRoot()->GetChildren();

				child = child->GetChildren();	//<filename> -> <TAG>

				while (child) {
					if (child->GetName() == wxT("TAG")) {
						wxString propvalue = child->GetPropVal(wxT("id"), wxT("default-value"));
	#ifdef _DEBUG_TAG_
						std::cout << "TAG ID:" << propvalue.ToAscii() << " readed.\n";
	#endif
						TagElement *tmp = new TagElement();
						long long unsigned xxl=0;
						for( wxXmlNode *element = child->GetChildren() ; element != NULL ; element = element->GetNext() ){
							if (element->GetName() == wxT("start_offset")){
							#ifdef __WXMSW__	//I don't knwo why but ToULongLong dowen't work on windows by mingw.
								xxl = atoll( element->GetNodeContent().ToAscii() );
							#else
								element->GetNodeContent().ToULongLong( &xxl, 10 );
							#endif
								tmp->start = xxl;
								}
							else if (element->GetName() == wxT("end_offset")){
							#ifdef __WXMSW__
								xxl = atoll( element->GetNodeContent().ToAscii() );
							#else
								element->GetNodeContent().ToULongLong( &xxl, 10 );
							#endif
								tmp->end = xxl;
								}
							else if (element->GetName() == wxT("tag_text"))
								tmp->tag = element->GetNodeContent();
							else if (element->GetName() == wxT("font_colour"))
								tmp->FontClrData.SetColour( wxColour(element->GetNodeContent()) );
							else if (element->GetName() == wxT("note_colour"))
								tmp->NoteClrData.SetColour( wxColour(element->GetNodeContent()) );
							}
					#ifdef _DEBUG_TAG_
						tmp->print();
					#endif
						MainTagArray.Add(tmp);
						}
					child = child->GetNext();
					}
				PreparePaintTAGs();
				ClearPaint();
				text_ctrl->RePaint();
				hex_ctrl ->RePaint();
				}
	}

//Moves tags for deletion & insertions
void HexEditorCtrl::MoveTAGS( uint64_t location, int64_t size ){
	for( unsigned i = 0 ; i < MainTagArray.Count() ; i++ ){
		TagElement *TAG = MainTagArray.Item(i);
		if( size < 0 and TAG->start >= location and TAG->start <= location+(-size) ){//Deletion, (-size) double negation indicates deletion range.
			MainTagArray.RemoveAt(i); //Deletion of code if start inside deletion selection.
			continue;
			}
		if( TAG->start >= location ){
			TAG->start += size;
			TAG->end += size;
			}
		}
	}

void HexEditorCtrl::SaveTAGS( wxFileName flnm ){
	if( MainTagArray.Count() ==  0){
		if( flnm.FileExists() )
			wxRemoveFile( flnm.GetFullPath() << wxT(".tags") );
		}
	else{
		wxXmlDocument doc;

		wxXmlNode *node_Root = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, wxT("wxHexEditor_XML_TAG"), wxEmptyString, NULL , NULL);

		wxXmlProperty *prop_filename = new wxXmlProperty( wxT("path"), flnm.GetFullPath(), NULL);
		wxXmlNode *node_File = new wxXmlNode( node_Root, wxXML_ELEMENT_NODE, wxT("filename"), flnm.GetFullPath(), prop_filename , NULL);

		for(unsigned i = 0 ; i < MainTagArray.Count() ; i++ ){
			TagElement *TAG = MainTagArray.Item(i);

			wxXmlProperty *ID = new wxXmlProperty( wxT("id"), wxString::Format(wxT("%d"),i), NULL );
			wxXmlNode *node_Tag = new wxXmlNode( node_File, wxXML_ELEMENT_NODE, wxT("TAG"), wxEmptyString, ID , NULL);

			wxXmlNode *element_NoteColour		= new wxXmlNode( node_Tag, wxXML_ELEMENT_NODE, wxT("note_colour"), wxEmptyString, NULL, NULL);
			new wxXmlNode( element_NoteColour, wxXML_TEXT_NODE, wxT("note_colour"), TAG->NoteClrData.GetColour().GetAsString(wxC2S_HTML_SYNTAX), NULL, NULL);

			wxXmlNode *element_FontColour		= new wxXmlNode( node_Tag, wxXML_ELEMENT_NODE, wxT("font_colour"), TAG->FontClrData.GetColour().GetAsString(wxC2S_HTML_SYNTAX), NULL, element_NoteColour);
			new wxXmlNode( element_FontColour, wxXML_TEXT_NODE, wxT("font_colour"), TAG->FontClrData.GetColour().GetAsString(wxC2S_HTML_SYNTAX), NULL, NULL);

			wxXmlNode *element_TagText		= new wxXmlNode( node_Tag, wxXML_ELEMENT_NODE, wxT("tag_text"), TAG->tag, NULL, element_FontColour);
			new wxXmlNode( element_TagText, wxXML_TEXT_NODE, wxT("tag_text"), TAG->tag, NULL, NULL);

			wxXmlNode *element_End 			= new wxXmlNode( node_Tag, wxXML_ELEMENT_NODE, wxT("end_offset"), wxString::Format(wxT("%ld"),TAG->end ) , NULL, element_TagText);
			new wxXmlNode( element_End, wxXML_TEXT_NODE, wxT("end_offset"), wxString::Format(wxT("%ld"),TAG->end ) , NULL, NULL);

			wxXmlNode *element_Start		= new wxXmlNode( node_Tag, wxXML_ELEMENT_NODE, wxT("start_offset"), wxString::Format(wxT("%ld"), TAG->start ), NULL, element_End);
			new wxXmlNode( element_Start, wxXML_TEXT_NODE, wxT("start_offset"), wxString::Format(wxT("%ld"), TAG->start ), NULL, NULL);
			}
		doc.SetFileEncoding( wxT("UTF-8") );
		doc.SetRoot( node_Root );
		doc.Save(flnm.GetFullPath().Append(wxT(".tags")));
		}
	}

void HexEditorCtrl::UpdateUI(wxUpdateUIEvent& event){
	std::cout << "wxHexEditorCtrl::OnUpdateUI()" << std::endl;
	m_static_offset->SetLabel( offset_ctrl->hex_offset==false ? _("Offset: DEC") : _("Offset: HEX"));
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

wxHugeScrollBar::wxHugeScrollBar( wxScrollBar* m_scrollbar_ ){
         m_range = m_thumb = 0;
         m_scrollbar = m_scrollbar_;

			m_scrollbar->Connect( wxEVT_SCROLL_TOP, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Connect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Connect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Connect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Connect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Connect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Connect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Connect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Connect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
         }

wxHugeScrollBar::~wxHugeScrollBar(){
			m_scrollbar->Disconnect( wxEVT_SCROLL_TOP, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Disconnect( wxEVT_SCROLL_BOTTOM, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Disconnect( wxEVT_SCROLL_LINEUP, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Disconnect( wxEVT_SCROLL_LINEDOWN, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Disconnect( wxEVT_SCROLL_PAGEUP, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Disconnect( wxEVT_SCROLL_PAGEDOWN, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Disconnect( wxEVT_SCROLL_THUMBTRACK, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Disconnect( wxEVT_SCROLL_THUMBRELEASE, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			m_scrollbar->Disconnect( wxEVT_SCROLL_CHANGED, wxScrollEventHandler( wxHugeScrollBar::OnOffsetScroll ), NULL, this );
			}

void wxHugeScrollBar::SetThumbPosition(int64_t setpos){
	std::cout << "SetThumbPosition()" << setpos << std::endl;
	m_thumb = setpos;
	if( m_range <= 2147483647){
		m_scrollbar->SetThumbPosition( setpos );
		}
	else{
		std::cout << "m_Range" << m_range << std::endl;
		std::cout << "SetThumbPositionx()" << static_cast<int>(setpos*(2147483648.0/m_range)) << std::endl;
		m_scrollbar->SetThumbPosition(  static_cast<int>(setpos*(2147483648.0/m_range)) );
		}
	}

void wxHugeScrollBar::SetScrollbar( int64_t Current_Position,int page_x, int64_t new_range, int pagesize, bool repaint ){
	m_range = new_range;
	if(new_range <= 2147483647){//if representable with 32 bit
		m_scrollbar->SetScrollbar( Current_Position, page_x, new_range, pagesize, repaint );
		}
	else{
		std::cout << "new_range " << new_range << std::endl;
		std::cout << "Current_Position :" << (Current_Position*(2147483647/new_range)) << std::endl;
		m_scrollbar->SetScrollbar( (Current_Position*(2147483647/new_range)), page_x, 2147483647, pagesize, repaint );
		}
	SetThumbPosition( Current_Position );
	}

void wxHugeScrollBar::OnOffsetScroll( wxScrollEvent& event ){
	if((event.GetEventType() == wxEVT_SCROLL_CHANGED) or (event.GetEventType() == wxEVT_SCROLL_THUMBTRACK)){
		if( m_range <= 2147483647){
			m_thumb = event.GetPosition();
			}
		else{	//64bit mode
			int64_t here =event.GetPosition();
			if(here == 2147483646)	//if maximum set
				m_thumb = m_range-1;	//than give maximum m_thumb which is -1 from range
			else
				m_thumb = static_cast<int64_t>(here*(m_range/2147483647.0));
			}
		wxYieldIfNeeded();
		}
	event.Skip();
	}
