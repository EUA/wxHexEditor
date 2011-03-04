/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
*   This file is File Abraction Layer for wxHexEditor                   *
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

///This file also available under propertieary license.
///Contact admin for licensing it.

// Journaling File Structure :)
#ifndef FAL_H
#define FAL_H
#include <iostream>	//for std::cout...
#include <wx/wx.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/dynarray.h>
#include <stdint.h>

#ifdef __WXMAC__
	#include <sys/disk.h>
#endif

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
		inline uint64_t end_offset( void ){ return start_offset + abs(size); }
//	DiffNode *prev, *next;
	DiffNode( uint64_t start, int64_t size_, bool inject){
		start_offset = start;
		flag_inject = inject;
		size = size_;
		flag_commit = flag_undo = false;
		old_data = new_data = NULL;
		}
	~DiffNode( void ){
		delete [] new_data;
		delete [] old_data;
		}
	};

WX_DECLARE_OBJARRAY(DiffNode *, ArrayOfNode);

class FAL : private wxFile{
	public:
	enum FileAccessMode { ReadOnly, ReadWrite, DirectWrite, AccessInvalid };
	    FAL(wxFileName& myfilename, FileAccessMode FAM = ReadOnly, bool is_block_dev=false);
		~FAL();
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

		wxFileOffset Seek(wxFileOffset ofs, wxSeekMode mode = wxFromStart);
		bool Close(){ return wxFile::Close();};
		bool IsOpened(){ return wxFile::IsOpened(); };
		int  fd() const { return wxFile::fd(); };
		long Read( char* buffer, int size );

		bool Add( uint64_t start_byte, const char* data, int64_t size, bool injection=false ); //adds new node
		bool IsAvailable_Undo( void );
		bool IsAvailable_Redo( void );
		bool IsInjected( void );
		const DiffNode* GetFirstUndoNode( void );
		int GetBlockSize( void );

	protected:
		long ReadR( char* buffer, int size, uint64_t location, ArrayOfNode *Patches, int PatchIndice );

		void RemoveTail( DiffNode *remove_node );	//remove further tails.
		DiffNode* NewNode( uint64_t start_byte, const char* data, int64_t size, bool extension = false );
		long DeletionPatcher( uint64_t location, char* data, int size, ArrayOfNode *Patches, int PatchIndice);
		long InjectionPatcher( uint64_t location, char* data, int size, ArrayOfNode *Patches, int PatchIndice);
		void ModificationPatcher( uint64_t location, char* data, int size, DiffNode* Patch);

	private:
		int BlockSize;
		uint64_t BlockCount;
		FileAccessMode file_access_mode;
		ArrayOfNode DiffArray;
		ArrayOfNode TempDiffArray;
		wxFileName the_file;
		uint64_t put_ptr,get_ptr;
//		DiffNode *head,*tail;	//linked list holds modification record

};

#endif // FAL_H
