///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __HexEditorGui__
#define __HexEditorGui__

#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

#define idMenuQuit 1000
#define idMenuAbout 1001

///////////////////////////////////////////////////////////////////////////////
/// Class HexEditorGui
///////////////////////////////////////////////////////////////////////////////
class HexEditorGui : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* mbar;
		wxMenu* fileMenu;
		wxMenu* editMenu;
		wxMenu* viewMenu;
		wxMenu* helpMenu;
		wxStatusBar* statusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnFileOpen( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileClose( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		HexEditorGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("wxWidgets Application Template"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 481,466 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~HexEditorGui();
	
};

#endif //__HexEditorGui__
