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

#ifndef __wxHexEditorCtrl__
#define __wxHexEditorCtrl__

#include <wx/xml/xml.h>
#include <wx/filename.h>
#include <wx/event.h>
#include <wx/config.h>

#include "HexEditorCtrlGui.h"
#define SELECT_EVENT 50005
#define UNREDO_EVENT 50006
#define TAG_CHANGE_EVENT 50007
#define SEARCH_CHANGE_EVENT 50008
#define COMPARE_CHANGE_EVENT 50009
#define XORVIEW_EVENT 50010
#define RESET_STYLE_EVENT 50011
#define THREAD_UPDATE_EVENT 50012

WX_DECLARE_OBJARRAY(uint64_t, wxArrayUINT64);

class Select{	//for using EventHandler
	public:
		Select( wxEvtHandler* evth_ ){
			StartOffset = EndOffset = 0;
			state = false;
			evth = evth_;
			}
		uint64_t GetSize( void ){
			return abs( EndOffset - StartOffset)+1;};	//for select byte 13 start=13, end=13
		void SetState( bool new_state ){
			state = new_state;

			std::cout << "Send UpdateUI Event" << std::endl;
			wxUpdateUIEvent event;
			if( new_state )
				event.SetString( wxT("Selected") );
			else
				event.SetString( wxT("NotSelected") );
			event.SetId( SELECT_EVENT );//idFileRO
			evth->ProcessEvent( event );
			}
		bool GetState(){
			return state;
			}
		uint64_t GetStart( void ){
			return StartOffset < EndOffset ? StartOffset : EndOffset;
			}
		uint64_t GetEnd( void ){
			return StartOffset > EndOffset ? StartOffset : EndOffset;
			}

		uint64_t StartOffset;	//real selection start position
		uint64_t OriginalStartOffset;  //real selection start position backup for HexTextCTRL
		uint64_t EndOffset;		//real selection end position, included to select
	private:
		bool  state;
		wxEvtHandler* evth;
	};

class wxHugeScrollBar: public wxEvtHandler{ //64bit wrapper for wxScrollbar
   //friend class wxHexEditorCtrl;
   public:
      wxHugeScrollBar( wxScrollBar* m_scrollbar_ );
		~wxHugeScrollBar();

      void Enable( bool en ){ m_scrollbar->Enable(en); }
      wxSize GetSize( void ){ return m_scrollbar->GetSize(); }
      int64_t GetRange( void ){ return m_range; };
      int64_t GetThumbPosition( void ){ return m_thumb; }
      void SetThumbPosition(int64_t setpos);
      void SetScrollbar( int64_t Current_Position,int page_x, int64_t new_range, int pagesize, bool repaint=true );
		void OnOffsetScroll( wxScrollEvent& event );

   private:
      int64_t m_range;
      int64_t m_thumb;
      wxScrollBar *m_scrollbar;
   };

class HexEditorCtrl: public HexEditorCtrlGui{
	public:
		HexEditorCtrl(wxWindow* parent, int id,
						const wxPoint& pos=wxDefaultPosition,
						const wxSize& size=wxDefaultSize,
						long style=0);
		~HexEditorCtrl( void );
		enum panels { HEX_CTRL=0x1, TEXT_CTRL=0x2, OFFSET_CTRL=0x4 } focus;
		class Select *select;
		enum IDS{ idTagAddSelection=2001,idTagEdit, idTagQuick };
		void ReadFromBuffer( uint64_t position, unsigned lenght, char *buffer, bool cursor_reset = true, bool paint = true );
		void RePaint( void );
		uint64_t CursorOffset( void );
		void SetFont( wxFont f );
		void SetFont( );
		void SetStyle( );
		void ControlShow( panels control, bool hide );
		bool ControlIsShown(panels control);
      //void OnOffsetScroll( wxScrollEvent& event );
		wxHugeScrollBar* offset_scroll;

		ArrayOfTAG CompareArray;
		ArrayOfTAG HighlightArray;
		ArrayOfTAG MainTagArray;
		wxFont stdfont;
		bool ZebraEnable;
		int *ZebraStriping;

		bool LoadTAGS( wxFileName );
		bool SaveTAGS( wxFileName );
	protected:
		void Dynamic_Connector();
		void Dynamic_Disconnector();
		void Clear( bool RePaint=false, bool cursor_reset=true );
//		DECLARE_EVENT_TABLE()
		virtual void OnResize( wxSizeEvent& event);
		void ClearPaint( void );
		void PaintSelection( void );
		void PreparePaintTAGs( void );
		void PushTAGToControls( TagElement* TAG);
//		void TagPaint( void );
		bool TAGMutex;
		void MoveTAGS( uint64_t location, int64_t size );

	public:
		bool Selector( void );
		bool Select( uint64_t start_offset, uint64_t end_offset );

	protected:
		void HexCharReplace( long char_location, const wxChar chr);
		void TextCharReplace( long char_location, const wxChar chr);
		void MyFreeze();
		void MyThaw();
		void ShowContextMenu(const wxMouseEvent& event);
		//------EVENTS---------//
		void OnMouseLeft(wxMouseEvent& event);
		void OnMouseMove( wxMouseEvent& event );
		void OnMouseSelectionEnd( wxMouseEvent& event );
		void OnMouseRight( wxMouseEvent& event );
		void TagCreator( bool QuickTag );
		void OnTagAddSelection( wxCommandEvent& event );
		void OnTagQuick( wxCommandEvent& event );
		void OnTagEdit( wxCommandEvent& event );
		void OnKillFocus( wxFocusEvent& event );
		void UpdateUI(wxUpdateUIEvent& event);

		uint64_t LastRightClickAt;
		//----ADAPTERS----//
	public:
		void TagHideAll( void );
		int HexPerLine( void )  { return (hex_ctrl->IsShown() ? hex_ctrl->CharacterPerLine() : text_ctrl->CharacterPerLine()*2); }
		int BytePerLine( void )	{ return (hex_ctrl->IsShown() ? hex_ctrl->BytePerLine() : text_ctrl->BytePerLine() ); }
		unsigned ByteCapacity( void ){ return (hex_ctrl->IsShown() ? hex_ctrl->ByteCapacity() : text_ctrl->ByteCapacity()); }
		int LineCount( void )	{ return (hex_ctrl->IsShown() ? hex_ctrl->LineCount() : text_ctrl->LineCount()); }
		int ActiveLine( void )	{ return (hex_ctrl->IsShown() ? hex_ctrl->ActiveLine() : text_ctrl->ActiveLine() ) ; }
		int GetByteCount( void ){ return (hex_ctrl->IsShown() ? hex_ctrl->GetByteCount() : text_ctrl->GetByteCount()); }
		int GetLastPosition( void ){ return (hex_ctrl->IsShown() ? hex_ctrl->GetLastPosition() : text_ctrl->GetLastPosition()); }
		int GetLocalHexInsertionPoint( void );
		int GetLocalInsertionPoint( void );
		wxString GetFormatedOffsetString( uint64_t offset ){ return offset_ctrl->GetFormatedOffsetString( offset ); }
virtual void SetLocalHexInsertionPoint( int hex_location );
		void OnOffsetMouseFocus( wxMouseEvent& event );
		int sector_size;
		wxArrayUINT64 ProcessRAMMap;
		uint64_t ProcessRAM_GetFootPrint();
		uint64_t ProcessRAM_GetVirtualOffset( uint64_t offset );
		uint64_t ProcessRAM_FindNextMap( uint64_t current_offset, bool backward=false );
		bool ProcessRAM_FindMap( uint64_t current_offset, uint64_t& start, uint64_t& end, bool backward=false);

	protected:
		int64_t page_offset;	//holds current start offset of file
/*
virtual int ToExactPosition( int InternalPosition );
virtual int ToInternalPosition( int ExactPosition );
virtual int PixelCoordToInternalPosition( wxPoint mouse );
		wxPoint InternalPositionToExactCoord( int position );
		wxPoint PixelCoordToInternalCoord( wxPoint mouse );
*/
//		virtual void OnKeyboardChar( wxKeyEvent& event );
//		virtual void OnKeyboardInput( wxKeyEvent& event );
	};
#endif
