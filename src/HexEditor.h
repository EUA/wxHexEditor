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


#ifndef _wxHexEditor_h_
#define _wxHexEditor_h_

#include "../mhash/include/mhash.h"

#include <wx/ffile.h>
#include <wx/clipbrd.h>
#include <wx/numdlg.h>
#include <wx/version.h>
#include <wx/progdlg.h>

#include "FAL.h"
#include "HexPanels.h"
#include "HexEditorGui.h"
#include "HexEditorCtrl/HexEditorCtrl.h"
#include "HexDialogs.h"

#define idInjection 3000
#define idBlockSelect 3001

#define MB (1024*1024)	//this utility uses old ECS format.

class scrollthread;
class copy_maker;

class HexEditor: public HexEditorCtrl{ /*, protected FAL*/
	public:
		HexEditor(wxWindow* parent,
		          int id,
		          wxStatusBar *statusbar=NULL,
		          DataInterpreter *interpreter=NULL,
		          InfoPanel *infopanel=NULL,
		          TagPanel *tagpanel=NULL,
		          DisassemblerPanel *dasmpanel=NULL,
		          wxFileName* myfile=NULL,
		          const wxPoint& pos=wxDefaultPosition,
		          const wxSize& size=wxDefaultSize,
		          long style=0);
		~HexEditor( void );
		friend class scrollthread;
		friend class ChecksumDialog;

		HexEditor *ComparatorHexEditor;
		void Goto( int64_t goto_offset=-1, bool set_focus=false );
		void OnOffsetScroll(wxScrollEvent &event);
		void LoadFromOffset(int64_t position, bool cursor_reset=false, bool paint=true, bool from_comparator=false );	//loads file from position
		void Reload();	//loads file from current page offset; refresh
		void ReDraw();
		void ThreadPaint(wxCommandEvent& event); //This class created for remove scroolthread update functions with MutexGui Lock mechanism.

		bool IsFileUsingXORKey( void );
		void FileSetXORKey( bool enable );
		wxString FileGetXORKey( void );
		bool FileOpen( wxFileName& filename  );
		bool FileReOpen( void );
		bool FileSave( bool question = true );
		bool FileSave( wxString filename );
		bool FileClose( bool WithoutChange = false );
		bool SaveAsDump( void );
		bool FillSelection( void );
		void BlockSelect( void );
		void DoUndo( void );
		void DoRedo( void );

		bool CutSelection( void );
		bool DeleteSelection( void );
		bool InsertBytes( void );
		bool CopySelection( void );
		bool PasteFromClipboard( void );

		//----File Functions----//
		int64_t FileLength( void ) {
			//if( myfile->IsProcess() )
			//	return ProcessRAMMap.Last();
			return myfile->Length();
			}
		bool FileAddDiff( int64_t start_byte, const char* data, int64_t size, bool extension = false ); //adds new node
		wxFileName GetFileName( void ) {
			return myfile->GetFileName();
			}
		bool SetFileAccessMode( FAL::FileAccessMode fam ) {
			return myfile->SetAccessMode( fam );
			}
		int GetFileAccessMode( void ) {
			return myfile->GetAccessMode();
			}
		wxString GetFileAccessModeString( void ) {
			return myfile->GetAccessModeString();
			}
		bool IsBlockDevice( void ){
			return myfile->GetBlockSize() > 0;
			}
		int GetFD( void ) {
			return myfile->fd();
			}
		bool IsAvailable_Undo( void ) {
			return myfile->IsAvailable_Undo();
			}
		bool IsAvailable_Redo( void ) {
			return myfile->IsAvailable_Redo();
			}
		int HashVerify(wxString hash_file,FAL* File=NULL);

		void FindDialog( void );
		void ReplaceDialog( void );
		void CopyAsDialog( void );
		void GotoDialog( void );

		void UpdateCursorLocation( bool force=false );
		void ConnectScroll(HexEditor* connection);
		void DisconnectScroll( void );
	protected:
		void SetLocalHexInsertionPoint( int hex_location, bool from_comparator=false );
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
		void OnUpdateUI( wxUpdateUIEvent& event );
#if wxCHECK_VERSION( 2,9,0 )
		void OnFileModify( wxFileSystemWatcherEvent& event );
#endif
		void ShowContextMenu( const wxMouseEvent& event );
		void ScrollNoThread( int speed );

		wxStatusBar* statusbar;
		FAL *myfile;
		scrollthread *myscroll;
		DataInterpreter *interpreter;
		InfoPanel *infopanel;
		TagPanel *tagpanel;
		DisassemblerPanel *dasmpanel;
		copy_maker *copy_mark;

	private:
		uint64_t BlockSelectOffset;
		bool MouseCapture;
		void Dynamic_Connector( void );
		void Dynamic_Disconnector( void );
	};

class copy_maker {
	public:
		bool copied;		//copy in action or not
		int64_t start;		//copy start position
		int64_t size;		//size of copy
		wxMemoryBuffer m_buffer; //uses RAM, for small data
		//wxFile *tempfile;	//uses Temp HDD File and delete after.
		FAL *sourcefile;	//uses HDD File and NOT delete after.
		copy_maker( ) {
			copied = false;
			start = size = 0;
			//	tempfile = NULL;
			sourcefile = NULL;
			}
		~copy_maker( ) {
			//if(tempfile != NULL)
			//if(sourcefile != NULL)
			}
		bool SetClipboardData( wxString& CopyString) {
			if(wxTheClipboard->Open()) {
//					if (wxTheClipboard->IsSupported( wxDF_TEXT )){
				wxTheClipboard->Clear();
				int isok = wxTheClipboard->SetData( new wxTextDataObject( CopyString ));
				wxTheClipboard->Flush();
				wxTheClipboard->Close();
				return isok;
				}
			else {
				wxMessageBox( wxString(_( "Clipboard could not be opened.")) + wxT("\n") + _("Operation cancelled!"), _("Copy To Clipboard Error"), wxOK|wxICON_ERROR);
				return false;
				}
			}

		wxString GetClipboardData( void ) {
			if(wxTheClipboard->Open()) {
				if (wxTheClipboard->IsSupported( wxDF_TEXT )) {
					wxTextDataObject data;
					wxTheClipboard->GetData( data );
					wxTheClipboard->Close();
					return data.GetText();
					}
				else {
					wxBell();
					wxTheClipboard->Close();
					return wxString();
					}
				}
			else {
				wxMessageBox( wxString(_( "Clipboard could not be opened.")) + wxT("\n") + _("Operation cancelled!"), _("Copy To Clipboard Error"), wxOK|wxICON_ERROR);
				return wxString();
				}
			}
	};

class scrollthread:wxThreadHelper {
	private:
		HexEditor *parent;
		int speed, sleeper, cursor;
	public:
		scrollthread(int initial_speed, HexEditor *parent ):parent(parent) {
			sleeper = 25;
			cursor = 0;
			speed = initial_speed;
#if wxCHECK_VERSION(2,9,0)
			CreateThread();
#else
			Create();
#endif
			GetThread()->Run();
			if(speed == 0)
				GetThread()->Pause();
			}

		void *Entry() {
			while( !(GetThread()->TestDestroy()) ) {
				if(speed == 0)
					continue;	// loop to "while" for init of class and wait for GetThread()->Pause();
				int64_t FileLength = parent->FileLength();
				parent->page_offset += ( parent->BytePerLine() )*speed;
				if( parent->page_offset < 0 )
					parent->page_offset = 0;
				else if( parent->page_offset + parent->ByteCapacity() >= FileLength ) {
					parent->page_offset = FileLength - parent->ByteCapacity();
					parent->page_offset += parent->BytePerLine() - (parent->page_offset % parent->BytePerLine()) ; //cosmetic
					}

#if wxCHECK_VERSION(3, 0, 0)
				wxCommandEvent *eventx=new wxCommandEvent( wxEVT_COMMAND_TEXT_UPDATED, THREAD_UPDATE_EVENT );
				::wxQueueEvent( parent, eventx );
				wxYield();

				while(not eventx->GetSkipped())
					GetThread()->Sleep(sleeper);
				//delete eventx;
#else			//Old versions
				wxCommandEvent eventx( wxEVT_COMMAND_TEXT_UPDATED, THREAD_UPDATE_EVENT );
				wxMutexGuiEnter();
				::wxPostEvent(parent, eventx );
				wxMutexGuiLeave();

				GetThread()->Sleep(sleeper);
#endif

				if( parent->page_offset == 0 ||
				      parent->page_offset + parent->ByteCapacity() >= FileLength )
					GetThread()->Pause();
				}
			return NULL;
			}
		void UpdateSpeed(int new_speed, int sleeptime = 25) {
			if (new_speed == 0 and speed == 0 )
				return;
			else if(new_speed == 0 and GetThread()->IsRunning() )
				GetThread()->Pause();
			else if( GetThread()->IsPaused() )
				GetThread()->Resume();
			speed = new_speed;
			sleeper = sleeptime;
			cursor = parent->GetLocalHexInsertionPoint();
			}
	};

#endif
