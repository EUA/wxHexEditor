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

#ifdef __WXMSW__
#include <psapi.h>
#endif

#if wxCHECK_VERSION( 2,9,0 ) && defined(__linux__)
//Only GTK port of Linux is working good on detect changes of file.
//MSW cannot detect changes in individual files yet in wxWidgets 3.1.0
#define _FSWATCHER_  1
#include <wx/fswatcher.h>
#else
#define _FSWATCHER_  0
#endif

#define idInjection 3000
#define idBlockSelect 3001



class scrollthread;
class copy_maker;

class HexEditor: public HexEditorCtrl
/*, protected FAL*/
	{
	public:
		HexEditor(wxWindow* parent,
		          int id,
		          wxStatusBar *statusbar=NULL,
		          DataInterpreter *interpreter=NULL,
		          InfoPanel *infopanel=NULL,
		          TagPanel *tagpanel=NULL,
		          DisassemblerPanel *dasmpanel=NULL,
		          copy_maker *copy_mark=NULL,
		          wxFileName* myfile=NULL,
		          const wxPoint& pos=wxDefaultPosition,
		          const wxSize& size=wxDefaultSize,
		          long style=0);
		~HexEditor( void );
		friend class scrollthread;
		friend class ChecksumDialog;

		HexEditor *ComparatorHexEditor;
		void Goto( int64_t goto_offset, bool set_focus=false );
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
		bool FileSaveAs( void );
		bool FileSaveAs( wxString filename );
		bool FileSaveGeneric( wxString filename, uint64_t fstart, uint64_t flength );
		bool FileClose( bool WithoutChange = false );
		bool SaveAsDump( void );
		bool FillSelection( void );
		void BlockSelect( void );
		void DoUndo( void );
		void DoRedo( void );
		void OpenMemorySPDDevice( int );

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
		bool IsBlockDevice( void ) {
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
		FAL::FALFileTypes GetFileType( void ) {
			return myfile->GetFileType();
			}
		int HashVerify(wxString hash_file,FAL* File=NULL);

		void FindDialog( void );
		void ReplaceDialog( void );
		void CopyAsDialog( void );
		void GotoDialog( void );

		void UpdateCursorLocation( bool force=false );
		void UpdateStatusBar( void );
		void ConnectScroll(HexEditor* connection);
		void DisconnectScroll( void );

#if _FSWATCHER_
		void OnFileModify(wxFileSystemWatcherEvent &event);
#endif // _FSWATCHER_

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
		void OnOffsetMouse( wxMouseEvent& event );
		void OnResize( wxSizeEvent &event );
		void OnUpdateUI( wxUpdateUIEvent& event );

		void ShowContextMenu( const wxMouseEvent& event );
		void ScrollNoThread( int speed );

		wxStatusBar* statusbar;
		FAL *myfile;
#ifndef DO_NOT_USE_THREAD_FOR_SCROLL
		scrollthread *myscrollthread;
#endif
		DataInterpreter *interpreter;
		InfoPanel *infopanel;
		TagPanel *tagpanel;
		DisassemblerPanel *dasmpanel;
		copy_maker *copy_mark;

	private:
		int64_t BlockSelectOffset;
		bool MouseCapture;
		void Dynamic_Connector( void );
		void Dynamic_Disconnector( void );
	};

class scrollthread:wxThreadHelper {
	private:
		HexEditor *parent;
		int speed;
		wxMutex speed_mtx;
	public:
		wxMutex ThreadScrool;
		scrollthread(int initial_speed, HexEditor *parent ):parent(parent) {
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
			int64_t newoffset=0;
			int64_t FileLength;
			int thread_speed;
			ThreadScrool.Unlock();
			while( !(GetThread()->TestDestroy()) ) {
				speed_mtx.Lock();
				thread_speed=speed;
				speed_mtx.Unlock();
				if(speed == 0) {
					wxMicroSleep( 25 );
					continue;	// loop to "while" for init of class and wait for GetThread()->Pause();
					}
#if _DEBUG_THREAD_SCROLL_
				std::cout << "scrollthread speed : " << thread_speed;
#endif
				FileLength = parent->FileLength();
				newoffset = parent->page_offset + ( parent->BytePerLine() )*thread_speed;
				if( newoffset < 0 )
					newoffset = 0;
				else if( newoffset + parent->ByteCapacity() >= FileLength ) {
					newoffset = FileLength - parent->ByteCapacity();
					newoffset += parent->BytePerLine() - (parent->page_offset % parent->BytePerLine()) ; //cosmetic
					}
#if _DEBUG_THREAD_SCROLL_
				std::cout << " \t- offset : " << newoffset << std::endl;
#endif
				if( newoffset != parent->page_offset ) {
					parent->page_offset=newoffset;

					while( ThreadScrool.TryLock() != wxMUTEX_NO_ERROR )
						GetThread()->Sleep(25);

					wxCommandEvent event( wxEVT_COMMAND_TEXT_UPDATED, THREAD_UPDATE_EVENT );
					//event.SetWillBeProcessedAgain();
					//event.SetInt(n);  // pass some data along the event, a number in this case
					::wxPostEvent( parent, event );
					wxYieldIfNeeded();
					}
				}
#if _DEBUG_THREAD_SCROLL_
			std::cout << "scrollthread got testDestroy!" << std::endl;
#endif
			return NULL;
			}

		void UpdateSpeed(int new_speed) {
#if _DEBUG_THREAD_SCROLL_
			std::cout << "UpdateSpeed:" << new_speed << std::endl;
#endif
			if (new_speed == 0 && speed == 0 )
				return;
			else if(new_speed == 0 && GetThread()->IsRunning() )
				GetThread()->Pause();
			else if( GetThread()->IsPaused() )
				GetThread()->Resume();

			speed_mtx.Lock();
			speed = new_speed;
			speed_mtx.Unlock();
			}

		void Exit(void) {
			if( !GetThread()->IsRunning() ) //We can only "Delete" running threads
				GetThread()->Resume();// Resume the thread for kill it
			GetThread()->Delete();
			}
	};



#endif
