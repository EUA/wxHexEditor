/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
*   Copyright (C) 2010  Erdem U. Altinyurt                              *
*                                                                       *
*   This program is free software; you can redistribute it and/or       *
*   modify it under the terms of the GNU General Public License         *
*   as published by the Free Software Foundation; either version 2      *
*   of the License, or any later version.                               *
*                                                                       *
*   This program is distributed in the hope that it will be useful,     *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*   GNU General Public License for more details.                        *
*                                                                       *
*   You should have received a copy of the GNU General Public License   *
*   along with this program;                                            *
*   if not, write to the Free Software	Foundation, Inc.,                *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : death_knight at gamebox.net                     *
*************************************************************************/

#ifndef _wxHexEditor_h_
#define _wxHexEditor_h_

#include <wx/ffile.h>
#include <wx/clipbrd.h>

#include "FileDifference.h"
#include "DataInterpreter.h"
#include "InfoPanel.h"
#include "HexEditorGui.h"
#include "HexEditorCtrl/HexEditorCtrl.h"
#include "HexDialogs.h"
#define MB 1024*1024	//this utility uses old ECS format.

class scrollthread;
class copy_maker;
class HexEditor: public HexEditorCtrl/*, protected FileDifference*/ {
	public:
	    HexEditor(wxWindow* parent,
					int id,
					wxStatusBar *statusbar=NULL,
					DataInterpreter *interpreter=NULL,
					InfoPanel *infopanel=NULL,
					wxFileName* myfile=NULL,
					const wxPoint& pos=wxDefaultPosition,
					const wxSize& size=wxDefaultSize,
					long style=0);
		~HexEditor( void );
		friend class scrollthread;
		//friend class FindDialog;

		void Goto( int64_t goto_offset=-1 );
		void OnOffsetScroll(wxScrollEvent &event);
		void LoadFromOffset(int64_t position, bool cursor_reset = false, bool paint = true );	//loads file from position
		void Reload();	//loads file from current page offset; refresh

		bool FileOpen( wxFileName& filename  );
		bool FileSave( bool question = true );
		bool FileSave( wxString filename );
		bool FileClose( void );
		void DoUndo( void );
		void DoRedo( void );

		bool CutSelection( void );
		bool DeleteSelection( void );
		bool CopySelection( void );
		bool PasteFromClipboard( void );

		//----File Functions----//
		int64_t FileLength( void ){ return myfile->Length();}
		bool FileAddDiff( int64_t start_byte, const char* data, int64_t size, bool extension = false ); //adds new node
		wxFileName GetFileName( void ){ return myfile->GetFileName();}
		bool SetFileAccessMode( FileDifference::FileAccessMode fam ){ return myfile->SetAccessMode( fam ); }
		int GetFileAccessMode( void ){ return myfile->GetAccessMode();}
		wxString GetFileAccessModeString( void ){ return myfile->GetAccessModeString();}
		int GetFD( void ){ return myfile->fd(); }
		bool IsAvailable_Undo( void ){ return myfile->IsAvailable_Undo(); }
		bool IsAvailable_Redo( void ){ return myfile->IsAvailable_Redo(); }
		void FindDialog( void );
		void ReplaceDialog( void );
		void GotoDialog( void );
		struct DialogVector *myDialogVector;

		void UpdateCursorLocation( bool force=false );
protected:
		void SetLocalHexInsertionPoint( int hex_location );
		void UpdateOffsetScroll( void );
		void OnKeyboardChar(wxKeyEvent& event);
		void OnKeyboardInput(wxKeyEvent& event);
		void OnKeyboardSelector( wxKeyEvent& event );
		void OnKeyboardSelectionEnd( wxKeyEvent& event );
		void OnMouseTest( wxMouseEvent& event );
		void OnMouseLeft( wxMouseEvent& event );
		void OnMouseRight( wxMouseEvent& event );
		void OnMouseSelectionEnd( wxMouseEvent& event );
		void OnMouseMove( wxMouseEvent& event );
		void OnMouseWhell( wxMouseEvent& event );
		void OnOffsetMouseFocus( wxMouseEvent& event );
		void OnResize( wxSizeEvent &event );

		void ShowContextMenu( const wxMouseEvent& event );
		void ScrollNoThread( int speed );

		wxStatusBar* statusbar;
		FileDifference *myfile;
		scrollthread *myscroll;
		DataInterpreter *interpreter;
		InfoPanel *infopanel;
		copy_maker *copy_mark;

	private:
		bool MouseCapture;
	    void Dynamic_Connector( void );
	    void Dynamic_Disconnector( void );
};

class copy_maker{
		public:
			bool copied;		//copy in action or not
			int64_t start;		//copy start position
			int64_t size;		//size of copy
			wxMemoryBuffer m_buffer; //uses RAM, for small data
			//wxFile *tempfile;	//uses Temp HDD File and delete after.
			FileDifference *sourcefile;	//uses HDD File and NOT delete after.
			copy_maker( ){
				copied = false;
				start = size = 0;
			//	tempfile = NULL;
				sourcefile = NULL;
				}
			~copy_maker( ){
				//if(tempfile != NULL)
				//if(sourcefile != NULL)
				}
			bool SetClipboardData( wxString& CopyString){
				if(wxTheClipboard->Open()){
//					if (wxTheClipboard->IsSupported( wxDF_TEXT )){
						wxTheClipboard->Clear();
						int isok = wxTheClipboard->SetData( new wxTextDataObject( CopyString ));
						wxTheClipboard->Flush();
						wxTheClipboard->Close();
						return isok;
						}
//					else{
//						wxMessageBox( _( "Clipboard is not support TEXT copy\nOperation cancelled!"), _("Copy To Clipboard Error"), wxOK|wxICON_ERROR);
//						wxTheClipboard->Close();
//						return false;
//						}
//					}
				else{
					wxMessageBox( _( "Clipboard could not be opened\nOperation cancelled!"), _("Copy To Clipboard Error"), wxOK|wxICON_ERROR);
					return false;
					}
				}

			wxString GetClipboardData( void ){
				if(wxTheClipboard->Open()){
					if (wxTheClipboard->IsSupported( wxDF_TEXT )){
						wxTextDataObject data;
						wxTheClipboard->GetData( data );
						wxTheClipboard->Close();
						return data.GetText();
						}
					else{
						wxBell();
						wxTheClipboard->Close();
						return wxString();
						}
					}
				else{
					wxMessageBox( _( "Clipboard could not be opened\nOperation cancelled!"), _("Copy To Clipboard Error"), wxOK|wxICON_ERROR);
					return wxString();
					}
				}
		};

class scrollthread:wxThreadHelper{
	private:
		HexEditor *parent;
		int speed, sleeper,cursor;
	public:
	scrollthread(int initial_speed, HexEditor *parent ):parent(parent){
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
			int64_t FileLength = parent->FileLength();
			parent->page_offset += ( parent->BytePerLine() )*speed;
			if( parent->page_offset < 0 )
				parent->page_offset = 0;
			else if( parent->page_offset + parent->ByteCapacity() >= FileLength ){
				parent->page_offset = FileLength - parent->ByteCapacity();
				parent->page_offset += parent->BytePerLine() - (parent->page_offset % parent->BytePerLine()) ; //cosmetic
				}
			wxMutexGuiEnter();

			parent->LoadFromOffset( parent->page_offset, false, false );

				parent->SetLocalHexInsertionPoint(cursor);	//KILLs MACOSX via Calling UpdateCursorLocation()
				parent->Selector();						//KILLs MACOSX
				parent->PaintSelection();
				parent->UpdateCursorLocation( true );	//KILLs MACOSX

			if( parent->offset_scroll->GetThumbPosition() != parent->page_offset / parent->ByteCapacity() )
				parent->offset_scroll->SetThumbPosition( parent->page_offset / parent->ByteCapacity() );

		//	wxYieldIfNeeded();
			wxMutexGuiLeave();
			GetThread()->Sleep(sleeper);
			if( parent->page_offset == 0 ||
				parent->page_offset + parent->ByteCapacity() >= FileLength )
				GetThread()->Pause();
			}
		return NULL;
		}
	void UpdateSpeed(int new_speed, int sleeptime = 25){
		if (new_speed == 0 and speed == 0 )
			return;

		else if(new_speed == 0)
			GetThread()->Pause();
		else
			GetThread()->Resume();
		speed = new_speed;
		sleeper = sleeptime;
		cursor = parent->GetLocalHexInsertionPoint();
		}
	};

#endif
