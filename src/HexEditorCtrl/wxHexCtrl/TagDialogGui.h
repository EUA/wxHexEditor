///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  5 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __TAGDIALOGGUI_H__
#define __TAGDIALOGGUI_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/clrpicker.h>
#include <wx/sizer.h>
#include <wx/button.h>
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
		wxStaticText* m_staticText2;
		wxColourPickerCtrl* m_FontColourPicker;
		wxColourPickerCtrl* m_NoteColourPicker;
		wxButton* m_CancelButton;
		wxButton* m_RemoveButton;
		wxButton* m_SaveButton;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDelete( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		TagDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tag Dialog"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER ); 
		~TagDialogGui();
	
};

#endif //__TAGDIALOGGUI_H__
