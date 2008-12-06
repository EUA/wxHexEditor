///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __TagDialogGui__
#define __TagDialogGui__

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class TagDialogGui
///////////////////////////////////////////////////////////////////////////////
class TagDialogGui : public wxDialog 
{
	private:
	
	protected:
		wxTextCtrl* TagTextCtrl;
		wxStaticText* m_staticText1;
		wxBitmapButton* FontBitmapButton;
		wxStaticText* m_staticText2;
		wxBitmapButton* NoteBitmapButton;
		wxButton* m_button1;
		wxButton* m_button2;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFontColor( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnNoteColor( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnDelete( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		TagDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tag Dialog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 395,185 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~TagDialogGui();
	
};

#endif //__TagDialogGui__
