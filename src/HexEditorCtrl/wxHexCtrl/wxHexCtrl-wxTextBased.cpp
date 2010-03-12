#include "wxHexCtrl.h"
//Connect( wxEVT_CHAR,wxKeyEventHandler(wxHexCtrl::keyboard_input),NULL, this);
BEGIN_EVENT_TABLE(wxHexCtrl,wxTextCtrl)
	EVT_CHAR( wxHexCtrl::keyboard_input )
	EVT_SIZE( wxHexCtrl::OnResize )
END_EVENT_TABLE()

int wxHexCtrl::byte_per_line( bool update ){
	static int bpl=0;		//Static integer for performance
	if(bpl == 0 || update){
		int x=0,y=0,descent=0;
		wxString anew(_("a"));
		GetTextExtent(anew,&x,&y,&descent);
		bpl = (GetSize().GetWidth()-descent)/(x*3); // x*3 for each hex takes 2 byte+space
		}
	return bpl;
    }

int wxHexCtrl::view_byte_capacity( void ){
    return byte_per_line()*max_line_count();
    }

int wxHexCtrl::get_byte_count( void ){
    return wxTextCtrl::GetLastPosition()/3 + ((wxTextCtrl::GetLastPosition()%3)?(1):(0)) ;
    }

int wxHexCtrl::max_line_count( bool update ){
	static int mlc=0;
	if(mlc == 0 || update)
		{
		int x=0,y=0,descent=0;
		wxString anew(_("a"));
		GetTextExtent(anew,&x,&y,&descent);
		mlc = (GetSize().GetHeight()-descent)/y;	//to detect current available line number
		}
	return mlc;
	}

int wxHexCtrl::active_line( void ){	//Starts from 1 ,NOT 0, and go to n.
    return (GetInsertionPoint()/(byte_per_line()*2))+1;
    }

long wxHexCtrl::GetInsertionPoint(){
	long gip = wxTextCtrl::GetInsertionPoint();
	return ((gip/3)*2 + gip%3);
	}

wxTextPos wxHexCtrl::GetLastPosition(){
	wxTextPos glp = GetValue().Trim().Length();
	return ((glp/3)*2 + glp%3);
	}

void wxHexCtrl::SetInsertionPoint(long pos){
	wxTextCtrl::SetInsertionPoint( pos*3/2 );
	}

void wxHexCtrl::Replace(long hexlocation, const wxChar& value){
	Replace(hexlocation, hexlocation+1, value);
	//wxLogError(hex_buffer);
	}

void wxHexCtrl::Replace(long from, long to, const wxString& value){
  //  wxCHECK_RET( m_text != NULL, wxT("invalid text ctrl") );
// TODO (death#4#): IsHEX?
	if (!value.empty()){
		SetInsertionPoint( from );
		for(unsigned int i=0; i<value.Length();i++)
			if(GetInsertionPoint()%2 == 0 && GetInsertionPoint()!=0){
				if(wxTextCtrl::GetLastPosition() == wxTextCtrl::GetInsertionPoint()){
					//"Write" new byte code is here
					if( wxTextCtrl::GetInsertionPoint() != (from*3/2) )
						wxTextCtrl::WriteText(wxT(" "));
					wxTextCtrl::WriteText( value[i] );
					if( value.Length() == 1){	//Adds zero to hex's second location.
						wxTextCtrl::WriteText( wxT("0") );
						wxTextCtrl::SetInsertionPoint(wxTextCtrl::GetInsertionPoint()-1);
						}
					}
				else{/**We don't use replace function at wxTextCtrl because it uses "virtual" internal functions*/
					wxTextCtrl::Remove(wxTextCtrl::GetInsertionPoint(),wxTextCtrl::GetInsertionPoint()+1);
					wxTextCtrl::WriteText( value[i] );
					}
				}
			else{
				wxTextCtrl::Remove(wxTextCtrl::GetInsertionPoint(),wxTextCtrl::GetInsertionPoint()+1);
				wxTextCtrl::WriteText( value[i] );
				}

		SetInsertionPoint(GetInsertionPoint());	//For alignin cursor to proper location.
		}
	}

void wxHexCtrl::WriteHexValue( const wxChar& hex_value ){
/*	if(GetInsertionPoint()!=GetLastPosition())	// I belive using this with 0<->F Alphabet, no additional check here
		hex_buffer.GetWritableChar(GetInsertionPoint()) = hex_value;
	else
		hex_buffer.Append(hex_value);

	if((wxTextCtrl::GetInsertionPoint()+1)%3 == 0)
		if(wxTextCtrl::GetLastPosition() == wxTextCtrl::GetInsertionPoint())
			wxTextCtrl::WriteText(wxT(" "));
		else
			wxTextCtrl::Replace(wxTextCtrl::GetInsertionPoint()+1,wxTextCtrl::GetInsertionPoint()+2,hex_value);
	else
		Replace(wxTextCtrl::GetInsertionPoint(),wxTextCtrl::GetInsertionPoint()+1,hex_value);

	wxYield();
	*/
	}

void wxHexCtrl::WriteBytes(char* buffer, long byte_count){
//	hex_buffer.Empty();	//hex_buffer.Clear();
	wxString hex_string;

	for( int i=0 ; i<byte_count ; i++ )
		hex_string << wxString::Format(wxT("%02X "),static_cast<unsigned char>(buffer[i]));
	//ChangeValue(hex_string);
	WriteText(hex_string);
	}

char wxHexCtrl::ReadByte(long byte_location){
	wxString hex_string = GetRange((byte_location)*3,(byte_location+1)*3);
//	wxLogError(wxString(_("Probing String: \"")) << hex_string << _("\""));
//	wxLogError(wxString::Format(_("Lucky number is: %02X"),(unsigned char*)HexToChar(hex_string)[0]));
	return HexToChar(hex_string)[0];
	}

/*
long wxHexCtrl::ReadBytes(unsigned char* buffer,long start_location, long byte_count){
	wxString hex_string = GetRange(start_location*3,(start_location+byte_count)*3);
//	Purify(hex_string);
//ALPHA oglu APLHA CODE
	return i;
    }
*/

char* wxHexCtrl::HexToChar(const wxString& HexValue){
	char *buffer = new char[HexValue.Length()/3+2];//+1 for "\0"

	for(unsigned int i=0,j=0 ; j < HexValue.Length()/3+1 ; i+=2,j++){
		if(HexValue[i]>='0' && HexValue[i]<='9')
			buffer[j] = HexValue[i] - '0';
		else if(HexValue[i]>='A' && HexValue[i]<='F')
			buffer[j] = HexValue[i] - 'A' + 10;
		else if(HexValue[i]>='a' && HexValue[i]<='f')
			buffer[j] = HexValue[i] - 'a' + 10;
		else if(HexValue[i] == ' '){	//Removes space char
			i--;
			continue;
			}
		else{
			wxBell();
			return NULL;
			}

		buffer[j] <<= 4;

		if(HexValue[i+1]>='0' && HexValue[i+1]<='9')
			buffer[j] |= HexValue[i+1] - '0';
		else if(HexValue[i+1]>='A' && HexValue[i+1]<='F')
			buffer[j] |= HexValue[i+1] - 'A' + 10;
		else if(HexValue[i+1]>='a' && HexValue[i+1]<='f')
			buffer[j] |= HexValue[i+1] - 'a'+ 10;
		else		// There is no space char remove code here, Hex codes must be sticked together.
			{
			wxBell();
			return NULL;
			}
		buffer[j+1]='\0';
		}
	return buffer;
	}

void wxHexCtrl::keyboard_input( wxKeyEvent& event ){
	wxChar chr = event.GetKeyCode();
	if( isxdigit(chr) ){
// TODO (death#1#): if text selected, enter from beggining!
// TODO (death#2#): If text Selected, than  remove selection first?
		selection.selected=false;
		//paint_selection();
		(chr>='a'&&chr<='z')?(chr-=('a'-'A')):(chr=chr);	//Upper() for Char
		if((GetInsertionPoint()+1)%3 == 0){
			if(GetLastPosition() == GetInsertionPoint())
				WriteText(wxT(" "));
			else{
				SetInsertionPoint((GetInsertionPoint()+1));
				Replace(GetInsertionPoint(),GetInsertionPoint()+1,chr);
				}
			}
		else{
			Replace(GetInsertionPoint(),GetInsertionPoint()+1,chr);
			}
		if((GetInsertionPoint()-2)%3 == 0 ) //cosmetic hex space jumper - Forward
			SetInsertionPoint(GetInsertionPoint()+1);
			}
	else{
		if(event.ShiftDown()){	//Selection from keyboard code
/*
			if(
				event.GetKeyCode()==WXK_UP || event.GetKeyCode()==WXK_NUMPAD_UP ||
				event.GetKeyCode()==WXK_DOWN || event.GetKeyCode()==WXK_NUMPAD_DOWN ||
				event.GetKeyCode()==WXK_LEFT || event.GetKeyCode()==WXK_NUMPAD_LEFT ||
				event.GetKeyCode()==WXK_RIGHT || event.GetKeyCode()==WXK_NUMPAD_RIGHT ||
				event.GetKeyCode()==WXK_HOME || event.GetKeyCode()==WXK_NUMPAD_HOME ||
				event.GetKeyCode()==WXK_END || event.GetKeyCode()==WXK_NUMPAD_END ||
				event.GetKeyCode()==WXK_PAGEUP || event.GetKeyCode()==WXK_NUMPAD_PAGEUP ||
				event.GetKeyCode()==WXK_PAGEDOWN || event.GetKeyCode()==WXK_NUMPAD_PAGEDOWN
				)
				hex_selector(event);	//Selection Starter call
*/
			}
		else
			selection.selected=false;

		switch (event.GetKeyCode()){
			case (WXK_UP):case (WXK_NUMPAD_UP):
			case (WXK_DOWN):case (WXK_NUMPAD_DOWN):
			case (WXK_HOME):case(WXK_NUMPAD_HOME):
			case (WXK_PAGEUP):case (WXK_NUMPAD_PAGEUP):
			case (WXK_PAGEDOWN):case (WXK_NUMPAD_PAGEDOWN):
				event.Skip(true);
				break;
			case (WXK_END):case(WXK_NUMPAD_END):
				// TODO (death#7#): Dangerous Code
				event.Skip(true);
				//SetInsertionPoint(active_line()*byte_per_line()*3 - 1);
				break;
			case (WXK_LEFT):case (WXK_NUMPAD_LEFT):
				if( GetInsertionPoint()==0 )
					wxBell();
				else
					if(GetInsertionPoint()%3 == 0 && GetInsertionPoint() !=0) //Cosmetic hex space jumper - Backward
						SetInsertionPoint(GetInsertionPoint()-2);
					else
						SetInsertionPoint(GetInsertionPoint()-1);
				break;
			case (WXK_RIGHT):case (WXK_NUMPAD_RIGHT):
				if( GetInsertionPoint() >= GetLastPosition()-2){
					wxBell();
					}
				else
					if((GetInsertionPoint()-1)%3 == 0 ) //cosmetic hex space jumper - Forward
						SetInsertionPoint(GetInsertionPoint()+2);
					else
						SetInsertionPoint(GetInsertionPoint()+1);
				break;
				event.Skip(true);
			case (WXK_DELETE):case (WXK_NUMPAD_DELETE):{
				if((GetInsertionPoint()+1)%3 != 0 ){
					Replace(GetInsertionPoint(),GetInsertionPoint()+1,wxT("0"));
					SetInsertionPoint(GetInsertionPoint()-1);
					wxBell();
				}
				else
					wxBell();
			}
				break;
			case (WXK_BACK):{
				if( GetInsertionPoint()!=0 )
					if(GetInsertionPoint()%3 == 0){
						Replace(GetInsertionPoint()-2,GetInsertionPoint()-1,wxT("0"));
						SetInsertionPoint(GetInsertionPoint()-1);
						}
					else{
						Replace(GetInsertionPoint()-1,GetInsertionPoint(),wxT("0"));
						SetInsertionPoint(GetInsertionPoint()-1); //Cosmetic hex space jumper - Backward
						if(GetInsertionPoint()%3 == 0 && GetInsertionPoint() !=0)
							SetInsertionPoint(GetInsertionPoint()-1);
						}
				else
					wxBell();
				}
				break;
// TODO (death#3#): CTRL+X
			default:
				wxBell();
				break;
		}//switch end
	wxYield();
//	hex_selector(event);
//	paint_selection();
	}
}

void wxHexCtrl::OnResize( wxSizeEvent &event ){
		byte_per_line(true);
		max_line_count(true);
		}

