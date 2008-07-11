#include "FileDifference.h"
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

FileDifference::FileDifference(wxFileName& myfilename){
	if(myfilename.FileExists()){
		//myfile = new wxFile(wxT("/dev/sda1"), wxFile::read);
		Open(myfilename.GetFullPath(), wxFile::read_write);
		// TODO (death#3#): Add option for open read only
		if(! IsOpened()){
			wxMessageDialog *dlg = new wxMessageDialog(NULL,_("File cannot open."),_("Error"), wxOK|wxICON_ERROR, wxDefaultPosition);
			dlg->ShowModal();dlg->Destroy();
			}
		}
	head = tail = NULL;
	}

FileDifference::~FileDifference(){
	RemoveTail( head );
	head = NULL;
	}

DiffNode* FileDifference::NewNode( int64_t start_byte, const char* data, int64_t size, struct DiffNode *prev , bool extension ){
	DiffNode* newnode = new struct DiffNode;
	newnode->size = size;
	newnode->start_byte = start_byte;
	newnode->old_data = new char[size];
	newnode->new_data = new char[size];
	newnode->writen = false;
	newnode->undo = false;
	newnode->extension = extension;
	newnode->next = NULL;
	newnode->prev = prev;

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
	for( DiffNode *temp = head ; temp != NULL ; temp = temp->next )
		if( temp->writen == temp->undo )	// means this node has to be (re)writen
			return true;
	return false;
	}

bool FileDifference::Apply( void ){
	bool success=true;
	for( DiffNode *temp = head ; temp != NULL ; temp = temp->next ){
		if(!(temp->undo) && !(temp->writen) ){		// If there is unwriten data without undo flag
			Seek(temp->start_byte, wxFromStart);	// write it
			success*=Write(temp->new_data, temp->size);
			temp->writen = true;
			}
		else if( temp->undo && temp->writen){		// If there is writen data with undo flag
			Seek(temp->start_byte, wxFromStart);	// restore it
			success*=Write(temp->old_data, temp->size);
			temp->writen = false;
			}
		}
	return success;
	}

int64_t FileDifference::Undo( void ){
	DiffNode* undo = GetFirstUndoNode();
	if(undo != NULL){					//if there is undo node available
		if(undo->prev == NULL){			//if this not first node
			wxBell();
			return -1;					//this means there is not undo node available
			}
		undo->prev->undo = true;		//set previous node is undo
		return undo->prev->start_byte;	//returning undo transaction start for screen focusing
		}
	else{								//if there is no undo node available
		if(tail != NULL){				//and there is any transactions available
			tail->undo=true;
			return tail->start_byte;
			}
		wxBell();						// No possible undo action here bell
		return -1;
		}
	}

void FileDifference::ShowDebugState( void ){
	int i=0;
	std::cout << "\n\nNumber\t" << "Start\t" << "Size\t" << "DataN\t" << "DataO\t" << "writen\t" << "Undo\n";
	for( DiffNode *temp = head ; temp != NULL ; temp = temp->next ){
		std::cout << ++i << '\t' << temp->start_byte << '\t' << temp->size << '\t' << temp->new_data[0] << '\t'
				<< temp->old_data[0] << '\t' <<	temp->writen << '\t' << temp->undo << std::endl;
		}
	}

DiffNode* FileDifference::GetFirstUndoNode( void ){
	for(DiffNode *temp = head; temp != NULL ; temp = temp->next ){
		if(temp->undo == true )	//if this node is undo node
			return temp;
		}
	return NULL;
	}

int64_t FileDifference::Redo( void ){
	DiffNode* undo = GetFirstUndoNode();
	if(undo != NULL){
		undo->undo = false;
		return undo->start_byte+undo->size;
		}
	wxBell();	// No possible redo action
	return -1;
	}

wxFileOffset FileDifference::Length( void ){
	if(! IsOpened() )
		return -1;
	wxFileOffset max_size=wxFile::Length();
	for( DiffNode *temp = head; temp != NULL ; temp = temp->next ){
		(( temp->start_byte + temp->size ) > max_size ) ? ( max_size = temp->start_byte + temp->size ):( max_size=max_size );
		}
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
	for(DiffNode *temp = head; temp != NULL ; temp = temp->next ){
		/**
		current_location 					= [
		current_location + size 			= ]
		temp->start_byte					= (
		temp->start_byte + temp_node->size	= )
		data								= ...
		changed data						= xxx
		**/
		if(temp->undo && !temp->writen)
			continue;
		//...[...(xxx]xxx)...
		if(current_location <= temp->start_byte && current_location+size >= temp->start_byte){
			int irq_loc = temp->start_byte - current_location;
			//...[...(xxx)...]... //not neccessery, this line includes this state
			int irq_size = (size - irq_loc > temp->size) ? (temp->size) : (size - irq_loc);
			memcpy(data+irq_loc , temp->undo ? temp->old_data : temp->new_data, irq_size );
			}

		//...(xxx[xxx)...]...
		else if (current_location <= temp->start_byte + temp->size && current_location+size >= temp->start_byte + temp->size){
			int irq_skipper = current_location - temp->start_byte;	//skip this bytes from start
			int irq_size = temp->size - irq_skipper;
			memcpy(data, temp->undo ? temp->old_data : temp->new_data + irq_skipper, irq_size );
			}

		//...(xxx[xxx]xxx)...
		else if(temp->start_byte <= current_location && temp->start_byte + temp->size >= current_location+size){
			int irq_skipper = current_location - temp->start_byte;	//skip this bytes from start
			memcpy(data, temp->undo ? temp->old_data : temp->new_data + irq_skipper, size );
			}
		}
	}

void FileDifference::RemoveTail( DiffNode *remove_node ){
	DiffNode* temp;
	if( remove_node == NULL) return;
	do{
		temp=remove_node->next;
		delete remove_node->old_data;
		delete remove_node->new_data;
		delete remove_node;
		remove_node = temp;
		}while(temp != NULL);
	}

bool FileDifference::Add( int64_t start_byte, const char* data, int64_t size, bool extension ){
	DiffNode* temp=head;
	if( head == NULL ){
		tail = head = NewNode(start_byte, data, size, NULL);
		return head;
		}
	else{
		while(temp->next != NULL){
			if(temp->undo == true){			//if this is undo node
				if(temp->writen){			//which has writen data (new_data writen to file)
					temp->undo = false;		//we have to survive this node as it unwriten, non undo node
					temp->writen = false;
					char *temp_buffer = temp->old_data;	//swap old <-> new data
					temp->old_data = temp->new_data;
					temp->new_data = temp_buffer;
					}
				else{						//if this is undo with no writen data, delete it and its tail...
					if(temp->prev == NULL){	//This is head node
						RemoveTail(temp);
						head = temp = NewNode( start_byte, data, size, NULL );
						return temp;
						}
					else{					//if this node is not head
						temp = temp->prev;
						RemoveTail( temp->next );
						temp->next = NewNode( start_byte, data, size, temp );
						return temp->next;
						}
					}
				}
			temp=temp->next;
			}
		tail = temp->next = NewNode(start_byte, data, size, temp );
		return temp;
		}
	}
