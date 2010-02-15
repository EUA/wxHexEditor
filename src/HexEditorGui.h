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
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/listbox.h>
#include <wx/radiobut.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/combobox.h>
#include <wx/statbox.h>
#include <wx/statbmp.h>
#include <wx/hyperlink.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define idClose 1000
#define idGotoOffset 1001
#define idInterpreter 1002
#define idToolbar 1003
#define idInfoPanel 1004
#define idFileRO 1005
#define idFileRW 1006
#define idFileDW 1007
#define ID_CHK_UNSIGNED 1008
#define ID_CHK_BIGENDIAN 1009

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
		wxMenu* optionsMenu;
		wxMenu* menuOptionsFileMode;
		wxMenu* helpMenu;
		wxStatusBar* statusBar;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ){ event.Skip(); }
		virtual void OnFileOpen( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileSave( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileSaveAs( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnFileClose( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditUndo( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditRedo( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditCopy( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditCut( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditPaste( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditFind( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditReplace( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnEditGoto( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnViewMenu( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnUpdateUI( wxUpdateUIEvent& event ){ event.Skip(); }
		virtual void OnOptionsFileMode( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		HexEditorGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("wxHexEditor AlPhA Development version"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 600,500 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~HexEditorGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class InterpreterGui
///////////////////////////////////////////////////////////////////////////////
class InterpreterGui : public wxPanel 
{
	private:
	
	protected:
		wxCheckBox* m_check_unsigned;
		wxCheckBox* m_check_bigendian;
		wxStaticText* m_static_bin;
		wxTextCtrl* m_textctrl_binary;
		wxStaticText* m_static_8bit;
		wxTextCtrl* m_textctrl_8bit;
		wxStaticText* m_static_16bit;
		wxTextCtrl* m_textctrl_16bit;
		wxStaticText* m_static_32bit;
		wxTextCtrl* m_textctrl_32bit;
		wxStaticText* m_static_64bit;
		wxTextCtrl* m_textctrl_64bit;
		wxStaticText* m_static_float;
		wxTextCtrl* m_textctrl_float;
		wxStaticText* m_static_double;
		wxTextCtrl* m_textctrl_double;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUpdate( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		InterpreterGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxPoint( -1,-1 ), const wxSize& size = wxSize( 174,218 ), long style = wxTAB_TRAVERSAL );
		~InterpreterGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class TagPanelGui
///////////////////////////////////////////////////////////////////////////////
class TagPanelGui : public wxPanel 
{
	private:
	
	protected:
		wxListBox* listTag;
	
	public:
		TagPanelGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~TagPanelGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class InfoPanelGui
///////////////////////////////////////////////////////////////////////////////
class InfoPanelGui : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_static_name;
		wxStaticText* m_name;
		wxStaticText* m_static_path;
		wxStaticText* m_path;
		wxStaticText* m_static_size;
		wxStaticText* m_size;
		wxStaticText* m_static_access;
		wxStaticText* m_access;
		wxStaticText* m_static_device;
		wxStaticText* m_device;
	
	public:
		InfoPanelGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 140,111 ), long style = wxTAB_TRAVERSAL );
		~InfoPanelGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GotoDialogGui
///////////////////////////////////////////////////////////////////////////////
class GotoDialogGui : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticTextOffset;
		wxTextCtrl* m_textCtrlOffset;
		wxRadioButton* m_dec;
		wxRadioButton* m_hex;
		wxRadioBox* m_branch;
		wxButton* m_button_go;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnInput( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnGo( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnConvert( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		GotoDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Goto Offset"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );
		~GotoDialogGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class FindDialogGui
///////////////////////////////////////////////////////////////////////////////
class FindDialogGui : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_static_search;
		wxComboBox* m_comboBoxSearch;
		wxStaticText* m_static_replace;
		wxComboBox* m_comboBoxReplace;
		wxButton* btnReplace;
		wxButton* btnReplaceAll;
		wxRadioBox* m_searchtype;
		wxCheckBox* chkMatchCase;
		wxCheckBox* chkSearchBackwards;
		wxCheckBox* chkWrapAround;
		wxButton* btnFind;
		wxButton* btnFindAll;
		
		// Virtual event handlers, overide them in your derived class
		virtual void EventHandler( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		FindDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );
		~FindDialogGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class UpdateDialog_Gui
///////////////////////////////////////////////////////////////////////////////
class UpdateDialog_Gui : public wxDialog 
{
	private:
	
	protected:
		wxStaticBitmap* wxbtmp_icon;
		wxStaticText* version_text;
		wxHyperlinkCtrl* m_hyperlink;
		wxCheckBox* wxchk_display;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChkDisplay( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		UpdateDialog_Gui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("New wxHexEditor Version!"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~UpdateDialog_Gui();
	
};

#endif //__HexEditorGui__
