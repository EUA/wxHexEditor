///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.9.0 Dec 16 2019)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/statusbr.h>
#include <wx/aui/auibook.h>
#include <wx/sizer.h>
#include <wx/frame.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/spinctrl.h>
#include <wx/collpane.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/radiobut.h>
#include <wx/radiobox.h>
#include <wx/dialog.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/statbmp.h>
#include <wx/hyperlink.h>
#include <wx/filepicker.h>
#include <wx/clrpicker.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default
#define idClose 1000
#define idImportTAGs 1001
#define idExportTAGs 1002
#define idImportTemplate 1003
#define wxID_QUIT 1004
#define idCopyAs 1005
#define idSaveAsDump 1006
#define idFillSelection 1007
#define idInsert 1008
#define idGotoOffset 1009
#define idInterpreter 1010
#define idToolbar 1011
#define idInfoPanel 1012
#define idTagPanel 1013
#define idDisassemblerPanel 1014
#define idSearchPanel 1015
#define idComparePanel 1016
#define idZebraStriping 1017
#define idShowOffset 1018
#define idShowHex 1019
#define idShowText 1020
#define idChecksum 1021
#define idCompare 1022
#define idXORView 1023
#define idHex2Color 1024
#define idDeviceRam 1025
#define idDeviceMemorySPD 1026
#define idProcessRAM 1027
#define idDeviceBackup 1028
#define idDeviceRestore 1029
#define idDeviceErase 1030
#define idFileRO 1031
#define idFileRW 1032
#define idFileDW 1033
#define idDonate 1034
#define idWiki 1035
#define idBugReport 1036
#define wxNEW 1037
#define ID_CHK_UNSIGNED 1038
#define ID_CHK_BIGENDIAN 1039

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
		wxMenu* menuDeviceImage;
		wxMenu* optionsMenu;
		wxMenu* menuOptionsFileMode;
		wxMenu* helpMenu;
		wxAuiToolBar* m_auiToolBar;
		wxAuiToolBarItem* m_tool_NewFile;
		wxAuiToolBarItem* m_tool_Open;
		wxAuiToolBarItem* m_tool_Save;
		wxAuiToolBarItem* m_tool_SaveAs;
		wxAuiToolBarItem* m_tool_Close;
		wxAuiToolBarItem* m_tool_Find;
		wxAuiToolBarItem* m_tool_Replace;
		wxAuiToolBarItem* m_tool_GotoOffset;
		wxAuiToolBarItem* m_tool_Undo;
		wxAuiToolBarItem* m_tool_Redo;
		wxAuiToolBarItem* m_tool_Copy;
		wxAuiToolBarItem* m_tool_Paste;
		wxAuiToolBarItem* m_tool_Cut;
		wxAuiToolBarItem* m_tool_Delete;
		wxAuiToolBarItem* m_tool_insert;
		wxStatusBar* statusBar;
		wxAuiNotebook* MyNotebook;

		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnMenuEvent( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnViewMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnUpdateUI( wxUpdateUIEvent& event ) { event.Skip(); }
		virtual void OnToolsMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnDevicesMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOptionsMenu( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHelpMenu( wxCommandEvent& event ) { event.Skip(); }


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
		wxStaticText* m_static_ascii;
		wxTextCtrl* m_textctrl_ascii;
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
		wxCollapsiblePane* m_collapsiblePane_TimeMachine;
		wxCheckBox* m_checkBoxLocal;
		wxPanel* m_panel_time;
		wxStaticText* m_static_timeUTC;
		wxSpinCtrl* m_spinCtrl_timeUTC;
		wxStaticText* m_static_timeUnix;
		wxTextCtrl* m_textctrl_timeUnix;
		wxStaticText* m_static_timeUnix64;
		wxTextCtrl* m_textctrl_timeUnix64;
		wxStaticText* m_static_timeFAT;
		wxTextCtrl* m_textctrl_timeFAT;
		wxStaticText* m_static_timeNTFS;
		wxTextCtrl* m_textctrl_timeNTFS;
		wxStaticText* m_static_timeHFSp;
		wxTextCtrl* m_textctrl_timeHFSp;
		wxStaticText* m_static_timeAPFS;
		wxTextCtrl* m_textctrl_timeAPFS;
		wxCollapsiblePane* m_collapsiblePaneExFAT;
		wxStaticText* m_static_timeExFAT_Creation;
		wxTextCtrl* m_textctrl_timeExFAT_Creation;
		wxStaticText* m_static_timeExFAT_Modification;
		wxTextCtrl* m_textctrl_timeExFAT_Modification;
		wxStaticText* m_static_timeExFAT_Access;
		wxTextCtrl* m_textctrl_timeExFAT_Access;

		// Virtual event handlers, overide them in your derived class
		virtual void OnUpdate( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTextEdit( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnTextMouse( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnCheckEdit( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSpin( wxSpinEvent& event ) { event.Skip(); }


	public:

		InterpreterGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxPoint( -1,-1 ), const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
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
		wxButton* m_buttonClear;
		wxButton* m_buttonHide;
		wxButton* m_buttonTAG;

		// Virtual event handlers, overide them in your derived class
		virtual void OnKeyDown( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnTagSelect( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnRightMouse( wxMouseEvent& event ) { event.Skip(); }
		virtual void OnClear( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnHide( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTAG( wxCommandEvent& event ) { event.Skip(); }


	public:

		TagPanelGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
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

		InfoPanelGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 140,111 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
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

		DisassemblerPanelGUI( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 273,310 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );
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
		wxButton* m_button_prev;
		wxButton* m_button_next;
		wxRadioBox* m_branch;
		wxButton* m_button_go;

		// Virtual event handlers, overide them in your derived class
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnChar( wxKeyEvent& event ) { event.Skip(); }
		virtual void OnInput( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnGO( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPreviousSector( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnNextSector( wxCommandEvent& event ) { event.Skip(); }
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
		wxButton* btnClose;

		// Virtual event handlers, overide them in your derived class
		virtual void OnChar( wxKeyEvent& event ) { event.Skip(); }
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }


	public:

		FindDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Find"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
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
		wxStaticText* m_staticTextHits;
		wxCheckBox* checkMergeSection;
		wxSpinCtrl* spinMergeSection;
		wxStaticText* m_staticTextBytes;
		wxCheckBox* checkSaveResults;
		wxFilePickerCtrl* filePickSave;
		wxCheckBox* checkConnectFiles;
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
		virtual void OnFileChange( wxFileDirPickerEvent& event ) { event.Skip(); }


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

///////////////////////////////////////////////////////////////////////////////
/// Class PreferencesDialogGui
///////////////////////////////////////////////////////////////////////////////
class PreferencesDialogGui : public wxDialog
{
	private:

	protected:
		wxStaticText* txtBtn;
		wxColourPickerCtrl* clrPickerForeground;
		wxColourPickerCtrl* clrPickerBackground;
		wxColourPickerCtrl* clrPickerBackgroundZebra;
		wxStaticText* txtSelection;
		wxColourPickerCtrl* clrPickerSelectionForeground;
		wxColourPickerCtrl* clrPickerSelectionBackground;
		wxButton* btnResetColours;
		wxStaticText* txtLang;
		wxChoice* chcLang;
		wxStaticText* txtCharacterFamily;
		wxChoice* chcCharacterEncodingFamily;
		wxStaticText* txtCharacterEncoding;
		wxChoice* chcCharacterEncoding;
		wxStaticText* txtfontSize;
		wxSpinCtrl* spinFontSize;
		wxComboBox* comboCustomHexFormat;
		wxCheckBox* chkCustom;
		wxCheckBox* chkBytePerLineLimit;
		wxSpinCtrl* spinBytePerLine;
		wxStaticLine* m_staticline_format;
		wxCheckBox* chkFakeBlockLines;
		wxTextCtrl* textCtlFakeBlockSize;
		wxCheckBox* chkAutoShowTagPanel;
		wxCheckBox* chkPortable;
		wxCheckBox* chkSingleInstance;
		wxButton* BtnCancel;
		wxButton* BtnSave;

		// Virtual event handlers, overide them in your derived class
		virtual void EventHandler( wxColourPickerEvent& event ) { event.Skip(); }
		virtual void OnResetColours( wxCommandEvent& event ) { event.Skip(); }
		virtual void EventHandler( wxCommandEvent& event ) { event.Skip(); }
		virtual void SpinEventHandler( wxSpinEvent& event ) { event.Skip(); }
		virtual void EventHandler( wxSpinEvent& event ) { event.Skip(); }
		virtual void OnSave( wxCommandEvent& event ) { event.Skip(); }


	public:

		PreferencesDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION );
		~PreferencesDialogGui();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DeviceBackupDialogGui
///////////////////////////////////////////////////////////////////////////////
class DeviceBackupDialogGui : public wxDialog
{
	private:

	protected:
		wxStaticText* txtSource;
		wxChoice* chcPartition;
		wxStaticText* txtDestination;
		wxFilePickerCtrl* filePickBackup;
		wxButton* btnCancel;
		wxButton* btnBackup;

		// Virtual event handlers, overide them in your derived class
		virtual void OnBackup( wxCommandEvent& event ) { event.Skip(); }


	public:

		DeviceBackupDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Disk/Partition Backup Tool"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~DeviceBackupDialogGui();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DeviceRestoreDialogGui
///////////////////////////////////////////////////////////////////////////////
class DeviceRestoreDialogGui : public wxDialog
{
	private:

	protected:
		wxStaticText* txtSource;
		wxFilePickerCtrl* filePickBackup;
		wxStaticText* txtDestination;
		wxChoice* chcPartition;
		wxButton* btnCancel;
		wxButton* btnRestore;

		// Virtual event handlers, overide them in your derived class
		virtual void OnRestore( wxCommandEvent& event ) { event.Skip(); }


	public:

		DeviceRestoreDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Disk/Partition Restore Tool"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~DeviceRestoreDialogGui();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DeviceEraseDialogGui
///////////////////////////////////////////////////////////////////////////////
class DeviceEraseDialogGui : public wxDialog
{
	private:

	protected:
		wxStaticText* txtDestination;
		wxChoice* chcPartition;
		wxRadioBox* radioErase;
		wxButton* btnCancel;
		wxButton* btnErase;

		// Virtual event handlers, overide them in your derived class
		virtual void OnErase( wxCommandEvent& event ) { event.Skip(); }


	public:

		DeviceEraseDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Disk/Partition Erase Tool"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~DeviceEraseDialogGui();

};

///////////////////////////////////////////////////////////////////////////////
/// Class OSXMessageDialogGui
///////////////////////////////////////////////////////////////////////////////
class OSXMessageDialogGui : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* icon_bitmap;
		wxStaticText* txtCaption;
		wxStaticText* txtMessage;
		wxButton* btnOK;

	public:

		OSXMessageDialogGui( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxDEFAULT_DIALOG_STYLE );
		~OSXMessageDialogGui();

};

