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

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

bool HexVerifyAndPrepare(wxString& hexval, wxString Value_Name, wxWindow* parent=NULL);
void ComboBoxFill( wxString SearchFormat, wxComboBox* CurrentBox, bool AddString);

class GotoDialog : public GotoDialogGui{
	public:
		GotoDialog( wxWindow* parent, uint64_t& offset, uint64_t cursor_offset, uint64_t filesize, unsigned BlockSize );
		void OnInput( wxCommandEvent& event );
		void OnGo( wxCommandEvent& event );
		void OnConvert( wxCommandEvent& event );
		void OnInit( wxCommandEvent& event );

		wxString Filter( wxString text );

	protected:
		uint64_t *offset;
		uint64_t cursor_offset;
		uint64_t filesize;
		unsigned BlockSize;
		bool is_olddec;
	};

class FindDialog : public FindDialogGui{
	public:
		FindDialog( wxWindow* parent, FAL *find_file, wxString title=_("Find") );
		bool OnFind( bool intenal=false );
		void OnFindPrev( wxCommandEvent& event );
		void OnFindAll( bool internal=false );
virtual void EventHandler( wxCommandEvent& event );
		bool OnFindEvents( wxCommandEvent& event );

		enum search_options{ SEARCH_HEX=0x1, SEARCH_TEXT=0x2, SEARCH_UTF8=0x4, SEARCH_UTF16=0x8,
									SEARCH_MATCHCASE=0x10, SEARCH_BACKWARDS=0x20, SEARCH_WRAPAROUND=0x40, SEARCH_FINDALL=0x80 } searchtype;
	protected:
		uint64_t FindBinary( wxMemoryBuffer target, uint64_t start_from, unsigned oper=SEARCH_HEX );
		uint64_t FindText( wxString target, uint64_t start_from, unsigned oper=SEARCH_TEXT);
		int SearchAtBuffer( char *bfr, int bfr_size, char* search, int search_size, unsigned oper );
		void FindSomeBytes( void );
		class HexEditor* parent;
		FAL *findfile;
		void PrepareComboBox( bool AddString );
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
		CompareDialog( wxWindow* parent );
		~CompareDialog();
	private:
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
		//enum checksum_options{ MD5=0x1,SHA1=0x2,SHA256=0x4,SHA384=0x8,SHA512=0x10 };
		char *checksum_options_strings[5];
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

#endif //__HexEditorDialogs__
