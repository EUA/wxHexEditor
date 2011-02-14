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

#include "FAL.h"
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfNode);

FAL::FAL(wxFileName& myfilename, FileAccessMode FAM ){
	file_access_mode = FAM;
	the_file = myfilename;
	if(myfilename.IsFileReadable()){//FileExists()){
//		if( myfilename.GetFullPath() == wxT("/dev/mem") ){
//			//Ram device in Unix has need special treatment.
//			}
//		else
			{
			if( FAM == ReadOnly)
				Open(myfilename.GetFullPath(), wxFile::read);
			else
				Open(myfilename.GetFullPath(), wxFile::read_write);

			if(! IsOpened()){
				file_access_mode = AccessInvalid;
				wxMessageBox( _("File cannot open."),_("Error"), wxOK|wxICON_ERROR );
				}
			}
		}
	}

FAL::~FAL(){
	DiffArray.Clear();
	}

bool FAL::SetAccessMode( FileAccessMode fam ){
	if( Access( the_file.GetFullPath() , (fam == ReadOnly ? wxFile::read : wxFile::read_write) ) ){
		Close();
		Open( the_file.GetFullPath(), (fam == ReadOnly ? wxFile::read : wxFile::read_write) );
		if(! IsOpened()){
			wxBell();wxBell();wxBell();
			wxMessageBox( _("File load error!.\nFile closed but not opened while access change. For avoid corruption close the program"),_("Error"), wxOK|wxICON_ERROR );
			file_access_mode = AccessInvalid;
			return false;
			}
		file_access_mode = fam;
		return true;
		}
	wxBell();
	wxMessageBox( wxString(_("File cannot open in ")).Append( FAMtoString( fam) ).Append(_(" mode.")),_("Error"), wxOK|wxICON_ERROR );
	return false;
	}

int FAL::GetAccessMode( void ){
	return file_access_mode;
	}

wxString FAL::GetAccessModeString( void ){
	return FAMtoString( file_access_mode );
	}

wxString FAL::FAMtoString( FileAccessMode& FAM ){
	return FAM == ReadOnly ? _T("Read-Only") :
		FAM == ReadWrite ? _T("Read-Write") :
		FAM == DirectWrite ? _T("Direct-Write") : _("Access Invalid");
	}

wxFileName FAL::GetFileName( void ){
	return the_file;
	}

DiffNode* FAL::NewNode( uint64_t start_byte, const char* data, int64_t size, bool inject ){
	DiffNode* newnode = new struct DiffNode( start_byte, size, inject );
	if( size < 0 ){//Deletion!
		newnode->old_data = new char[-size];
		if( newnode->old_data == NULL )
			wxLogError( _("Not Enought RAM") );
		else{
			Seek( start_byte, wxFromStart );
			Read( newnode->old_data, -size );
			return newnode;
			}
		}
	else if( inject ){
		newnode->new_data = new char[size];
		if( newnode->new_data == NULL )
			wxLogError( _("Not Enought RAM") );
		else{
			memcpy( newnode->new_data, data, size);
			return newnode;
			}
		}
	else{// Normal opeariton
		newnode->old_data = new char[size];
		newnode->new_data = new char[size];
		if( newnode->old_data == NULL || newnode->new_data == NULL){
			wxLogError( _("Not Enought RAM") );
			delete newnode->old_data;
			delete newnode->new_data;
			return NULL;
			}
		else{
			memcpy( newnode->new_data, data, size);
			Seek( start_byte, wxFromStart );
			Read( newnode->old_data, size );
			return newnode;
			}
		}
	return NULL;
	}

bool FAL::IsChanged( void ){
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( DiffArray[i]->flag_commit == DiffArray[i]->flag_undo )	// means this node has to be (re)writen to disk
			return true;
	return false;
	}

bool FAL::Apply( void ){
	bool success=true;
	if( file_access_mode != ReadOnly )
		for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ ){
			if( DiffArray[i]->flag_commit == DiffArray[i]->flag_undo ){		// If there is unwriten data
				Seek(DiffArray[i]->start_offset, wxFromStart);			// write it
				success*=Write(DiffArray[i]->new_data, DiffArray[i]->size);
				DiffArray[i]->flag_commit = DiffArray[i]->flag_commit ? false : true;	//alter state of commit flag
				}
		}
	return success;
	}

int64_t FAL::Undo( void ){
	if( DiffArray.GetCount() == 0 ){
		wxBell();						// No possible undo action here bell
		return -1;
		}
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( DiffArray.Item(i)->flag_undo ){		//find first undo node if available
			if( i != 0 ){			//if it is not first node
				DiffArray.Item( i-1 )->flag_undo = true;	//set previous node as undo node
				if( file_access_mode == DirectWrite )		//Direct Write mode is always applies directly.
					Apply();
				return DiffArray.Item( i-1 )->start_offset;//and return undo transaction start for screen focusing
				}
			else{						//if first node is undo
				wxBell();				//ring the bell
				return -1;				//this means there is not undo node available
				}
			}
	DiffArray.Last()->flag_undo=true;
	return DiffArray.Last()->start_offset;
	}

bool FAL::IsAvailable_Undo( void ){
	if( DiffArray.GetCount() == 0 )
		return false;
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( !DiffArray.Item(i)->flag_undo )
			return true;
	return false;
	}

int64_t FAL::Redo( void ){
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( DiffArray.Item(i)->flag_undo ){		//find first undo node if available
			DiffArray.Item(i)->flag_undo = false;
			if( file_access_mode == DirectWrite )	//Direct Write mode is always applies directly.
				Apply();
			return DiffArray.Item(i)->start_offset;
			}
	wxBell();	// No possible redo action
	return -1;
	}

bool FAL::IsAvailable_Redo( void ){
	if( DiffArray.GetCount() == 0 )
		return false;
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( DiffArray.Item(i)->flag_undo )
			return true;
	return false;
	}

void FAL::ShowDebugState( void ){
	std::cout << "\n\nNumber\t" << "Start\t" << "Size\t" << "DataN\t" << "DataO\t" << "writen\t" << "Undo\t" << "Injection\n";
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ ){
		std::cout << i << '\t' << DiffArray[i]->start_offset << '\t' << DiffArray[i]->size << '\t';
		if (DiffArray[i]->size > 0 )
			std::cout << std::hex
			   << (DiffArray[i]->new_data == NULL ? 0 : (unsigned short)DiffArray[i]->new_data[0] )
				<< '\t' << (DiffArray[i]->old_data == NULL ? 0 : (unsigned short)DiffArray[i]->old_data[0] )
				<< '\t' << DiffArray[i]->flag_commit
				<< '\t' << DiffArray[i]->flag_undo
				<< '\t' << DiffArray[i]->flag_inject
				<< std::dec << std::endl;
		else
			std::cout << std::hex <<  0 << '\t' << (unsigned short)DiffArray[i]->old_data[0] << '\t' <<	DiffArray[i]->flag_commit << '\t' << DiffArray[i]->flag_undo  << std::dec << std::endl;
		}
	}

wxFileOffset FAL::Length( void ){
	#ifdef __WXGTK__
		if( the_file.GetFullPath() == wxT("/dev/mem") ){
			return 512*1024*1024;
			}
	#endif
	#ifdef __WXMAC__
		if( the_file.GetFullPath().StartsWith(wxT("/dev/disk") ) ){
			int block_size=0;
			int64_t block_count=0;
			if( ioctl(FD, DKIOCGETBLOCKSIZE, &block_size) and ioctl(FD, DKIOCGETBLOCKCOUNT, &block_count) )
				return block_size*block_count;
			else
				return -1;
			}
	#endif
	if(! IsOpened() )
		return -1;
	wxFileOffset max_size=wxFile::Length();
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( DiffArray[i]->flag_undo and !DiffArray[i]->flag_commit )
			continue;
		else if( DiffArray[i]->flag_inject )
				max_size += DiffArray[i]->size;

	return max_size;
	}

long FAL::Read( char* buffer, int size){
	uint64_t from = Tell();
	int j=0;
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++)
		if( DiffArray[i]->flag_undo and not DiffArray[i]->flag_commit )	// Allready committed to disk, nothing to do here
			break;
		else
			j=i+1;
	return ReadR( buffer, size, from, &DiffArray, j);
	}

//Returns information that if file has some deletions or injections.
bool FAL::IsInjected( void ){
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++)
		if( DiffArray[i]->flag_undo and not DiffArray[i]->flag_commit )
			break;
		else if( DiffArray.Item(i)->flag_inject )
			return true;
	return false;
	}

///State of the Art Read Function
//Only recursion could handle such a complex operation. Comes my mind while I am trying to sleep.
long FAL::ReadR( char* buffer, int size, uint64_t from, ArrayOfNode* PatchArray, int PatchIndice ){
	uint64_t loc = from; //?
	int readsize=0;

	///Getting Data from bellow layer.
	if( PatchIndice == 0 )	//Deepest layer
		{
		wxFile::Seek( from );
		return wxFile::Read( buffer, size ); //Ends recursion. here
		}

	//than process at current layer
	if(PatchIndice != 0){
		DiffNode* patch = PatchArray->Item(PatchIndice-1); //PatchIndice-1 means the last patch item! Don't confuse at upper code.
		if( patch->flag_inject and patch->size < 0 ){//Deletion patch
			readsize = DeletionPatcher( from, buffer, size, PatchArray, PatchIndice );
			}
		else if( patch->flag_inject ){	//Injection patch
			readsize = InjectionPatcher( from, buffer, size, PatchArray, PatchIndice );
			}
		else{
			readsize = ReadR( buffer, size, loc, PatchArray, PatchIndice-1 );//PatchIndice-1 => Makes Patch = 0 for 1 patch. Read from file.
			if(size != readsize) //If there is free chars
				readsize = (Length() - loc > size) ? (size):(Length() - loc);	//check for buffer overflow
			ModificationPatcher( loc, buffer, size, patch );
			}
		}
 	return readsize;
	}

long FAL::InjectionPatcher(uint64_t current_location, char* data, int size, ArrayOfNode* PatchArray, int PatchIndice){
	/******* MANUAL of Code understanding *******
	* current_location                    = [   *
	* current_location + size             = ]   *
	* Injections start_offset             = (   *
	* Injections start_offset + it's size = )   *
	* data                                = ... *
	* first partition                     = xxx *
	********************************************/
	if( PatchArray->Item(PatchIndice-1) -> size > 0 and PatchArray->Item(PatchIndice-1)->flag_inject ){
		DiffNode *Inject_Node = PatchArray->Item(PatchIndice-1);	//For easy reading of code
		///If injection starts behind, so we needed to read bytes from before to show correct location.
		///State ...().,,,[...]..... -> ...(...).[,,,].
		if( Inject_Node->end_offset() < current_location ){
			return ReadR( data, size, current_location - Inject_Node->size, PatchArray, PatchIndice-1 );
			}
		///State ...(),,,[....]..... -> ...(..[xx),,],..
		else if( Inject_Node->start_offset <= current_location
					and Inject_Node->end_offset() > current_location
					and Inject_Node->end_offset() < current_location + size
					){
				int movement = current_location - Inject_Node->start_offset;   // This bytes discarded from injection
				int first_part = Inject_Node->end_offset() - current_location; // First part size
				memcpy( data, Inject_Node->new_data + movement, first_part );  // Copy first part to buffer
				int read_size = ReadR( data+first_part, size-first_part, Inject_Node->start_offset, PatchArray, PatchIndice-1 ); //Than copy second part.
				return read_size+first_part;
			}

		///State ...(),,,[...].).... -> ...(...[...].),,,
		else if( Inject_Node->start_offset <= current_location
					and Inject_Node->end_offset() >= current_location + size
					){
				int movement = current_location - Inject_Node->start_offset;	// This bytes discarded from injection
				memcpy( data, Inject_Node->new_data + movement, size );			// Copy first part to buffer
				return size;
			}
		///State ...[...(),,,]... -> ...[xx(...)...],,,...
		else if( Inject_Node->start_offset > current_location
					and Inject_Node->start_offset < current_location + size
					and Inject_Node->end_offset() < current_location + size
					){
				int first_part = Inject_Node->start_offset - current_location; // First part size
				int read_size = ReadR( data, first_part, current_location, PatchArray, PatchIndice-1 ); //Read First Part from lower layer
				memcpy( data+first_part, Inject_Node->new_data, Inject_Node->size );  // Copy second part to buffer
				read_size += ReadR( data+first_part+Inject_Node->size, size-(first_part+Inject_Node->size), current_location+first_part, PatchArray, PatchIndice-1 ); //Read latest chunk
				return read_size+Inject_Node->size;
			}
		///State ...[...(),,,]... -> ...[xx(...]...),,,...
		else if( Inject_Node->start_offset > current_location
					and Inject_Node->start_offset < current_location + size
					and Inject_Node->end_offset() > current_location + size
					){
				int first_part = Inject_Node->start_offset - current_location; // First part size
				int read_size = ReadR( data, first_part, current_location, PatchArray, PatchIndice-1 ); //Read First Part from lower layer
				memcpy( data+first_part, Inject_Node->new_data, size-first_part );  // Copy second part to buffer
				return read_size+Inject_Node->size;
			}
		///State ...[...]...(...)... -> ...[...]...()...
		else{
			return ReadR( data, size, current_location, PatchArray, PatchIndice-1 );
			}
		}
	return -1;
	}

long FAL::DeletionPatcher(uint64_t current_location, char* data, int size, ArrayOfNode* PatchArray, int PatchIndice){
	/******* MANUAL of Code understanding ******
	* current_location                   = [   *
	* current_location + size            = ]   *
	* Deletes start_offset               = (   *
	* Deletes start_offset + it's size   = )   *
	* data                               = ... *
	* first partition                    = xxx *
	*******************************************/
	if( PatchArray->Item(PatchIndice-1) -> size < 0 ){
		DiffNode *Delete_Node = PatchArray->Item(PatchIndice-1);	//For easy reading of code
		///If deletion starts behind, so we needed to read bytes from after to show correct location.
		///State ...(...)...[...],,,. -> ...()......[,,,].
		///State ...(..[..).].,,,.... -> ...()..[,,,]....
		///State ...(..[...].)..,,,.. -> ...()..[,,,]..
		if( Delete_Node->start_offset <= current_location ){
			return ReadR( data, size, current_location - Delete_Node->size, PatchArray, PatchIndice-1 );
			}
		///State ...[xxx(...)...],,,... -> ...[xxx()...,,,]...
		///State ...[xxx(...]...),,,... -> ...[xxx()...,,,]...
		else if( Delete_Node->start_offset > current_location and Delete_Node->start_offset < current_location + size ){
			int first_part_size = Delete_Node->start_offset - current_location;
			int read_size=0;
			read_size += ReadR( data,						first_part_size,				current_location,															PatchArray,  PatchIndice-1 );
			read_size += ReadR( data+first_part_size, size - first_part_size,		current_location + first_part_size - Delete_Node->size,		PatchArray,  PatchIndice-1 );
			return read_size;
			}
		///State ...[...]...(...)... -> ...[...]...()...
		else{
			return ReadR( data, size, current_location, PatchArray, PatchIndice-1 );
			}
		}
	return -1;
	}

void FAL::ModificationPatcher(uint64_t current_location, char* data, int size, DiffNode* Patch){
	/***** MANUAL of Code understanding *********
	* current_location                    = [   *
	* current_location + size             = ]   *
	* Patch's start_offset                = (   *
	* Patch's start_offset + it's size    = )   *
	* data                                = ... *
	* changed data                        = xxx *
	********************************************/
	if( Patch->flag_undo && !Patch->flag_commit )	// Allready committed to disk, nothing to do here
		return;
	///State: ...[...(xxx]xxx)...
	if(current_location <= Patch->start_offset && current_location+size >= Patch->start_offset){
		int irq_loc = Patch->start_offset - current_location;
		///...[...(xxx)...]... //not neccessery, this line includes this state too
		int irq_size = (size - irq_loc > Patch->size) ? (Patch->size) : (size - irq_loc);
		memcpy(data+irq_loc , Patch->flag_undo ? Patch->old_data : Patch->new_data, irq_size );
		}

	///State: ...(xxx[xxx)...]...
	else if (current_location <= Patch->start_offset + Patch->size && current_location+size >= Patch->start_offset + Patch->size){
		int irq_skipper = current_location - Patch->start_offset;	//skip this bytes from start
		int irq_size = Patch->size - irq_skipper;
		memcpy(data, Patch->flag_undo ? Patch->old_data : Patch->new_data + irq_skipper, irq_size );
		}

	///State: ...(xxx[xxx]xxx)...
	else if(Patch->start_offset <= current_location && Patch->start_offset + Patch->size >= current_location+size){
		int irq_skipper = current_location - Patch->start_offset;	//skip this bytes from start
		memcpy(data, Patch->flag_undo ? Patch->old_data : Patch->new_data + irq_skipper, size );
		}
 	}

bool FAL::Add( uint64_t start_byte, const char* data, int64_t size, bool injection ){
	//Check for undos first
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ ){
		if( DiffArray[i]->flag_undo ){
			if(DiffArray.Item(i)->flag_commit ){	// commited undo node
				DiffArray[i]->flag_undo = false;		//we have to survive this node as it unwriten, non undo node
				DiffArray[i]->flag_commit = false;
				char *temp_buffer = DiffArray[i]->old_data;	//swap old <-> new data
				DiffArray[i]->old_data = DiffArray[i]->new_data;
				DiffArray[i]->new_data = temp_buffer;
				}
			else{									// non committed undo node
				while( i < (DiffArray.GetCount()) ){	// delete beyond here
					#ifdef _DEBUG_FILE_
						std::cout << "DiffArray.GetCount() : " << DiffArray.GetCount() << " while i = " << i<< std::endl;
					#endif
					DiffNode *temp;
					temp = *(DiffArray.Detach( i ));
					delete temp;
					}
				break;								// break for addition
				}
			}
		}
	//Adding node here
	DiffNode *rtn = NewNode( start_byte, data, size, injection );
		if( rtn != NULL ){
			DiffArray.Add( rtn );						//Add new node to tail
			if( file_access_mode == DirectWrite )	//Direct Write mode is always applies directly.
				Apply();
			return true;
			}
		else
			return false;							//if not created successfuly
	}

wxFileOffset FAL::Seek(wxFileOffset ofs, wxSeekMode mode){
	get_ptr = put_ptr = ofs;
	return wxFile::Seek( ofs, mode );
	}


const DiffNode* FAL::GetFirstUndoNode( void ){
	if( DiffArray.GetCount() == 0 )
		return false;
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( !DiffArray.Item(i)->flag_undo )
			return DiffArray.Item(i);
	return NULL;
	}
