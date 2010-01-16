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
	wxFont stdfont(wxFont(10, wxMODERN, wxNORMAL, wxNORMAL, 0, wxT("")));
	m_static_offset->SetFont( stdfont );
	m_static_offset->SetLabel( _("Offset: DEC") );
	m_static_adress->SetFont( stdfont );
	m_static_byteview->SetFont( stdfont );
	Dynamic_Connector();
	select.start_offset = select.end_offset = 0;
	select.state = xselect::SELECT_FALSE;
	TAGMutex = false;
	hex_ctrl->TagMutex = &TAGMutex;
	text_ctrl->TagMutex = &TAGMutex;
    }
HexEditorCtrl::~HexEditorCtrl( void ){
	Dynamic_Disconnector();
	Clear();
	MainTagArray.Clear();
	}

void HexEditorCtrl::Dynamic_Connector(){
	this->Connect( idTagSelect, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagSelection ), NULL, this );
	this->Connect( idTagEdit, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( HexEditorCtrl::OnTagEdit ), NULL, this );

	offset_ctrl	->Connect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
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

	offset_ctrl	->Disconnect( wxEVT_LEFT_DOWN,	wxMouseEventHandler(HexEditorCtrl::OnMouseLeft),NULL, this);
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

void HexEditorCtrl::ReadFromBuffer( int64_t position, unsigned lenght, char *buffer, bool cursor_reset, bool paint ){
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

	if( select.state == xselect::SELECT_END ){
		menu.Append(idTagSelect, _T("Tag Selection"));
		}
//  menu.AppendSeparator();
	wxPoint pos = event.GetPosition();
	wxWindow *scr = static_cast<wxWindow*>( event.GetEventObject() );
	pos += scr->GetPosition();
    PopupMenu(&menu, pos);
    // test for destroying items in popup menus
#if 0 // doesn't work in wxGTK!
    menu.Destroy(Menu_Popup_Submenu);
    PopupMenu( &menu, event.GetX(), event.GetY() );
#endif // 0
	}

//-----VISUAL FUNCTIONS------//

bool HexEditorCtrl::Selector(){
	static bool polarity_possitive;
	if( FindFocus() == hex_ctrl || FindFocus() == text_ctrl )
		select.end_offset = page_offset + GetLocalHexInsertionPoint()/2;
	else{
		std::cout << "Selector without focuse captured" << std::endl;
		return false;
		}

	bool first_selection = false;
	if( select.state != xselect::SELECT_TRUE ){			// If no select available,
		select.state = xselect::SELECT_TRUE;				// then select start procedure code
		select.start_offset	= select.end_offset;
		polarity_possitive		= false;
		first_selection			= true;
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
	select.state = xselect::SELECT_END;
	select.start_offset = start_offset;
	select.end_offset  = end_offset;
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
	TagHideAll();
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
	if( select.state != xselect::SELECT_FALSE ){
		int64_t start_byte = select.start_offset;
		int64_t end_byte = select.end_offset;

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
	int charx = hex_ctrl->GetCharSize().GetX();
	int offset_x = offset_ctrl->GetCharSize().GetX()*12 + 4;
    x -= offset_x;			//Remove Offset Control box X because its changeable
    x -= offset_scroll->GetSize().GetX();		//Remove Offset scroll size
    x -= 4*2;									//+x 4 pixel external borders (dark ones, 2 pix each size)
    y -= m_static_byteview->GetSize().GetY();	//Remove Head Text Y
	int i = x/(4*charx);	//2 hex + 1 whitespace & 1 byte view
	int text_x = charx*i +2 +4;// +2 for internal space pixel and +4 for external ones, 4 reduced at around SetSize function.
	int hex_x = charx*3*i +2 +4 - charx; //no need for last gap

#if defined(_DEBUG_) && _DEBUG_ > 1
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
	fgSizer1->Add( m_static_adress, 0, wxLEFT, 3 );
	fgSizer1->Add( m_static_byteview, 0, wxALIGN_CENTER|wxALL, 0 );
	fgSizer1->Add( m_static_null, 0, wxALIGN_CENTER, 3 );
	fgSizer1->Add( offset_ctrl, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 0 );
	fgSizer1->Add( hex_ctrl, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 0 );
	fgSizer1->Add( text_ctrl, 0, wxALIGN_CENTER|wxALL|wxEXPAND, 0 );
	fgSizer1->Add( offset_scroll, 0, wxEXPAND, 0 );

	this->SetSizer( fgSizer1 );
	this->Layout();

#ifdef __WXMSW__
///At windows, OnResize cannot update required things immeditialy, this hack fixes this behaviour.
	hex_ctrl->ChangeSize();
	text_ctrl->ChangeSize();
	offset_ctrl->ChangeSize();
#endif
//	offset_ctrl->BytePerLine = BytePerLine(); //Not needed, Updated via ReadFromBuffer

#if defined(_DEBUG_) && _DEBUG_ > 1
	std::cout<< "HexEditorCtrl After ReSize=(" << x << ',' << y << ")\n"
			<< "Offset Scrll: \t(" << offset_scroll->GetSize().GetX() << ',' << offset_scroll->GetSize().GetY()<<")\n"
			<< "Offset Ctrl: \t(" << offset_ctrl->GetSize().GetX() << ',' << offset_ctrl->GetSize().GetY()<<")\n"
			<< "Hex Ctrl: \t(" << hex_ctrl->GetSize().GetX() << ',' << hex_ctrl->GetSize().GetY()<<")\n"
			<< "Text Ctrl: \t(" << text_ctrl->GetSize().GetX() << ',' << text_ctrl->GetSize().GetY()<<")\n";
#endif
}

//------EVENTS---------//
void HexEditorCtrl::OnMouseLeft(wxMouseEvent& event){
	select.state = xselect::SELECT_FALSE;
	ClearPaint();
	if( event.GetEventObject() == hex_ctrl ){
		hex_ctrl->SetFocus();
		SetLocalHexInsertionPoint( hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) );
		}
	else if( event.GetEventObject() == text_ctrl ){
		text_ctrl->SetFocus();
		SetLocalHexInsertionPoint( 2 * text_ctrl->PixelCoordToInternalPosition( event.GetPosition() ) + 1);
		}
	else// if( event.GetEventObject() == offset_ctrl)
		{
		m_static_offset->SetLabel( offset_ctrl->hex_offset==true ? _("Offset: DEC") : _("Offset: HEX"));
		event.Skip(true);
		}
	}

void HexEditorCtrl::OnMouseMove( wxMouseEvent& event ){
	if(event.m_leftDown){
		int new_location=0;
		if( event.GetEventObject() == hex_ctrl )
			new_location = hex_ctrl->PixelCoordToInternalPosition( event.GetPosition() );
		else if ( FindFocus() == text_ctrl )
			new_location = 2*(text_ctrl->PixelCoordToInternalPosition( event.GetPosition() ));
		int old_location = GetLocalHexInsertionPoint();
		if( new_location != old_location ){
			if( select.state != xselect::SELECT_TRUE)		// At first select
				if( Selector() == false )						// select without focus.
					return;
			SetLocalHexInsertionPoint( new_location );
			Selector();
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
	if(select.state == xselect::SELECT_TRUE)
		select.state = xselect::SELECT_END;
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
	if( select.state == xselect::SELECT_END){
		TagElement *TE = new TagElement;
		TE->start=select.start_offset;
		TE->end=select.end_offset;
		static wxColour last_tag_color = *wxRED;
		TE->NoteClrData.SetColour( last_tag_color );

		TagDialog *x=new TagDialog( *TE, this );

		if( x->ShowModal() == wxID_SAVE ){
			last_tag_color = TE->NoteClrData.GetColour();
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
	uint64_t pos = LastRightClickAt;
#ifdef _DEBUG_
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
	#ifdef _DEBUG_
						std::cout << "TAG ID:" << propvalue.ToAscii() << " readed.\n";
	#endif
						TagElement *tmp = new TagElement();
						long long unsigned xxl;
						for( wxXmlNode *element = child->GetChildren() ; element != NULL ; element = element->GetNext() ){
							if (element->GetName() == wxT("start_offset")){
								element->GetNodeContent().ToULongLong( &xxl, 10 );;
								tmp->start = xxl;
								}
							else if (element->GetName() == wxT("end_offset")){
								element->GetNodeContent().ToULongLong( &xxl, 10 );;
								tmp->end = xxl;
								}
							else if (element->GetName() == wxT("tag_text"))
								tmp->tag = element->GetNodeContent();
							else if (element->GetName() == wxT("font_colour"))
								tmp->FontClrData.SetColour( wxColour(element->GetNodeContent()) );
							else if (element->GetName() == wxT("note_colour"))
								tmp->NoteClrData.SetColour( wxColour(element->GetNodeContent()) );
							}
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
