///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Sep  8 2010)
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
#include <wx/aui/auibook.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/checkbox.h>
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
#include <wx/statline.h>
#include <wx/statbmp.h>
#include <wx/hyperlink.h>
#include <wx/filepicker.h>
#include <wx/spinctrl.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define idClose 1000
#define idInsert 1001
#define idGotoOffset 1002
#define idInterpreter 1003
#define idToolbar 1004
#define idInfoPanel 1005
#define idTagPanel 1006
#define idCompare 1007
#define idXORView 1008
#define idDeviceRam 1009
#define idFileRO 1010
#define idFileRW 1011
#define idFileDW 1012
#define ID_CHK_UNSIGNED 1013
#define ID_CHK_BIGENDIAN 1014

///////////////////////////////////////////////////////////////////////////////
/// Class HexEditorGui
///////////////////////////////////////////////////////////////////////////////
class HexEditorGui : public wxFrame 
{
	private:
	
	protected:
		wxMenuBar* mbar;
		wxMenu* fileMenu;
		wxMenu* menuFileOpenRecent;
		wxMenu* editMenu;
		wxMenu* viewMenu;
		wxMenu* toolsMenu;
		wxMenu* devicesMenu;
		wxMenu* menuDeviceDisk;
		wxMenu* optionsMenu;
		wxMenu* menuOptionsFileMode;
		wxMenu* helpMenu;
		wxStatusBar* statusBar;
		wxAuiNotebook* MyNotebook;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnMenuEvent( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnQuit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnViewMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnToolMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnToolsMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDevicesMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		HexEditorGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("wxHexEditor Alpha Development version"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 800,500 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
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
		wxCheckBox* m_check_edit;
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
		virtual void OnUpdate( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTextEdit( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnTextMouse( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnCheckEdit( wxCommandEvent& event ) { event.Skip(); }
		
	
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
		wxListBox* TagPanelList;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTagSelect( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		TagPanelGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL );
		~TagPanelGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class InfoPanelGui
///////////////////////////////////////////////////////////////////////////////
class InfoPanelGui : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_InfoPanelText;
	
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
		virtual void OnInput( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnGo( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnConvert( wxCommandEvent& event ) { event.Skip(); }
		
	
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
		wxRadioBox* m_searchtype;
		wxCheckBox* chkMatchCase;
		wxCheckBox* chkSearchBackwards;
		wxCheckBox* chkWrapAround;
		wxCheckBox* chkUTF8;
		wxButton* btnFind;
		wxButton* btnFindAll;
		wxStaticLine* m_staticline1;
		wxButton* btnReplace;
		wxButton* btnReplaceAll;
		
		// Virtual event handlers, overide them in your derived class
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		FindDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );
		~FindDialogGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class UpdateDialogGui
///////////////////////////////////////////////////////////////////////////////
class UpdateDialogGui : public wxDialog 
{
	private:
	
	protected:
		wxStaticBitmap* wxbtmp_icon;
		wxStaticText* version_text;
		wxHyperlinkCtrl* m_hyperlink;
		wxCheckBox* wxchk_display;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnChkDisplay( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		UpdateDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("New wxHexEditor Version!"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~UpdateDialogGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CompareDialogGui
///////////////////////////////////////////////////////////////////////////////
class CompareDialogGui : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_TextFile1;
		wxFilePickerCtrl* filePick1;
		wxStaticText* m_TextFile2;
		wxFilePickerCtrl* filePick2;
		wxRadioButton* m_radioDifferent;
		wxRadioButton* m_radioSame;
		wxCheckBox* checkStopCompare;
		wxSpinCtrl* m_spinCtrl2;
		wxStaticText* m_staticText20;
		wxCheckBox* checkSaveResults;
		wxFilePickerCtrl* filePickSave;
		wxButton* btnCancel;
		wxButton* btnCompare;
		
		// Virtual event handlers, overide them in your derived class
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CompareDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Compare Files"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~CompareDialogGui();
	
};

#endif //__HexEditorGui__
