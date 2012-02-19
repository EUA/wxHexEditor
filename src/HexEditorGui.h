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
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/radiobut.h>
#include <wx/radiobox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/statbmp.h>
#include <wx/hyperlink.h>
#include <wx/filepicker.h>
#include <wx/spinctrl.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define idClose 1000
#define idCopyAs 1001
#define idSaveAsDump 1002
#define idFillSelection 1003
#define idInsert 1004
#define idGotoOffset 1005
#define idInterpreter 1006
#define idToolbar 1007
#define idInfoPanel 1008
#define idTagPanel 1009
#define idDisassemblerPanel 1010
#define idZebraStriping 1011
#define idChecksum 1012
#define idCompare 1013
#define idXORView 1014
#define idDeviceRam 1015
#define idFileRO 1016
#define idFileRW 1017
#define idFileDW 1018
#define ID_CHK_UNSIGNED 1019
#define ID_CHK_BIGENDIAN 1020

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
		virtual void OnToolsMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDevicesMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnAbout( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		HexEditorGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("wxHexEditor Alpha Development version"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
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
/// Class DisassemblerPanelGUI
///////////////////////////////////////////////////////////////////////////////
class DisassemblerPanelGUI : public wxPanel 
{
	private:
	
	protected:
		wxChoice* m_choiceVendor;
		wxChoice* m_choiceASMType;
		wxChoice* m_choiceBitMode;
		wxTextCtrl* m_dasmCtrl;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnUpdate( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		DisassemblerPanelGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 273,310 ), long style = wxTAB_TRAVERSAL );
		~DisassemblerPanelGUI();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class GotoDialogGui
///////////////////////////////////////////////////////////////////////////////
class GotoDialogGui : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticTextOffset;
		wxChoice* m_choiceMode;
		wxComboBox* m_comboBoxOffset;
		wxRadioButton* m_dec;
		wxRadioButton* m_hex;
		wxRadioBox* m_branch;
		wxButton* m_button_go;
		
		// Virtual event handlers, overide them in your derived class
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnInput( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnGO( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnConvert( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnGo( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		GotoDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Go to Offset"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );
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
		wxButton* btnFindSomeBytes;
		wxStaticLine* m_staticline;
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
		wxSpinCtrl* spinStopCompare;
		wxStaticText* m_staticText20;
		wxCheckBox* checkMerge;
		wxSpinCtrl* spinMerge;
		wxStaticText* m_staticText201;
		wxCheckBox* checkSaveResults;
		wxFilePickerCtrl* filePickSave;
		wxButton* btnCancel;
		wxButton* btnCompare;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFileChange( wxFileDirPickerEvent& event ) { event.Skip(); }
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }
		virtual void filePickSaveOnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		CompareDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Compare Files"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~CompareDialogGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class ChecksumDialogGui
///////////////////////////////////////////////////////////////////////////////
class ChecksumDialogGui : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* txtFileToHash;
		wxCheckBox* chkFile;
		wxFilePickerCtrl* filePick;
		wxCheckBox* chkMD2;
		wxCheckBox* chkMD4;
		wxCheckBox* chkMD5;
		wxCheckBox* chkSHA1;
		wxCheckBox* chkSHA224;
		wxCheckBox* chkSHA256;
		wxCheckBox* chkSHA384;
		wxCheckBox* chkSHA512;
		wxCheckBox* chkRIPEMD128;
		wxCheckBox* chkRIPEMD160;
		wxCheckBox* chkRIPEMD256;
		wxCheckBox* chkRIPEMD320;
		wxCheckBox* chkHAVAL128;
		wxCheckBox* chkHAVAL160;
		wxCheckBox* chkHAVAL192;
		wxCheckBox* chkHAVAL224;
		wxCheckBox* chkHAVAL256;
		wxCheckBox* chkTIGER128;
		wxCheckBox* chkTIGER160;
		wxCheckBox* chkTIGER;
		wxCheckBox* chkADLER32;
		wxCheckBox* chkCRC32;
		wxCheckBox* chkCRC32B;
		wxCheckBox* chkWHIRLPOOL;
		wxCheckBox* chkGOST;
		wxCheckBox* chkSNEFRU128;
		wxCheckBox* chkSNEFRU256;
		wxButton* btnCancel;
		wxButton* btnCalculate;
		
		// Virtual event handlers, overide them in your derived class
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }
		virtual void filePickOnFileChanged( wxFileDirPickerEvent& event ) { event.Skip(); }
		
	
	public:
		
		ChecksumDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Calculate Checksum"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~ChecksumDialogGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class CopyAsDialogGui
///////////////////////////////////////////////////////////////////////////////
class CopyAsDialogGui : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* txtCopyAs;
		wxChoice* chcCopyAs;
		wxStaticText* txtOption;
		wxChoice* chcOption;
		wxCheckBox* chkBigEndian;
		wxSpinCtrl* spnBytePerLine;
		wxCheckBox* chkOffset;
		wxCheckBox* chkHex;
		wxCheckBox* chkText;
		wxButton* btnCancel;
		wxButton* btnCopy;
		
		// Virtual event handlers, overide them in your derived class
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		CopyAsDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Copy Selected Block As"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~CopyAsDialogGui();
	
};

///////////////////////////////////////////////////////////////////////////////
/// Class XORViewDialogGui
///////////////////////////////////////////////////////////////////////////////
class XORViewDialogGui : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* txtWarning;
		wxStaticText* txtSelection;
		wxRadioButton* radioASCII;
		wxRadioButton* radioHex;
		wxTextCtrl* XORtext;
		wxButton* btnCancel;
		wxButton* btnOK;
		
		// Virtual event handlers, overide them in your derived class
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		XORViewDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("XORView Thru Warning!"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~XORViewDialogGui();
	
};

#endif //__HexEditorGui__
