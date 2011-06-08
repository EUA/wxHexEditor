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
*               home  : wxhexeditor.sourceforge.net                     *
*               email : spamjunkeater at gmail dot com                  *
*************************************************************************/


#ifndef __HexEditorDialogs__
#define __HexEditorDialogs__

#include "HexEditor.h"
#include "HexEditorGui.h"

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif
struct DialogVector{
	unsigned short goto_hex;
	unsigned short goto_branch;
	uint64_t goto_input;
	};

class GotoDialog : public GotoDialogGui{
	public:
		GotoDialog( wxWindow* parent, uint64_t& offset, uint64_t cursor_offset, uint64_t filesize, DialogVector * );
		void OnInput( wxCommandEvent& event );
		void OnGo( wxCommandEvent& event );
		void OnConvert( wxCommandEvent& event );
		void OnInit( wxCommandEvent& event );

		wxString Filter( wxString text );

		DialogVector *myDialogVector;

	protected:
		uint64_t *offset;
		uint64_t cursor_offset;
		uint64_t filesize;
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
		enum search_options{ SEARCH_HEX=0x1, SEARCH_TEXT=0x2,SEARCH_UTF8=0x4,SEARCH_UTF16=0x8, SEARCH_MATCHCASE=0x10, SEARCH_BACKWARDS=0x20, SEARCH_WRAPAROUND=0x40, SEARCH_FINDALL=0x80 } searchtype;
	protected:
		uint64_t FindBinary( wxMemoryBuffer target, uint64_t start_from, unsigned oper=SEARCH_HEX );
		uint64_t FindText( wxString target, uint64_t start_from, unsigned oper=SEARCH_TEXT);
		int SearchAtBuffer( char *bfr, int bfr_size, char* search, int search_size, unsigned oper );
		class HexEditor* parent;
		FAL *findfile;
	};

class ReplaceDialog : public FindDialog{
	public:
		ReplaceDialog( wxWindow* parent, FAL *find_file, wxString title=_("Find & Replace") );
			void EventHandler( wxCommandEvent& event );
			int OnReplace( bool internal = false );
			void OnReplaceAll( void );
	};

class CompareDialog : public CompareDialogGui{
	public:
		CompareDialog( wxWindow* parent );
	private:
		class HexEditorFrame* parent;
		bool Compare( wxFileName f1, wxFileName f2, bool SearchForDiff, int StopAfterNMatch, wxFileName fsave);
virtual void EventHandler( wxCommandEvent& event );
	};

class ChecksumDialog : public ChecksumDialogGui{
	public:
		ChecksumDialog( wxWindow* parent );
		ChecksumDialog( wxWindow* parent, FAL *find_file );
	private:
		class HexEditorFrame* parent;
		enum checksum_options{ MD5=0x1,SHA1=0x2,SHA256=0x4,SHA384=0x8,SHA512=0x10 };
		char *checksum_options_strings[5];
		wxString CalculateChecksum( FAL& fl, unsigned options );
virtual void EventHandler( wxCommandEvent& event );
	};
#endif //__HexEditorDialogs__
