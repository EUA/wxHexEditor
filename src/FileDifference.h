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
*               email : death_knight at gamebox.net                     *
*************************************************************************/
// Journaling File Structure :)
#ifndef FILEDIFFERENCE_H
#define FILEDIFFERENCE_H
#include <iostream>	//for std::cout...
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/dynarray.h>
#include <stdint.h>
#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

class DiffNode{
	public:
		bool flag_undo;			//For undo
		bool flag_commit;			//For undo
		bool flag_inject;			//this is for file size extension. If this flag is true, that means a new data inserted to file
		uint64_t start_offset;	//start byte of node
		int64_t size;				//size of node
		char *old_data;			//old data buffer
		char *new_data;			//new data buffer
//	DiffNode *prev, *next;
	DiffNode( uint64_t start, int64_t size_, bool inject){
		start_offset = start;
		flag_inject = inject;
		size = size_;
		flag_commit = flag_undo = false;
		old_data = new_data = NULL;
		}
	~DiffNode( void ){
		delete new_data;
		delete old_data;
		}
	};

WX_DECLARE_OBJARRAY(DiffNode *, ArrayOfNode);

class FileDifference : public wxFile{
	public:
	enum FileAccessMode { ReadOnly, ReadWrite, DirectWrite, AccessInvalid };
	    FileDifference(wxFileName& myfilename, FileAccessMode FAM = ReadOnly);
		~FileDifference();
//		friend class FindDialog;

		bool SetAccessMode( FileAccessMode fam );
		int GetAccessMode( void );
		wxString GetAccessModeString( void );
		wxString FAMtoString( FileAccessMode& FAM );
		wxFileName GetFileName( void );

		bool IsChanged( void );	//returns if file is dirty or not
		bool Apply( void );		//flush changes to file
		int64_t Undo( void );	//undo last action
		int64_t Redo( void );	//redo last undo
		void ShowDebugState( void );
		wxFileOffset Length( void );
		long Read( char* buffer, int size );
		bool ReadByte( char* buffer, uint64_t location );
		bool Add( uint64_t start_byte, const char* data, int64_t size, bool injection=false ); //adds new node
		bool IsAvailable_Undo( void );
		bool IsAvailable_Redo( void );

	protected:
		void RemoveTail( DiffNode *remove_node );	//remove further tails.
		DiffNode* NewNode( uint64_t start_byte, const char* data, int64_t size, bool extension = false );
		DiffNode* GetFirstUndoNode( void );
		void FileIRQ( uint64_t location, char* data, int size);
		int64_t GetByteMovements( uint64_t before_than );

	private:
		FileAccessMode file_access_mode;
		ArrayOfNode DiffArray;
		ArrayOfNode TempDiffArray;
		wxFileName the_file;
//		DiffNode *head,*tail;	//linked list holds modification record

};

#endif // FILEDIFFERENCE_H
