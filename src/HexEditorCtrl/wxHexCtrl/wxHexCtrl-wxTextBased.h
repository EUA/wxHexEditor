#ifndef wxHEX_CTRL_H
#define wxHEX_CTRL_H

#include <wx/textctrl.h>
#include <wx/caret.h>
#include <wx/wx.h>

class wxHexCtrl : public wxTextCtrl{
	public:
		wxHexCtrl():wxTextCtrl(){};
		wxHexCtrl(wxWindow *parent,
							wxWindowID id,
							const wxString &value = wxEmptyString,
							const wxPoint &pos = wxDefaultPosition,
							const wxSize &size = wxDefaultSize,
							long style = 0,
							const wxValidator& validator = wxDefaultValidator,
							const wxString &name = wxTextCtrlNameStr):
							wxTextCtrl(parent,id,value,pos,size,style,validator,name)
							{};
		int byte_per_line(  bool update =false );
		int max_line_count(  bool update =false );
		int view_byte_capacity( void );
		int get_byte_count( void );
		int active_line( void );
		void WriteBytes( char* buffer, long byte_count );
		long ReadBytes( char* buffer, long start_location, long byte_count );
		char ReadByte(long byte_location);
		char* HexToChar(const wxString& HexValue);
		void keyboard_input( wxKeyEvent& event );

		long GetInsertionPoint();
		wxTextPos GetLastPosition();
		void SetInsertionPoint(long pos);
		void Replace(long from, long to, const wxString& value);
		void Replace(long hex_location, const wxChar& value);
		void WriteHexValue( const wxChar& hex_value );

	protected:
		struct selector{
			bool selected;
			int64_t start;
			int64_t end;
			int match;
			} selection;

	private:
		DECLARE_EVENT_TABLE();
		void OnResize( wxSizeEvent &event );
	};
#endif
