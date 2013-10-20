/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
*   Copyright (C) 2010  Erdem U. Altinyurt                              *
*                                                                       *
*   This program is free software; you can redistribute it and/or       *
*   modify it under the terms of the GNU General Public License         *
*   as published by the Free Software Foundation; either version 2      *
*   of the License.                                                     *
*                                                                       *
*   This program is distributed in the hope that it will be useful,     *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*   GNU General Public License for more details.                        *
*                                                                       *
*   You should have received a copy of the GNU General Public License   *
*   along with this program;                                            *
*   if not, write to the Free Software Foundation, Inc.,                *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : www.wxhexeditor.org                             *
*               email : spamjunkeater@gmail.com                         *
*************************************************************************/


#ifndef __HexEditorDialogs__
#define __HexEditorDialogs__

#include "HexEditor.h"
#include "HexEditorCtrl/HexEditorCtrl.h"
#include "HexEditorGui.h"
#include <vector>
#ifdef WX_GCH
	#include <wx_pch.h>
#else
	#include <wx/wx.h>
#endif

bool HexVerifyAndPrepare(wxString& hexval, wxString Value_Name, wxWindow* parent=NULL);

//ComboBox Filler from stored Registry values
void ComboBoxFill( wxString SearchFormat, wxComboBox* CurrentBox, bool AddString);

class GotoDialog : public GotoDialogGui{
	public:
		GotoDialog( wxWindow* parent, uint64_t& offset, uint64_t cursor_offset, uint64_t filesize, unsigned BlockSize );
		void OnChar( wxKeyEvent& event );
		void OnInput( wxCommandEvent& event );
		void OnGo( wxCommandEvent& event );
		void OnConvert( wxCommandEvent& event );
		void EventHandler( wxCommandEvent& event );
		wxString Filter( wxString text );
		void OnPreviousSector( wxCommandEvent& event );
		void OnNextSector( wxCommandEvent& event );

	protected:
		enum {OPT_OFFSET_MODE=0x1,OPT_DEC_INPUT=0x2,OPT_BRANCH_NORMAL=0x4,OPT_BRANCH_END=0x8};
		uint64_t *offset;
		uint64_t cursor_offset;
		uint64_t filesize;
		unsigned BlockSize;
		bool is_olddec;
		class HexEditor* parent;
		wxArrayString hex_letters;//=wxT("abcdefABCDEFxX");
		wxArrayString dec_letters;//=wxT("1234567890abcdefABCDEFxX");
	};

class FindDialog : public FindDialogGui{
	public:
		FindDialog( wxWindow* parent, FAL *find_file, wxString title=_("Find") );
		bool OnFind( bool intenal=false );
		void OnFindPrev( wxCommandEvent& event );
		void OnFindAll( bool internal=false );
		void OnChar( wxKeyEvent& event );
virtual void EventHandler( wxCommandEvent& event );
		bool OnFindEvents( wxCommandEvent& event );
		char UTF8SpeedHack[2];

	protected:
		enum search_options{ SEARCH_HEX=0x1, SEARCH_TEXT=0x2, SEARCH_UTF8=0x4, SEARCH_UTF16=0x8,
									SEARCH_MATCHCASE=0x10, SEARCH_BACKWARDS=0x20, SEARCH_WRAPAROUND=0x40, SEARCH_FINDALL=0x80 } searchtype;
		uint64_t FindBinary( wxMemoryBuffer target, uint64_t start_from, unsigned oper=SEARCH_HEX );
		uint64_t FindText( wxString target, uint64_t start_from, unsigned oper=SEARCH_TEXT);
		int SearchAtBufferMultiThread( char *bfr, int bfr_size, char* search, int search_size, unsigned oper );
		int SearchAtBuffer( char *bfr, int bfr_size, char* search, int search_size, unsigned oper );
		int MultiSearchAtBuffer( char *bfr, int bfr_size, char* search, int search_size, unsigned oper, std::vector<int> *ret_ptr );
		bool SearchAtBufferUnitTest();
		void FindSomeBytes( void );
		class HexEditor* parent;
		FAL *findfile;
		void PrepareComboBox( bool AddString );

		bool SkipRAM( uint64_t& current_offset, unsigned search_size, unsigned& search_step, bool backward=false );
	};

class ReplaceDialog : public FindDialog{
	public:
		ReplaceDialog( wxWindow* parent, FAL *find_file, wxString title=_("Find & Replace") );
			void EventHandler( wxCommandEvent& event );
			int OnReplace( bool internal = false );
			void OnReplaceAll( void );
			void PrepareComboBox( bool AddString );
	};

class CopyAsDialog : public CopyAsDialogGui{
	public:
		CopyAsDialog( wxWindow* parent, FAL* copy_file, Select* select, ArrayOfTAG* MainTagArray);
	private:
		class HexEditor* parent;
		class Select* select;
		FAL * copy_file;
		ArrayOfTAG* MainTagArray;
      void EventHandler( wxCommandEvent& event );
      void Copy( void );
      void PrepareOptions( int SelectedFunction );
      void PrepareFullText( wxString& , wxMemoryBuffer& );
      void PrepareFullTextWithTAGs( wxString& , wxMemoryBuffer& , wxString startup);
      wxString GetDigitFormat( void );
	};

class CompareDialog : public CompareDialogGui{
	public:
		//CompareDialog( wxWindow* parent );
		CompareDialog( wxWindow* parent, wxString file1=wxEmptyString, wxString file2=wxEmptyString );
		~CompareDialog();

	private:
		enum{OPT_CMP_SEARCH_DIFF=0x01,OPT_CMP_STOP_AFTER=0x02, OPT_CMP_MERGE_SECTION=0x04, OPT_CMP_SAVE=0x8};
		class HexEditorFrame* parent;
		bool Compare( wxFileName f1, wxFileName f2, bool SearchForDiff, int StopAfterNMatch, wxFileName fsave);
      void EventHandler( wxCommandEvent& event );
      void EventHandler2( wxDropFilesEvent& event );
      void OnFileChange( wxFileDirPickerEvent& event );
	};

class ChecksumDialog : public ChecksumDialogGui{
	public:
		ChecksumDialog( wxWindow* parent );
		ChecksumDialog( wxWindow* parent, FAL *find_file );
	private:
		class HexEditorFrame* parent;
		wxString CalculateChecksum( FAL& fl, int options );
      void EventHandler( wxCommandEvent& event );
      void OnFileChange( wxFileDirPickerEvent& event );
	};

class XORViewDialog : public XORViewDialogGui{
	public:
		XORViewDialog( wxWindow* parent, wxMemoryBuffer *XORKey );
		void EventHandler( wxCommandEvent& event );
	private:
		wxMemoryBuffer *XORKey;
	};

class PreferencesDialog : public PreferencesDialogGui{
	public:
		PreferencesDialog( wxWindow* parent );
		void EventHandler( wxCommandEvent& event );
		void SpinEventHandler( wxSpinEvent& event );
		void GetInstalledLanguages(wxArrayString & names, wxArrayLong & identifiers);
		void OnSave(wxCommandEvent& event);
		void OnResetColours(wxCommandEvent& event);
		void OnCustomHexCheck( wxCommandEvent& event );

		wxArrayString LangNames;
		wxArrayLong LangIds;

		wxArrayString ExperimentalEncodingsList;
		wxArrayString AvailableEncodings;
	};

class DeviceBackupDialog : public DeviceBackupDialogGui{
	public:
		DeviceBackupDialog( wxWindow* parent );
		void OnBackup( wxCommandEvent& event );
	};

class DeviceRestoreDialog : public DeviceRestoreDialogGui{
	public:
		DeviceRestoreDialog( wxWindow* parent );
		void OnRestore( wxCommandEvent& event );
	};

class DeviceEraseDialog : public DeviceEraseDialogGui{
	public:
		DeviceEraseDialog( wxWindow* parent );
		void OnErase( wxCommandEvent& event );
	};

class OSXMessageDialog : public OSXMessageDialogGui{
	public:
		OSXMessageDialog( wxWindow* parent, wxString message, wxString Caption );
	};

void OSXwxMessageBox( wxString message, wxString Caption, int x, wxWindow* parent );

#endif //__HexEditorDialogs__
