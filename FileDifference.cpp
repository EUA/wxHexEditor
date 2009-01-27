/***********************************(GPL)********************************
*	wxHexEditor is a hex edit tool for editing massive files in Linux   *
*	Copyright (C) 2006  Erdem U. Altinyurt                              *
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
#include "FileDifference.h"
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfNode);

FileDifference::FileDifference(wxFileName& myfilename, FileAccessMode FAM ){
	file_access_mode = FAM;
	the_file = myfilename;
	if(myfilename.IsFileReadable()){//FileExists()){
		if( FAM == ReadOnly)
			Open(myfilename.GetFullPath(), wxFile::read);
		else
			Open(myfilename.GetFullPath(), wxFile::read_write);

		if(! IsOpened()){
			file_access_mode = AccessInvalid;
			wxMessageDialog *dlg = new wxMessageDialog(NULL,_("File cannot open."),_("Error"), wxOK|wxICON_ERROR, wxDefaultPosition);
			dlg->ShowModal();dlg->Destroy();
			}
		}
	}

FileDifference::~FileDifference(){
	DiffArray.Clear();
	}

bool FileDifference::SetAccessMode( FileAccessMode fam ){
	if( Access( the_file.GetFullPath() , (fam == ReadOnly ? wxFile::read : wxFile::read_write) ) ){
		Close();
		Open( the_file.GetFullPath(), (fam == ReadOnly ? wxFile::read : wxFile::read_write) );
		if(! IsOpened()){
			wxBell();wxBell();wxBell();
			wxMessageDialog *dlg = new wxMessageDialog(NULL,_("File load error!.\nFile closed but not opened while access change. For avoid corruption close the program"),_("Error"), wxOK|wxICON_ERROR, wxDefaultPosition);
			dlg->ShowModal();dlg->Destroy();
			file_access_mode = AccessInvalid;
			return false;
			}
		file_access_mode = fam;
		return true;
		}
	wxBell();
	wxMessageDialog *dlg = new wxMessageDialog(NULL,_("File cannot open in this mode."),_("Error"), wxOK|wxICON_ERROR, wxDefaultPosition);
	dlg->ShowModal();dlg->Destroy();
	return false;
	}

int FileDifference::GetAccessMode( void ){
	return file_access_mode;
	}

wxString FileDifference::GetAccessModeString( void ){
	return file_access_mode == ReadOnly ? _T("Read-Only") :
		file_access_mode == ReadWrite ? _T("Read-Write") :
		file_access_mode == DirectWrite ? _T("Direct-Write") : _("Error!");
	}

wxFileName FileDifference::GetFileName( void ){
	return the_file;
	}

DiffNode* FileDifference::NewNode( int64_t start_byte, const char* data, int64_t size, bool sizechange ){
	DiffNode* newnode = new struct DiffNode;
	newnode->size = size;
	newnode->start_offset = start_byte;
	newnode->old_data = new char[size];
	newnode->new_data = new char[size];
	newnode->flag_commit = false;
	newnode->flag_undo = false;
//	newnode->flag_sizechange = sizechange;
//	newnode->next = NULL;
//	newnode->prev = prev;

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

bool FileDifference::IsChanged( void ){
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( DiffArray[i]->flag_commit == DiffArray[i]->flag_undo )	// means this node has to be (re)writen to disk
			return true;
	return false;
	}

bool FileDifference::Apply( void ){
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

int64_t FileDifference::Undo( void ){
	if( DiffArray.GetCount() == 0 ){
		wxBell();						// No possible undo action here bell
		return -1;
		}
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( DiffArray.Item(i)->flag_undo ){		//find first undo node if available
			if( i - 1 >= 0 ){			//if it is not first node
				DiffArray.Item( i-1 )->flag_undo = true;	//set previous node as undo node
				return DiffArray.Item( i-1 )->start_offset;	//and return undo transaction start for screen focusing
				}
			else{						//if first node is undo
				wxBell();				//ring the bell
				return -1;				//this means there is not undo node available
				}
			}
	DiffArray.Last()->flag_undo=true;
	DiffArray.Last()->start_offset;
	}

void FileDifference::ShowDebugState( void ){
	std::cout << "\n\nNumber\t" << "Start\t" << "Size\t" << "DataN\t" << "DataO\t" << "writen\t" << "Undo\n";
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ ){
		std::cout << i << '\t' << DiffArray[i]->start_offset << '\t' << DiffArray[i]->size << '\t' << std::hex << (unsigned short)DiffArray[i]->new_data[0] << '\t'
				<< (unsigned short)DiffArray[i]->old_data[0] << '\t' <<	DiffArray[i]->flag_commit << '\t' << DiffArray[i]->flag_undo  << std::dec << std::endl;
		}
	}

int64_t FileDifference::Redo( void ){
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( DiffArray.Item(i)->flag_undo ){		//find first undo node if available
			DiffArray.Item(i)->flag_undo = false;
			return DiffArray.Item(i)->start_offset;
			}
	wxBell();	// No possible redo action
	return -1;
	}

wxFileOffset FileDifference::Length( void ){
	if(! IsOpened() )
		return -1;
	wxFileOffset max_size=wxFile::Length();
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		max_size = (( DiffArray[i]->start_offset + DiffArray[i]->size ) > max_size ) ? ( DiffArray[i]->start_offset + DiffArray[i]->size ):( max_size );
	return max_size;
	}

long FileDifference::Read( char* buffer, int size ){
	int64_t loc = Tell();
	int readsize = wxFile::Read(buffer,size);	//Reads file as wxFile::Lenght
	if(size != readsize)				//If there is free chars
		readsize = (Length() - loc > size) ? (size):(Length() - loc);	//check for buffer overflow
	FileIRQ( loc, buffer, size);
	return readsize;
	}

void FileDifference::FileIRQ(int64_t current_location, char* data, int size){
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ ){
		/** MANUAL of Code understanding**
		current_location 						= [
		current_location + size 				= ]
		DiffArray[i]'s start_offset				= (
		DiffArray[i]'s start_offset + it's size	= )
		data									= ...
		changed data							= xxx
		**/
		if( DiffArray[i]->flag_undo && !DiffArray[i]->flag_commit )	// Allready committed to disk, nothing to do here
			continue;

		//State: ...[...(xxx]xxx)...
		if(current_location <= DiffArray[i]->start_offset && current_location+size >= DiffArray[i]->start_offset){
			int irq_loc = DiffArray[i]->start_offset - current_location;
			//...[...(xxx)...]... //not neccessery, this line includes this state
			int irq_size = (size - irq_loc > DiffArray[i]->size) ? (DiffArray[i]->size) : (size - irq_loc);
			memcpy(data+irq_loc , DiffArray[i]->flag_undo ? DiffArray[i]->old_data : DiffArray[i]->new_data, irq_size );
			}

		//State: ...(xxx[xxx)...]...
		else if (current_location <= DiffArray[i]->start_offset + DiffArray[i]->size && current_location+size >= DiffArray[i]->start_offset + DiffArray[i]->size){
			int irq_skipper = current_location - DiffArray[i]->start_offset;	//skip this bytes from start
			int irq_size = DiffArray[i]->size - irq_skipper;
			memcpy(data, DiffArray[i]->flag_undo ? DiffArray[i]->old_data : DiffArray[i]->new_data + irq_skipper, irq_size );
			}

		//State: ...(xxx[xxx]xxx)...
		else if(DiffArray[i]->start_offset <= current_location && DiffArray[i]->start_offset + DiffArray[i]->size >= current_location+size){
			int irq_skipper = current_location - DiffArray[i]->start_offset;	//skip this bytes from start
			memcpy(data, DiffArray[i]->flag_undo ? DiffArray[i]->old_data : DiffArray[i]->new_data + irq_skipper, size );
			}
		}
	}

bool FileDifference::Add( int64_t start_byte, const char* data, int64_t size, bool extension ){
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
				while( i <= DiffArray.GetCount() ){	// delete beyond here
					DiffNode *temp;
					temp = *(DiffArray.Detach( i ));
					delete temp;
					}
				break;								// break for addition
				}
			}
		}
	DiffNode *rtn = NewNode( start_byte, data, size );
		if( rtn != NULL ){
			DiffArray.Add( rtn );					//Add new node to tail
			return true;
			}
		else
			return false;							//if created successfuly
	}
