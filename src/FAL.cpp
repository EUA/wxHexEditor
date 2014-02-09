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
*               home  : www.wxhexeditor.org                             *
*               email : spamjunkeater@gmail.com                         *
*************************************************************************/

///This file also available under propertieary license.
///Contact author for licensing.

#include "FAL.h"
#include <wx/arrimpl.cpp>

#ifndef __WXMSW__
	#include <sys/ptrace.h> //No ptrace at windows
#endif

#ifdef __WXMAC__
	#define PTRACE_ATTACH PT_ATTACH
	#define PTRACE_DETACH PT_DETACH
	#define PTRACE_PEEKTEXT PT_READ_I
	#define PTRACE_POKETEXT PT_WRITE_I
#endif


WX_DEFINE_OBJARRAY(ArrayOfNode);

bool IsBlockDev( int FD ){
	struct stat *sbufptr = new struct stat;
   fstat( FD, sbufptr );
   if(S_ISBLK( sbufptr->st_mode )
#ifdef __WXMSW__
		or (sbufptr->st_mode==0)	//Enable block size detection code on windows targets,
#endif
		)
		return true;
	return false;
	}

int FDtoBlockSize( int FD ){
	int block_size=0;
#ifdef __WXGTK__
	ioctl(FD, BLKSSZGET, &block_size);
#elif defined (__WXMAC__)
	ioctl(FD, DKIOCGETBLOCKSIZE, &block_size);
#elif defined (__WXMSW__)
	struct stat *sbufptr = new struct stat;
   fstat( FD, sbufptr );
   if(S_ISBLK( sbufptr->st_mode )
#ifdef __WXMSW__
		or (sbufptr->st_mode==0)	//Enable block size detection code on windows targets,
#endif
		){
		DWORD dwResult;
		DISK_GEOMETRY driveInfo;
		DeviceIoControl ( (void*)_get_osfhandle(FD), IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &driveInfo, sizeof (driveInfo), &dwResult, NULL);
		block_size=driveInfo.BytesPerSector;
		}
#endif
	return block_size;
	}

uint64_t FDtoBlockCount( int FD ) {
	uint64_t block_count=0;
#ifdef __WXGTK__
	ioctl(FD, BLKGETSIZE64, &block_count);
	block_count/=FDtoBlockSize( FD );
#elif defined (__WXMAC__)
	ioctl(FD, DKIOCGETBLOCKCOUNT, &block_count);
#elif defined (__WXMSW__)
	DWORD dwResult;
	DISK_GEOMETRY driveInfo;
	DeviceIoControl ( (void*)_get_osfhandle(FD), IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &driveInfo, sizeof (driveInfo), &dwResult, NULL);
	block_count=driveInfo.TracksPerCylinder*driveInfo.SectorsPerTrack*driveInfo.Cylinders.QuadPart;
#endif
	return block_count;
	}

FAL::FAL(wxFileName& myfilename, FileAccessMode FAM, unsigned ForceBlockRW ){
	file_access_mode = FAM;
	the_file = myfilename;
	BlockRWSize = ForceBlockRW;
	ProcessID=-1;
	get_ptr = put_ptr = 0;
	OSDependedOpen( myfilename, FAM, ForceBlockRW  );
#if wxCHECK_VERSION( 2,9,0 )
	if(not myfilename.GetFullPath().Lower().StartsWith( wxT("-pid="))){
	::wxFileSystemWatcher::Add( myfilename.GetFullPath(), wxFSW_EVENT_MODIFY );
#endif
	}

#ifdef __WXMSW__
bool FAL::OSDependedOpen(wxFileName& myfilename, FileAccessMode FAM, unsigned ForceBlockRW){
	//Windows special device opening
	if(myfilename.GetFullPath().StartsWith( wxT(".:"))
		or myfilename.GetFullPath().StartsWith( wxT("\\Device\\Harddisk")) ){

		wxString devnm;
		//wxFileName converts "\\.\E:" to ".:\E:"  so we need to fix this
		if(myfilename.GetFullPath().StartsWith( wxT(".:")))
			devnm = wxString(wxT("\\\\.")) + myfilename.GetFullPath().AfterFirst(':');
		else devnm = myfilename.GetFullPath();

		HANDLE hDevice;
		hDevice = CreateFile( devnm, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		DWORD dwResult;
		DISK_GEOMETRY driveInfo;
		//dev = CreateFile (devnm, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
		DeviceIoControl (hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &driveInfo, sizeof (driveInfo), &dwResult, NULL);
		BlockRWSize=driveInfo.BytesPerSector;
		BlockRWCount=driveInfo.TracksPerCylinder*driveInfo.SectorsPerTrack*driveInfo.Cylinders.QuadPart;

		int fd = _open_osfhandle((long) hDevice, 0);
	#ifdef _DEBUG_
		std::cout<< "Win Device Info:\n" << "Bytes per sector = " <<  BlockRWSize << "\nTotal number of bytes = " << BlockRWCount << std::endl;
	#endif
		wxFile::Attach( fd );
		return true;
		}
	if( not myfilename.IsFileReadable() ){
		wxMessageBox(wxString(_("File is not readable by permissions."))+wxT("\n")+_("Please change file permissons or run this program with root privileges"),_("Error"), wxOK|wxICON_ERROR);
		return false;
		}

	return FALOpen( myfilename, FAM, ForceBlockRW);
	}
#elif defined( __WXGTK__ )

bool FAL::OSDependedOpen(wxFileName& myfilename, FileAccessMode FAM, unsigned ForceBlockRW){
	struct stat fileStat;
   bool DoFileExists = (stat((const char*) myfilename.GetFullPath().fn_str(),&fileStat) >= 0);

	//Handling Memory Process Debugging Here
	if(myfilename.GetFullPath().Lower().StartsWith( wxT("-pid="))){
		long int a;
		myfilename.GetFullPath().Mid(5).ToLong(&a);
		ProcessID=a;
		RAMProcess=true;
		if((ptrace(PTRACE_ATTACH, ProcessID, NULL, NULL)) < 0 ){
			wxMessageBox( _("Process cannot open."),_("Error"), wxOK|wxICON_ERROR );
			ProcessID=-1;
			return false;
			}
		waitpid(ProcessID, NULL, WUNTRACED);
		BlockRWSize=4;
		BlockRWCount=0x800000000000LL/4;
		FAM = ReadOnly;
		return true;
		}

	else if(not DoFileExists){
		wxMessageBox(wxString(_("File does not exists at path:"))+wxT("\n")+myfilename.GetFullPath(),_("Error"), wxOK|wxICON_ERROR);
		return false;
		}

	//Owning file
	else if( not myfilename.IsFileReadable() and DoFileExists ){ // "and myfilename.FileExist()" not used because it's for just plain files, not for block ones.
		if( wxCANCEL == wxMessageBox(wxString(_("File is not readable by permissions."))+wxT("\n")+_("Do you want to own the file?"),_("Error"), wxOK|wxCANCEL|wxICON_ERROR) )
			return false;

		wxArrayString output,errors;

		//Save old owner to update at file close...
		wxExecute( wxT("stat -c %U ")+myfilename.GetFullPath(), output, errors, wxEXEC_SYNC );
		if(output.Count()>0 and errors.Count()==0)
			oldOwner = output[0];//this return root generally :D
		else{
			wxMessageBox(_("Unknown error on \"stat -c %U")+myfilename.GetFullPath()+wxT("\""),_("Error"), wxOK|wxCANCEL|wxICON_ERROR);
			return false;
			}
		//Changing owner of file...
		//I think it's better than changing permissions directly. Doesn't it?
		//Will restore owner on file close.
		wxString cmd;
		if( wxFile::Exists( wxT("/usr/bin/gnomesu")))
			cmd = wxT("gnomesu -u root -c \"chown ");
		else if( wxFile::Exists( wxT("/usr/bin/gksu")))
			cmd = wxT("gksu -u root \"chown ");
		else if( wxFile::Exists( wxT("/usr/bin/gksudo")))
			cmd = wxT("gksudo -u root \"chown ");
		cmd+=wxGetUserId() + wxT(" ")+ myfilename.GetFullPath() +wxT("\"");
	#ifdef _DEBUG_
		std::cout << "Changing permission of " << myfilename.GetFullPath().ToAscii() << std::endl;
		std::cout << cmd.ToAscii() << std::endl;
	#endif
		//wxExecute( cmd , output, errors, wxEXEC_SYNC);
		wxShell( cmd );
		}

	return FALOpen(myfilename, FAM, ForceBlockRW);
	}

#elif defined( __WXOSX__ )
bool FAL::OSDependedOpen(wxFileName& myfilename, FileAccessMode FAM, unsigned ForceBlockRW){
	//Handling Memory Process Debugging Here
	if(myfilename.GetFullPath().Lower().StartsWith( wxT("-pid="))){
		long int a;
		myfilename.GetFullPath().Mid(5).ToLong(&a);
		ProcessID=a;
		RAMProcess=true;
		if((ptrace(PTRACE_ATTACH, ProcessID, NULL, NULL)) < 0 ){
			wxMessageBox( _("Process cannot open."),_("Error"), wxOK|wxICON_ERROR );
			ProcessID=-1;
			return false;
			}
		waitpid(ProcessID, NULL, WUNTRACED);
		BlockRWSize=4;
		FAM == ReadOnly;
		return true;
		}

	if( not myfilename.IsFileReadable() ){
		wxMessageBox(wxString(_("File is not readable by permissions."))+wxT("\n")+_("Please change file permissons or run this program with root privileges"),_("Error"), wxOK|wxICON_ERROR );
		return false;
		}
	return FALOpen(myfilename, FAM, ForceBlockRW);
	}
#endif

bool FAL::FALOpen(wxFileName& myfilename, FileAccessMode FAM, unsigned ForceBlockRW){
	if(myfilename.IsFileReadable()){//FileExists()){
		if( FAM == ReadOnly)
			Open(myfilename.GetFullPath(), wxFile::read);
		else
			Open(myfilename.GetFullPath(), wxFile::read_write);

		if(not IsOpened()){
			file_access_mode = AccessInvalid;
			wxMessageBox( _("File cannot open."),_("Error"), wxOK|wxICON_ERROR );
			return false;
			}
		if(IsBlockDev( wxFile::fd() )){
			BlockRWSize=FDtoBlockSize( wxFile::fd() );
			BlockRWCount=FDtoBlockCount( wxFile::fd() );
			}
		else if( ForceBlockRW )
			BlockRWCount=wxFile::Length()/ForceBlockRW;

		return true;
		}
	else
		return false;
	}

bool FAL::Close(){
			#ifndef __WXMSW__
			if( ProcessID >=0 )
				return ((ptrace(PTRACE_DETACH, ProcessID, NULL, NULL)) >= 0 );
			#endif
			return wxFile::Close();
			};

FAL::~FAL(){
	Close();
#ifndef __WXMSW__
	if(not oldOwner.IsEmpty() ){
		//Will restore owner on file close.
		wxString cmd;
		if( wxFile::Exists( wxT("/usr/bin/gnomesu")))
			cmd = wxT("gnomesu -c \"chown ");
		else if( wxFile::Exists( wxT("/usr/bin/gksu")))
			cmd = wxT("gksu -u root \"chown ");
		else if( wxFile::Exists( wxT("/usr/bin/gksudo")))
			cmd = wxT("gksudo -u root \"chown ");
		cmd += oldOwner + wxT(" ")+ the_file.GetFullPath() +wxT("\"");
#ifdef _DEBUG_
		std::cout << "Changing permission of " << the_file.GetFullPath().ToAscii() << std::endl;
		std::cout << cmd.ToAscii() << std::endl;
#endif
		wxShell( cmd );
		//wxArrayString output,errors;
		//wxExecute( cmd , output, errors, wxEXEC_SYNC);
		wxSleep(1);
		}
#endif
	DiffArray.Clear();
	}

bool FAL::SetAccessMode( FileAccessMode fam ){
	if( ProcessID > 0 ){
		file_access_mode = fam;
		return true;
		}

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
	wxMessageBox( wxString(_("File cannot open in this mode : ")).Append( FAMtoString( fam) ),_("Error"), wxOK|wxICON_ERROR );
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
	if( ProcessID >=0 )
		return wxFileNameFromPath( wxString::Format( wxT("PID:%u"), ProcessID));
	return the_file;
	}

DiffNode* FAL::NewNode( uint64_t start_byte, const char* data, int64_t size, bool inject ){
	DiffNode* newnode = new struct DiffNode( start_byte, size, inject );
	if( size < 0 ){//Deletion!
		newnode->old_data = new unsigned char[-size];
		if( newnode->old_data == NULL )
			wxLogError( _("Not Enought RAM") );
		else{
			Seek( start_byte, wxFromStart );
			Read( newnode->old_data, -size );
			return newnode;
			}
		}
	else if( inject ){
		newnode->new_data = new unsigned char[size];
		if( newnode->new_data == NULL )
			wxLogError( _("Not Enought RAM") );
		else{
			memcpy( newnode->new_data, data, size);
			return newnode;
			}
		}
	else{// Normal opeariton
		newnode->old_data = new unsigned char[size];
		newnode->new_data = new unsigned char[size];
		if( newnode->old_data == NULL || newnode->new_data == NULL){
			wxLogError( _("Not Enought RAM") );
			delete [] newnode->old_data;
			delete [] newnode->new_data;
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

size_t FAL::BlockWrite( unsigned char* buffer, unsigned size ){
	if(BlockRWSize==0)
		return Write( buffer, size );

	if( size % BlockRWSize )
		return false;

	std::cout << "buffer write : " << size << "put_ptr :" << put_ptr << std::endl;
	uint64_t StartSector = put_ptr / BlockRWSize;
	unsigned StartShift = put_ptr - StartSector*BlockRWSize;
	uint64_t EndSector = (put_ptr + size)/BlockRWSize;

	int rd = 0;
	wxFile::Seek(StartSector*BlockRWSize);
	size_t ret = Write(buffer, size);//*= to make update success true or false
	put_ptr+=size;
	return ret;
	}

bool FAL::Apply( void ){
	bool success=true;
	if( file_access_mode != ReadOnly )
		for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ ){
			if( DiffArray[i]->flag_commit == DiffArray[i]->flag_undo ){		// If there is unwriten data
				Seek(DiffArray[i]->start_offset, wxFromStart);					// write it

				if(BlockRWSize > 0){
					uint64_t StartSector = put_ptr / BlockRWSize;
					unsigned StartShift = put_ptr - StartSector*BlockRWSize;
					uint64_t EndSector = (put_ptr + DiffArray[i]->size)/BlockRWSize;
					int rd_size = (EndSector - StartSector + 1 )*BlockRWSize;// +1 for make read least one sector
					char *bfr = new char[rd_size];
					int rd = 0;
					if ( ProcessID >=0 ){
						#ifndef __WXMSW__
						long word=0;
						//unsigned long *ptr = (unsigned long *) buffer;
						while (rd < rd_size) {
							word = ptrace(PTRACE_PEEKTEXT, ProcessID, reinterpret_cast<char*>(StartSector*BlockRWSize+rd), NULL);
							memcpy( bfr+rd , &word, 4);
							rd += 4;
							}
						#endif
						}
					else{
						wxFile::Seek(StartSector*BlockRWSize);
						rd = wxFile::Read( bfr, rd_size);
						}

					if( rd != rd_size ){	//Crucial if  block read error.
						delete [] bfr;
						return false;
						}
					//if already written and makeing undo, than use old_data
					memcpy( bfr+StartShift, (DiffArray[i]->flag_commit ? DiffArray[i]->old_data : DiffArray[i]->new_data), DiffArray[i]->size);

					if ( ProcessID >=0 ){
						int wr=0;
						long word=0;
						#ifndef __WXMSW__
						while (wr < rd_size) {
								memcpy(&word, bfr + wr, sizeof(word));
								if( ptrace(PTRACE_POKETEXT, ProcessID, reinterpret_cast<char*>(StartSector*BlockRWSize+wr), word) == -1 )
									wxMessageBox( _("Error on Write operation to Process RAM"), _("FATAL ERROR") );
								wr += 4;
							}
						success*=true;
						#endif
						}
					else
						success*=Write(bfr, rd_size);//*= to make update success true or false

					delete [] bfr;
					}
				else
					//if already written and makeing undo, than use old_data
					success*=Write((DiffArray[i]->flag_commit ? DiffArray[i]->old_data : DiffArray[i]->new_data), DiffArray[i]->size);

				if( success )
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
	//search for a undo node...
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
	if( file_access_mode == DirectWrite )		//Direct Write mode is always applies directly.
		Apply();
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

int FAL::GetBlockSize( void ){
	return BlockRWSize;
	}

wxFileOffset FAL::Length( void ){
	if( ProcessID >=0 )
		return 0x800000000000LL;

	if ( BlockRWSize > 0 )
		return BlockRWSize*BlockRWCount;

	#ifdef __WXGTK__
		if( the_file.GetFullPath() == wxT("/dev/mem") ){
			return 512*1024*1024;
			}
	#endif
	#ifdef __WXMAC__
		if( the_file.GetFullPath().StartsWith(wxT("/dev/disk") ) ){
			int block_size=0;
			int64_t block_count=0;
			if( ioctl( fd(), DKIOCGETBLOCKSIZE, &block_size) || ioctl(fd(), DKIOCGETBLOCKCOUNT, &block_count) )
				return -1;
			else
				return block_size*block_count;
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

void FAL::SetXORKey( wxMemoryBuffer Key){
	XORview=Key;
	}

wxMemoryBuffer FAL::GetXORKey( void ){
	return XORview;
	}

void FAL::ApplyXOR( unsigned char* buffer, unsigned size, uint64_t from ){
	if( XORview.GetDataLen() ){
		unsigned Xi = from%XORview.GetDataLen(); //calculates keyshift
		for(unsigned int i = 0; i < size; i++){
			buffer[i] ^= XORview[Xi];
			if(Xi++ == XORview.GetDataLen()-1)
				Xi = 0;
			}
		}
	}

long FAL::Read( char* buffer, int size){
	return Read( reinterpret_cast<unsigned char*>(buffer), size);
	}

long FAL::Read( unsigned char* buffer, int size ){
	//Why did I calculate j here? To find active patch indice...
	int j=0;
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++)
		if( DiffArray[i]->flag_undo and not DiffArray[i]->flag_commit )	// Allready committed to disk, nothing to do here
			break;
		else
			j=i+1;

	long ret = ReadR( buffer, size, get_ptr, &DiffArray, j);

	//Encryption layer
	ApplyXOR( buffer, ret, get_ptr );

	//for next read
	get_ptr += ret;

	return ret;
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
long FAL::ReadR( unsigned char* buffer, unsigned size, uint64_t from, ArrayOfNode* PatchArray, int PatchIndice ){
	///Getting Data from bellow layer.
	if( PatchIndice == 0 )	//Deepest layer
		{
		//Block Read/Write mechanism.
		//if( 0 )//for debugging

#ifdef __WXGTK__ //Linux file read speed up hack for Block devices.
		//Block read code just allowed for memory devices under linux.
		//Because you can read arbitrary locations from block devices at linux.
		//Kernel handle the job...
		//This hack increase reading speed from 164MB to 196MB on my SSD with using read 4MB buffer due use of memcmp.
		//(Max disk rw is 230MB/s)
		if( ProcessID >=0 )
#else
		if( BlockRWSize > 0 )
#endif // __WXGTK__
			{
			///NOTE:This function just read +1 more sectors and copies to buffer via memcpy, thus inefficient, at least for SSD's.
			///TODO:Need to read 1 sector at start to bfs, than read to buffer directly and read one more sector to bfr.
			///Than we could copy readed sectors via memcpy which supposed to increase copy speed some percent.

			//Start & end sector and shift calculation.
			uint64_t StartSector = from / BlockRWSize;
			unsigned StartShift = from - (from / BlockRWSize)*BlockRWSize;
			uint64_t EndSector = (from + size)/BlockRWSize;

			if( EndSector > BlockRWCount-1 )
				EndSector = BlockRWCount-1;

		   int rd_size = (EndSector - StartSector + 1)*BlockRWSize; //+1 for read least one sector
			char *bfr = new char[rd_size];
			int rd=0;

			//If reading from a process memory
			if ( ProcessID >=0 ){
				long word=0;
				//unsigned long *ptr = (unsigned long *) buffer;
				#ifndef __WXMSW__
				while (rd < rd_size) {
					word = ptrace(PTRACE_PEEKTEXT, ProcessID, reinterpret_cast<char*>(StartSector*BlockRWSize+rd), NULL);
					memcpy( bfr+rd , &word, 4);
					rd += 4;
					}
				#endif
				}
			//Reading from a file
			else
				{
				wxFile::Seek(StartSector*BlockRWSize);
				rd = wxFile::Read( bfr, rd_size);
				}
			//Here, we adjust shifting by copying bfr to buffer. Inefficient but easy to programe.
			memcpy(buffer, bfr+StartShift, wxMin(wxMin( rd, rd_size-StartShift) , size)); //wxMin protects file ends.
			delete [] bfr;
			return wxMin(wxMin( rd, rd_size-StartShift), size);
			}

		wxFile::Seek( from ); //Since this is the Deepest layer
		return wxFile::Read( buffer, size ); //Ends recursion. here
		}

	int readsize=0;
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
			readsize = ReadR( buffer, size, from, PatchArray, PatchIndice-1 );//PatchIndice-1 => Makes Patch = 0 for 1 patch. Read from file.
			if(size != readsize) //If there is free chars
				readsize = (Length() - from > size) ? (size):(Length() - from);	//check for buffer overflow
			ModificationPatcher( from, buffer, size, patch );
			}
		}
 	return readsize;
	}

long FAL::InjectionPatcher(uint64_t current_location, unsigned char* data, int size, ArrayOfNode* PatchArray, int PatchIndice){
	/******* MANUAL of Code Understanding *******
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

long FAL::DeletionPatcher(uint64_t current_location, unsigned char* data, int size, ArrayOfNode* PatchArray, int PatchIndice){
	/******* MANUAL of Code Understanding ******
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

void FAL::ModificationPatcher(uint64_t current_location, unsigned char* data, int size, DiffNode* Patch){
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
				unsigned char *temp_buffer = DiffArray[i]->old_data;	//swap old <-> new data
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
	ApplyXOR( rtn->new_data, size, start_byte );
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
	if(!IsOpened())
		return -1;

	get_ptr = put_ptr = ofs;

	if( BlockRWSize > 0 )
		return ofs;

	return wxFile::Seek( ofs, mode );
	}

const DiffNode* FAL::GetFirstUndoNode( void ){
	if( DiffArray.GetCount() == 0 )
		return NULL;
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( !DiffArray.Item(i)->flag_undo )
			return DiffArray.Item(i);
	return NULL;
	}

