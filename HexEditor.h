/***********************************(GPL)********************************
*	wxHexEditor is a hex edit tool for editing massive files in Linux   *
*	Copyright (C) 2007  Erdem U. Altinyurt                              *
*                                                                       *
*	This program is free software; you can redistribute it and/or       *
*	modify it under the terms of the GNU General Public License         *
*	as published by the Free Software Foundation; either version 2      *
*	of the License, or any later version.                               *
*                                                                       *
*	This program is distributed in the hope that it will be useful,     *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*	GNU General Public License for more details.                        *
*                                                                       *
*	You should have received a copy of the GNU General Public License   *
*	along with this program;                                            *
*   if not, write to the Free Software	Foundation, Inc.,               *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : death_knight at gamebox.net                     *
*************************************************************************/

#ifndef __wxHexEditor__
#define __wxHexEditor__

#include <wx/filename.h>
#include "HexEditorCtrl/HexEditorCtrl.h"

class scrollthread;
class wxHexEditor: public HexEditorCtrl {
	public:
	    wxHexEditor(wxWindow* parent,
					int id,
					wxStatusBar *statusbar=NULL,
//					DataInterpreter *interpreter=NULL,
					wxFileName* myfile=NULL,
					const wxPoint& pos=wxDefaultPosition,
					const wxSize& size=wxDefaultSize,
					long style=0);
		~wxHexEditor( void );
		friend class scrollthread;

		void OnOffsetScroll(wxScrollEvent &event);
		void LoadFromOffset(int64_t position, bool cursor_reset = false, bool paint = true );	//loads file from position

		int64_t FileLenght( void );
		void FileOpen( wxFileName& filename  );	//opens a file
		bool FileClose( void );
//		bool FileSave( bool question = true );
		void OnResize( wxSizeEvent &event );
//		bool save( bool question = true );
//		void redo( void );
//		void undo( void );
//		void finddlg( void );
//		int64_t findbin( const char *target, int size, int64_t start_from );
//		int64_t find( wxString *target, int size, int64_t start_from, bool as_hex = false );
		int64_t GetHexOffset( void );
		void SetHexOffset( int64_t HexOffset );
		bool Select ( int64_t start_offset, int64_t end_offset );
protected:
//public4test		bool Select ( int64_t start_offset, int64_t end_offset );

		void OnKeyboardInput(wxKeyEvent& event);

		void OnKeyboardSelector( wxKeyEvent& event );
		void OnKeyboardSelectionEnd( wxKeyEvent& event );
		//void RefreshCursor(int64_t cursor_location = -1 );
		void OnHexMouseFocus( wxMouseEvent& event );
		void OnTextMouseFocus( wxMouseEvent& event );
		void OnMouseSelectionEnd( wxMouseEvent& event );
		void OnMouseMove( wxMouseEvent& event );
//=====================
/*

	bool copy( bool as_hex = false );
	bool paste( bool as_hex = false );
	bool replace ( bool as_hex = false );

private:

    void init_hex_editor( void );
    wxWindow* parent;
	int id;
    DECLARE_EVENT_TABLE()

    void OnMouseMove( wxMouseEvent& event );
	void OnKeyboardChar(wxKeyEvent& event);
    void OnOffsetControlMouseFocus( wxMouseEvent& event );
    void OnMouseSelectionEnd( wxMouseEvent& event );
    wxChar inline TextFilter(const unsigned char);
	void hex_mouse_input_handler( wxMouseEvent& event );
	void text_mouse_input_handler( wxMouseEvent& event );
    void TextCharReplace(long byte, const wxChar chr);
    void HexCharReplace(long hex_location, const wxChar chr);
	int search_at_buffer( const char *bfr, int bfr_size, const char* search, int search_size );

protected:
   	friend class scrollthread;
   	friend class FindDialog;
	scrollthread *myscroll;
	copy_maker *copy_mark;
    DataInterpreter *interpreter;
	void UpdateInterpreter();

*/
//=======================
	protected:
		wxStatusBar* statusbar;
		wxFileName myfilename;
		wxFile *myfile;
		void UpdateStatusBar();
		scrollthread *myscroll;

	private:
	    void Dynamic_Connector( void );
	    void Dynamic_Disconnector( void );
		void inline ClearPaint( void );
		bool Selector( void );
};

class scrollthread:wxThreadHelper{
	private:
		wxHexEditor *parent;
		int speed, sleeper,cursor;
	public:
	scrollthread(int initial_speed, wxHexEditor *parent ):parent(parent){
		sleeper = 25;
		cursor = 0;
		speed = initial_speed;
		Create();
		GetThread()->Run();
		if(speed == 0)
			GetThread()->Pause();
		}
	void *Entry(){
		while( !(GetThread()->TestDestroy()) ){
			if(speed == 0)
				continue;	// loop to "while" for init of class and wait for GetThread()->Pause();
			int64_t FileLenght = parent->FileLenght();
			parent->start_offset += ( parent->hex_ctrl->BytePerLine() )*speed;
			if( parent->start_offset < 0 )
				parent->start_offset = 0;
			else if( parent->start_offset + parent->hex_ctrl->ByteCapacity() >= FileLenght ){
				parent->start_offset = FileLenght - parent->hex_ctrl->ByteCapacity();
				parent->start_offset += parent->hex_ctrl->BytePerLine() - (parent->start_offset % parent->hex_ctrl->BytePerLine()) ; //cosmetic
				}
			wxMutexGuiEnter();
		//	parent->MyFreeze();
			parent->LoadFromOffset( parent->start_offset, false, false );

				parent->SetHexInsertionPoint(cursor);
				parent->Selector();
				parent->PaintSelection();

			if( parent->offset_scroll->GetThumbPosition() != parent->start_offset / parent->hex_ctrl->ByteCapacity() )
				parent->offset_scroll->SetThumbPosition( parent->start_offset / parent->hex_ctrl->ByteCapacity() );
		//	parent->MyThaw();

		//	wxYieldIfNeeded();
			wxMutexGuiLeave();
			GetThread()->Sleep(sleeper);
			if( parent->start_offset == 0 ||
				parent->start_offset + parent->hex_ctrl->ByteCapacity() >= FileLenght )
				GetThread()->Pause();
			}
		return NULL;
		}
	void UpdateSpeed(int new_speed, int sleeptime = 25){
		if(new_speed == 0)
			GetThread()->Pause();
		else
			GetThread()->Resume();
		speed = new_speed;
		sleeper = sleeptime;
		cursor = parent->GetHexInsertionPoint();
		}
	void Exit(){
		GetThread()->Delete();
		}
	};

#endif
