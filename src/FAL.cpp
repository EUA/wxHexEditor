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

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/param.h> //defines BSD
#endif

#if defined(__WXMAC__) || defined(BSD)
	#define PTRACE_ATTACH PT_ATTACH
	#define PTRACE_DETACH PT_DETACH
	#define PTRACE_PEEKTEXT PT_READ_I
	#define PTRACE_POKETEXT PT_WRITE_I
#endif

//#if defined BSD
//	#define DKIOCGETBLOCKSIZE DIOCGSECTORSIZE	/* Get the sector size of the device in bytes */
//	#define DKIOCGETBLOCKCOUNT DIOCGMEDIASIZE	/* Get media size in bytes */
//#endif


WX_DEFINE_OBJARRAY(ArrayOfNode);

bool IsBlockDev( int FD ){
    struct stat *sbufptr = new struct stat;
    fstat( FD, sbufptr );
#ifdef __WXMSW__
	return sbufptr->st_mode==0;	//Enable block size detection code on windows targets,
#elif defined (BSD)
    //return S_ISBLK( sbufptr->st_mode ); //this not working on BSD
    return S_ISCHR( sbufptr->st_mode ); //well, this works...
    //return !S_ISREG( sbufptr->st_mode ); //if not an ordinary file, it might be block...
#else
	return S_ISBLK( sbufptr->st_mode );
#endif
	}

int FDtoBlockSize( int FD ){
	int block_size=0;
#if defined(__linux__)
	ioctl(FD, BLKSSZGET, &block_size);
#elif defined (__WXMAC__)
	ioctl(FD, DKIOCGETBLOCKSIZE, &block_size);
#elif defined (BSD)
    ioctl(FD, DIOCGSECTORSIZE, &block_size);
#elif defined (__WXMSW__)
	struct stat *sbufptr = new struct stat;
   fstat( FD, sbufptr );
   if(sbufptr->st_mode==0){	//Enable block size detection code on windows targets,
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
#if defined(__linux__)
	ioctl(FD, BLKGETSIZE64, &block_count);
	block_count/=FDtoBlockSize( FD );
#elif defined (__WXMAC__)
	ioctl(FD, DKIOCGETBLOCKCOUNT, &block_count);
#elif defined (BSD)
    ioctl(FD, DIOCGMEDIASIZE, &block_count);
    block_count/=FDtoBlockSize(FD);
#elif defined (__WXMSW__)
	DWORD dwResult;
	DISK_GEOMETRY driveInfo;
	DeviceIoControl ( (void*)_get_osfhandle(FD), IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &driveInfo, sizeof (driveInfo), &dwResult, NULL);
	block_count=driveInfo.TracksPerCylinder*driveInfo.SectorsPerTrack*driveInfo.Cylinders.QuadPart;
#endif
	return block_count;
	}

bool DiffNode::Apply( unsigned char *data_buffer, int64_t size, int64_t irq_skip ){
	if(virtual_node==NULL){
		memcpy(data_buffer, flag_undo ? old_data : new_data+irq_skip, size );
		return true;
		}
	virtual_node->Seek( virtual_node_start_offset+irq_skip, wxFromStart);
	virtual_node->Read( data_buffer, size );
	return true;
	}

FAL::FAL(wxFileName& myfilename, FileAccessMode FAM, unsigned ForceBlockRW ){
	file_access_mode = FAM;
	the_file = myfilename;
	FileLock=false;
	BlockRWSize = ForceBlockRW;
	ProcessID=-1;
	get_ptr = put_ptr = 0;
	if(myfilename.GetFullPath().Lower().StartsWith( wxT("-buf"))){
		memset( internal_file_buffer.GetWriteBuf(1024), 0 , 1024 );
		internal_file_buffer.UngetWriteBuf(1024);
		FileType=FAL_Buffer;
		}
    else
        OSDependedOpen( myfilename, FAM, ForceBlockRW  );
	}

#ifdef __WXMSW__
HANDLE GetDDK(PCWSTR a);

bool IsWinDevice( wxFileName myfilename ){
	if(myfilename.GetFullPath().StartsWith( wxT(".:"))
		or myfilename.GetFullPath().StartsWith( wxT("\\Device\\Harddisk")) )
		return true;
	return false;
	}

bool FAL::OSDependedOpen(wxFileName& myfilename, FileAccessMode FAM, unsigned ForceBlockRW){
	//Windows special device opening
	std::cout << "WinOSDepOpen"<< std::endl;
		//Handling Memory Process Debugging Here
	if(myfilename.GetFullPath().Lower().StartsWith( wxT("-pid="))){
		long int a;
		myfilename.GetFullPath().Mid(5).ToLong(&a);
		ProcessID=a;
		RAMProcess=true;
		//HANDLE WINAPI OpenProcess(  _In_ DWORD dwDesiredAccess,  _In_ BOOL  bInheritHandle,  _In_ DWORD dwProcessId );
		hDevice = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
		if( hDevice == NULL ){
			wxMessageBox( wxString::Format(_("Process ID:%d cannot be open."),ProcessID ),_("Error"), wxOK|wxICON_ERROR );
			ProcessID=-1;
			return false;
			}
		//waitpid(ProcessID, NULL, WUNTRACED);
		BlockRWSize=4;
		//BlockRWCount=0x800000000000LL/4;
		FAM = ReadOnly;
		return true;
		}

	else if(IsWinDevice(myfilename)){
		//wxFileName converts "\\.\E:" to ".:\E:"  so we need to fix this
		if(myfilename.GetFullPath().StartsWith( wxT(".:")))
			devnm = wxString(wxT("\\\\.")) + myfilename.GetFullPath().AfterFirst(':');
		else
			devnm = myfilename.GetFullPath();
		//devnm=wxT("\\Device\\HarddiskVolume1");

		DWORD dwResult;

		std::wcout << "WinDevice" << devnm << std::endl;
		if( myfilename.GetFullPath().StartsWith("\\Device") ){
			//hDevice=GetDDK(devnm);
			//std::cout << hDevice << std::endl;
			int nDosLinkCreated = wdd.FakeDosNameForDevice (devnm.wchar_str(), szDosDevice, szCFDevice, FALSE);
			devnm=szCFDevice;
			}

		if( FAM == ReadOnly){
			hDevice = CreateFile (devnm, GENERIC_READ,
											FILE_SHARE_READ | FILE_SHARE_WRITE,
											NULL,
											OPEN_EXISTING,
											FILE_FLAG_NO_BUFFERING | FILE_ATTRIBUTE_READONLY  | FILE_FLAG_RANDOM_ACCESS,
											NULL);
			//Check if drive is mounted
		//#ifdef FSCTL_IS_VOLUME_MOUNTED //not available on Win???
			if(!DeviceIoControl (hDevice, FSCTL_IS_VOLUME_MOUNTED, NULL, 0, NULL, 0, &dwResult, NULL)
				&& (!devnm.StartsWith("\\\\.\\PhysicalDrive" ) ) //PhysicalDrive can not checked since it's not logical volume to mount.
				){
				DWORD err = GetLastError ();
				std::cout << err << std::endl;
				wxMessageBox( _("Device is not mounted"), _("Error"), wxOK|wxICON_ERROR );
				return false;
				}
		//#endif //FSCTL_IS_VOLUME_MOUNTED
			}

		else{
		//	hDevice = CreateFile( devnm, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			hDevice = CreateFile( devnm, GENERIC_READ | GENERIC_WRITE,
											FILE_SHARE_READ | FILE_SHARE_WRITE,
											NULL,
											OPEN_EXISTING,
											FILE_FLAG_NO_BUFFERING | FILE_FLAG_WRITE_THROUGH | FILE_FLAG_RANDOM_ACCESS,
											NULL);

//			 //lock volume
//			if (!DeviceIoControl (hDevice, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL))
//				wxMessageBox( wxString::Format( wxT("Error %d attempting to lock volume: %s"), GetLastError (), devnm) );
//
//			//Dismount
//			if (!DeviceIoControl (hDevice, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL))
//				wxMessageBox( wxString::Format( wxT("Error %d attempting to dismount volume: %s"), GetLastError (), devnm) );
			}

		if(hDevice==INVALID_HANDLE_VALUE) // this may happen if another program is already reading from disk
			{
			std::cout << "Device cannot open due invalid handle : " << hDevice << std::endl;
			CloseHandle(hDevice);
			return false;
			}

		DISK_GEOMETRY driveInfo;

		DeviceIoControl (hDevice, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &driveInfo, sizeof (driveInfo), &dwResult, NULL);
		BlockRWSize=driveInfo.BytesPerSector;
		BlockRWCount=driveInfo.TracksPerCylinder*driveInfo.SectorsPerTrack*driveInfo.Cylinders.QuadPart;

		int fd = _open_osfhandle(reinterpret_cast<intptr_t>(hDevice), 0);
	#ifdef _DEBUG_
		std::cout<< "Win Device Info:\n" << "Bytes per sector = " <<  BlockRWSize << "\nTotal number of bytes = " << BlockRWCount << std::endl;
	#endif

		wxFile::Attach( fd );
		return true;
		}
	if( !myfilename.IsFileReadable() ){
		wxMessageBox(wxString(_("File is not readable by permissions."))+wxT("\n")+_("Please change file permissions or run this program with Windows UAC privileges."),_("Error"), wxOK|wxICON_ERROR);
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
		if((ptrace(PTRACE_ATTACH, ProcessID, NULL, 0)) < 0 ){
			wxMessageBox( wxString::Format(_("Process ID:%d cannot be open."),ProcessID ),_("Error"), wxOK|wxICON_ERROR );
			ProcessID=-1;
			return false;
			}
		waitpid(ProcessID, NULL, WUNTRACED);
		BlockRWSize=4;
		BlockRWCount=0x800000000000LL/4;
		FAM = ReadOnly;
		FileType=FAL_Process;
		return true;
		}

	else if(!DoFileExists){
		wxMessageBox(wxString(_("File does not exists at path:"))+wxT("\n")+myfilename.GetFullPath(),_("Error"), wxOK|wxICON_ERROR);
		return false;
		}

	//Owning file
	else if( !myfilename.IsFileReadable() && DoFileExists ){ // "and myfilename.FileExist()" not used because it's for just plain files, not for block ones.
		if( wxCANCEL == wxMessageBox(wxString(_("File is not readable by permissions."))+ wxT("\n")+
												        _("Please change file permissions or run this program with root privileges.")+wxT("\n")+
												        _("You can also try to own the file temporarily (requires root's password.)")+wxT("\n")+wxT("\n")+
												        _("Do you want to own the file?"),_("Warning"), wxOK|wxCANCEL|wxICON_WARNING) )

			return false;

		wxArrayString output,errors;

		//Save old owner to update at file close...
		wxExecute( wxT("stat -c %U ")+myfilename.GetFullPath(), output, errors, wxEXEC_SYNC );
		if(output.Count()>0 && errors.Count()==0)
			oldOwner = output[0];//this return root generally :D
		else{
			wxMessageBox(_("Unknown error on \"stat -c %U")+myfilename.GetFullPath()+wxT("\""),_("Error"), wxOK|wxCANCEL|wxICON_ERROR);
			return false;
			}
		//Changing owner of file...
		//I think it's better than changing permissions directly. Doesn't it?
		//Will restore owner on file close.
		wxString cmd, spacer = wxT(" ");
		if ( wxFile::Exists( wxT("/usr/bin/pkexec"))){
			cmd = wxT("pkexec --user root chown \"");
			spacer = wxT("\" \"");
		}
		else if( wxFile::Exists( wxT("/usr/bin/gnomesu")))
			cmd = wxT("gnomesu -u root -c \"chown ");
		else if( wxFile::Exists( wxT("/usr/bin/gksu")))
			cmd = wxT("gksu -u root \"chown ");
		else if( wxFile::Exists( wxT("/usr/bin/gksudo")))
			cmd = wxT("gksudo -u root \"chown ");
		else{
			wxMessageBox(_("For using this function, please install \"pkexec\", \"gnomesu\" or \"gksu\" tools first."),_("Error"), wxOK|wxCANCEL|wxICON_ERROR);
			return false;
			}
		cmd+=wxGetUserId() + spacer + myfilename.GetFullPath() +wxT("\"");
	#ifdef _DEBUG_
		std::cout << "Changing permission of " << myfilename.GetFullPath().ToAscii() << std::endl;
		std::cout << cmd.ToAscii() << std::endl;
	#endif
		//wxExecute( cmd , output, errors, wxEXEC_SYNC);
		wxShell( cmd );
		}
    FileType=FAL_File;
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
			wxMessageBox( wxString::Format(_("Process ID:%d cannot be open."),ProcessID ),_("Error"), wxOK|wxICON_ERROR );
			ProcessID=-1;
			return false;
			}
		waitpid(ProcessID, NULL, WUNTRACED);
		BlockRWSize=4;
		FAM = ReadOnly;
		return true;
		}

	if( !myfilename.IsFileReadable() ){
		wxMessageBox(wxString(_("File is not readable by permissions."))+wxT("\n")+_("Please change file permissions or run this program with root privileges"),_("Error"), wxOK|wxICON_ERROR );
		return false;
		}
	return FALOpen(myfilename, FAM, ForceBlockRW);
	}
#endif

bool FAL::FALOpen(wxFileName& myfilename, FileAccessMode FAM, unsigned ForceBlockRW){
#ifdef _DEBUG_
    std::cout<< "FAL:FALOpen( " << myfilename.GetFullPath().ToAscii() << " )" << std::endl;
#endif // _DEBUG_
	if(myfilename.IsFileReadable()){//FileExists()){
		if( FAM == ReadOnly)
			Open(myfilename.GetFullPath(), wxFile::read);
		else
			Open(myfilename.GetFullPath(), wxFile::read_write);

		if(!IsOpened()){
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
	else if(FileType==FAL_Buffer)
		return true;
	else
		return false;
	}

bool FAL::Close(){
	if( ProcessID >=0 )
	#ifdef __WXMSW__
		CloseHandle(hDevice);
	 #else
		return ((ptrace(PTRACE_DETACH, ProcessID, NULL, 0)) >= 0 );
	#endif
	#ifdef __WXMSW__
	if(IsWinDevice( the_file ) ){
		DWORD dwResult;

		// unlock volume
		//DeviceIoControl (hDevice, FSCTL_UNLOCK_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL);

		wdd.RemoveFakeDosName(devnm.wchar_str(), szDosDevice);
		wxFile::Detach();
		//_close( reinterpret_cast<int>(hDevice) );
		CloseHandle( hDevice ); //Not neccessary, already closed by _close

		//mount
		//if (!DeviceIoControl (hDevice, FSCTL_MOUNT_VOLUME, NULL, 0, NULL, 0, &dwResult, NULL)){
		//	DWORD err = GetLastError ();
		//	wxMessageBox( wxString::Format( wxT("Error %d attempting to dismount volume: %s"), err, devnm) );
		//	}

		hDevice=NULL;
		return true;
		}
	#endif // __WXMSW__
	return wxFile::Close();
	};

FAL::~FAL(){
	Close();
#ifndef __WXMSW__
	if(!oldOwner.IsEmpty() ){
		//Will restore owner on file close.
		wxString cmd, spacer = wxT(" ");
		if ( wxFile::Exists( wxT("/usr/bin/pkexec"))){
			cmd = wxT("pkexec --user root chown \"");
			spacer = wxT("\" \"");
		}
		else if( wxFile::Exists( wxT("/usr/bin/gnomesu")))
			cmd = wxT("gnomesu -c \"chown ");
		else if( wxFile::Exists( wxT("/usr/bin/gksu")))
			cmd = wxT("gksu -u root \"chown ");
		else if( wxFile::Exists( wxT("/usr/bin/gksudo")))
			cmd = wxT("gksudo -u root \"chown ");
		cmd += oldOwner + spacer + the_file.GetFullPath() +wxT("\"");
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

	if( Access( the_file.GetFullPath() , (fam == ReadOnly ? wxFile::read : wxFile::read_write) )
#ifdef __WXMSW__
	or (IsWinDevice( the_file ) )
#endif // __WXMSW__
		){
		Close();
		//Open( the_file.GetFullPath(), (fam == ReadOnly ? wxFile::read : wxFile::read_write) );
		OSDependedOpen( the_file, fam );
		std::cout << "file reopened!" << std::endl;
		if(! IsOpened()){
			wxBell();
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
	DiffNode* newnode = new DiffNode( start_byte, size, inject );
	if( size < 0 ){//Deletion!
		newnode->old_data = new unsigned char[-size];
		if( newnode->old_data == NULL )
			wxLogError( _("Not Enough RAM") );
		else{
			Seek( start_byte, wxFromStart );
			Read( newnode->old_data, -size );
			return newnode;
			}
		}
	else if( inject ){
		newnode->new_data = new unsigned char[size];
		if( newnode->new_data == NULL )
			wxLogError( _("Not Enough RAM") );
		else{
			memcpy( newnode->new_data, data, size);
			return newnode;
			}
		}
	else{// Normal operation
		newnode->old_data = new unsigned char[size];
		newnode->new_data = new unsigned char[size];
		if( newnode->old_data == NULL || newnode->new_data == NULL){
			wxLogError( _("Not Enough RAM") );
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

//	unsigned StartShift = put_ptr - StartSector*BlockRWSize;
//	uint64_t EndSector = (put_ptr + size)/BlockRWSize;
//	int rd = 0;

	wxFile::Seek(StartSector*BlockRWSize);
	size_t ret = Write(buffer, size);//*= to make update success true or false
	//sync();
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
					long unsigned int rd_size = (EndSector - StartSector + 1 )*BlockRWSize;// +1 for make read least one sector
					char *bfr = new char[rd_size];
					//for( int i=0 i< rd_size; i++) bfr[i]=0;
					long unsigned rd = 0;
					//#ifdef __DEBUG__
					std::cout << "BlockRW sectors:" << StartSector << ":" << EndSector << " Size:" << rd_size << std::endl;
					//#endif // __DEBUG__

					//First read the original sector
					if ( ProcessID >=0 ){
						long word=0;
						#ifdef __WXMSW__
						SIZE_T written=0;
						#endif
						char* addr=0;
						//unsigned long *ptr = (unsigned long *) buffer;
						while (rd < rd_size) {
							addr = reinterpret_cast<char*>(StartSector*BlockRWSize+rd);
						#ifdef __WXMSW__
							ReadProcessMemory(hDevice, addr, &word, sizeof(word), &written);
						#else
							word = ptrace(PTRACE_PEEKTEXT, ProcessID, addr, 0);
						#endif
							memcpy( bfr+rd , &word, 4);
							rd += 4;
							}
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

					//Than apply the changes

					//if memory process
					if ( ProcessID >=0 ){
						unsigned wr=0;
						long word=0;
					#ifdef __WXMSW__
						SIZE_T written=0;
					#endif
						char* addr;
						while (wr < rd_size) {
							addr = reinterpret_cast<char*>(StartSector*BlockRWSize+wr);
							memcpy(&word, bfr + wr, sizeof(word));
							#ifdef __WXMSW__
							if( WriteProcessMemory(hDevice, addr, &word, sizeof(word), &written) == 0 )
							#else
							if( ptrace(PTRACE_POKETEXT, ProcessID, addr, word) == -1 )
							#endif
								wxMessageBox( _("Error on Write operation to Process RAM"), _("FATAL ERROR") );
							wr += 4;
							}
						success&=true;
						}
					else{
						wxFile::Seek(StartSector*BlockRWSize);
						success = Write(bfr, rd_size) && success;
						}
					delete [] bfr;
					}
				else if (FileType==FAL_Buffer){
					memcpy( internal_file_buffer.GetData(),(DiffArray[i]->flag_commit ? DiffArray[i]->old_data : DiffArray[i]->new_data), DiffArray[i]->size  );
					}
				else{
					//if already written and makeing undo, than use old_data
					success = Write((DiffArray[i]->flag_commit ? DiffArray[i]->old_data : DiffArray[i]->new_data), DiffArray[i]->size) && success;
					}
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

wxFileOffset FAL::Length( int PatchIndice ){
	if( ProcessID >=0 )
		return 0x800000000000LL;

	if ( BlockRWSize > 0 )
		return BlockRWSize*BlockRWCount;

	#ifdef __WXGTK__
		if( the_file.GetFullPath() == wxT("/dev/mem") ){
			return 512*MB;
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

	wxFileOffset max_size;
	if (FileType==FAL_Buffer)
		max_size=internal_file_buffer.GetDataLen();
	else
		max_size=wxFile::Length();

	#ifdef __WXGTK__
	///WorkAround for wxFile::Length() zero size bug
	if( max_size == 0 ){ //This could be GIANT file like /proc/kcore = 128 TB -10MB +12KB
		struct stat st;
		stat(the_file.GetFullPath().To8BitData(), &st);
		uint64_t sz = st.st_size;
		#ifdef _DEBUG_FILE_
		printf( "File Size by STD C is : %llu \r\n" ,sz );
		#endif // _DEBUG_FILE_
		max_size=sz;
		}
	#endif // __wxGTK__

	if(PatchIndice == -1)
		PatchIndice = DiffArray.GetCount();
	for( int i=0 ; i < PatchIndice; i++ )
		if( DiffArray[i]->flag_undo && !DiffArray[i]->flag_commit )
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
		if( DiffArray[i]->flag_undo && !DiffArray[i]->flag_commit )	// Allready committed to disk, nothing to do here
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
		if( DiffArray[i]->flag_undo && !DiffArray[i]->flag_commit )
			break;
		else if( DiffArray.Item(i)->flag_inject )
			return true;
	return false;
	}

#ifdef _DEBUG_FILE_
#include <iomanip> //for setw
#endif // _DEBUG_FILE_
///State of the Art Read Function
//Only recursion could handle such a complex operation. Comes my mind while I am trying to sleep.
long FAL::ReadR( unsigned char* buffer, unsigned size, uint64_t from, ArrayOfNode* PatchArray, int PatchIndice ){
#ifdef _DEBUG_FILE_
	std::cout << "ReadR from:" << std::dec << from << "\t size:"  <<  std::setw(3) << size  << "\t PtchIndice" << PatchIndice << std::endl;
#endif // _DEBUG_FILE_
	///Getting Data from bellow layer.
	if( PatchIndice == 0 )	//Deepest layer
		{
		//Block Read/Write mechanism.
		//if( 0 )//for debugging

#ifdef __linux__
		//Linux file read speed up hack for Block devices.
		//This macro disables blockRW code.
		//Block read code just allowed for memory devices under linux.
		//Because you can read arbitrary locations from block devices at linux.
		//Kernel handle the job...
		//This hack increase reading speed from 164MB to 196MB on my SSD with using read 4MB buffer due use of memcmp.
		//(Max disk rw is 230MB/s)
		if( ProcessID >=0 )
#else
		if( BlockRWSize > 0 )
#endif
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
			#ifdef __WXMSW__
				SIZE_T written=0;
			#endif
				char* addr;
				//unsigned long *ptr = (unsigned long *) buffer;
				while (rd < rd_size) {
					addr = reinterpret_cast<char*>(StartSector*BlockRWSize+rd);
				#ifdef __WXMSW__
					ReadProcessMemory(hDevice, addr, &word, sizeof(word), &written);
				#else
					word = ptrace(PTRACE_PEEKTEXT, ProcessID, addr, 0);
				#endif
					memcpy( bfr+rd , &word, 4);
					rd += 4;
					}

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
		else if(FileType==FAL_Buffer){
			memcpy( buffer, reinterpret_cast<char*>(internal_file_buffer.GetData())+from, size );
			return (from+size > internal_file_buffer.GetDataLen() ? internal_file_buffer.GetDataLen()-from : size);
			}
		wxFile::Seek( from ); //Since this is the Deepest layer
		return wxFile::Read( buffer, size ); //Ends recursion. here
		}

	int readsize=0;
	//than process at current layer
	if(PatchIndice != 0){
		DiffNode* patch = PatchArray->Item(PatchIndice-1); //PatchIndice-1 means the last patch item! Don't confuse at upper code.
		if( patch->flag_inject && patch->size < 0 ){//Deletion patch
			readsize = DeletionPatcher( from, buffer, size, PatchArray, PatchIndice );
			}
		else if( patch->flag_inject ){	//Injection patch
			readsize = InjectionPatcher( from, buffer, size, PatchArray, PatchIndice );
			}
		else{
			readsize = ReadR( buffer, size, from, PatchArray, PatchIndice-1 );//PatchIndice-1 => Makes Patch = 0 for 1 patch. Read from file.
			if(size != static_cast<unsigned int>(readsize)) //If there is free chars
				readsize = (Length(PatchIndice-1) - from > size) ? (size):(Length(PatchIndice-1) - from);	//check for buffer overflow
			ModificationPatcher( from, buffer, size, patch );
			}
		}
//	if(readsize < 0)
//		return -1;
	if( static_cast<int64_t>(from + readsize) > Length(PatchIndice) ){
		//Injection fills all buffer as requested. So we need truncate it for avoid random memory on file end.
		readsize = Length(PatchIndice)-from;
		}
#ifdef _DEBUG_FILE_
	std::cout << "Read Size:"  <<  std::setw(3) << readsize << std::endl;
#endif // _DEBUG_FILE_
 	return readsize;
	}

long FAL::InjectionPatcher(uint64_t current_location, unsigned char* data, int size, ArrayOfNode* PatchArray, int PatchIndice){
#ifdef _DEBUG_FILE_
	std::cout << "InjectionP:" << std::dec << current_location << "\t size:"  <<  std::setw(3) << size  << "\t PtchIndice" << PatchIndice << std::endl;
#endif // _DEBUG_FILE_
	/******* MANUAL of Code Understanding *******
	* current_location                    = [   *
	* current_location + size             = ]   *
	* Injections start_offset             = (   *
	* Injections start_offset + it's size = )   *
	* data                                = ... *
	* first partition                     = xxx *
	********************************************/
	if( PatchArray->Item(PatchIndice-1) -> size > 0 && PatchArray->Item(PatchIndice-1)->flag_inject ){
		DiffNode *Inject_Node = PatchArray->Item(PatchIndice-1);	//For easy debugging
		///If injection starts behind, so we needed to read bytes from before to show correct location.
		///State ...().,,,[...]..... -> ...(...).[,,,].
		if( Inject_Node->end_offset() <= current_location ){
			return ReadR( data, size, current_location - Inject_Node->size, PatchArray, PatchIndice-1 );
			}
		///State ...(),,,[....]..... -> ...(..[xx),,],..
		else if( Inject_Node->start_offset <= current_location
					&& Inject_Node->end_offset() > current_location
					&& Inject_Node->end_offset() < current_location + size
					){
				int movement = current_location - Inject_Node->start_offset;   // This bytes discarded from injection
				int first_part = Inject_Node->end_offset() - current_location; // First part size
				memcpy( data, Inject_Node->new_data + movement, first_part );  // Copy first part to buffer
				int read_size = ReadR( data+first_part, size-first_part, Inject_Node->start_offset, PatchArray, PatchIndice-1 ); //Than copy second part.
				return wxMin(read_size+first_part, size);
			}

		///State ...(),,,[...]..... -> ...(...[...].),,,
		else if( Inject_Node->start_offset <= current_location
					&& Inject_Node->end_offset() >= current_location + size
					){
				int movement = current_location - Inject_Node->start_offset;	// This bytes discarded from injection
				memcpy( data, Inject_Node->new_data + movement, size );			// Copy first part to buffer
				return size;
			}
		///State ...[...(),,,]... -> ...[xx(...)...],,,...
		else if( Inject_Node->start_offset > current_location
					&& Inject_Node->start_offset < current_location + size
					&& Inject_Node->end_offset() < current_location + size
					){
				int first_part = Inject_Node->start_offset - current_location; // First part size
				int read_size = ReadR( data, first_part, current_location, PatchArray, PatchIndice-1 ); //Read First Part from lower layer
				memcpy( data+first_part, Inject_Node->new_data, Inject_Node->size );  // Copy second part to buffer
				read_size += ReadR( data+first_part+Inject_Node->size, size-(first_part+Inject_Node->size), current_location+first_part, PatchArray, PatchIndice-1 ); //Read latest chunk
				return wxMin( read_size+Inject_Node->size , size );
			}
		///State ...[...(),,,]... -> ...[xx(...]...),,,...
		else if( Inject_Node->start_offset > current_location
					&& Inject_Node->start_offset < current_location + size
					&& Inject_Node->end_offset() > current_location + size
					){
				int first_part = Inject_Node->start_offset - current_location; // First part size
				int read_size = ReadR( data, first_part, current_location, PatchArray, PatchIndice-1 ); //Read First Part from lower layer
				memcpy( data+first_part, Inject_Node->new_data, size-first_part );  // Copy second part to buffer
				return wxMin(read_size+Inject_Node->size, size);
			}
		///State ...[...]...(...)... -> ...[...]...()...
		else{
			return ReadR( data, size, current_location, PatchArray, PatchIndice-1 );
			}
		}
	return -1;
	}

long FAL::DeletionPatcher(uint64_t current_location, unsigned char* data, int size, ArrayOfNode* PatchArray, int PatchIndice){
#ifdef _DEBUG_FILE_
	std::cout << "Deletion P:" << std::dec << current_location << "\t size:"  <<  std::setw(3) << size  << "\t PtchIndice" << PatchIndice << std::endl;
#endif // _DEBUG_FILE_
	/******* MANUAL of Code Understanding ******
	* current_location                   = [   *
	* current_location + size            = ]   *
	* Deletes start_offset               = (   *
	* Deletes start_offset + it's size   = )   *
	* data                               = ... *
	* first partition                    = xxx *
	*******************************************/
	if( PatchArray->Item(PatchIndice-1) -> size < 0 ){
		DiffNode *Delete_Node = PatchArray->Item(PatchIndice-1);	//For easy debugging
		///If deletion starts behind, so we needed to read bytes from after to show correct location.
		///State ...(...)...[...],,,. -> ...()......[,,,].
		///State ...(..[..).].,,,.... -> ...()..[,,,]....
		///State ...(..[...].)..,,,.. -> ...()..[,,,]..
		if( Delete_Node->start_offset <= current_location ){
		#ifdef _DEBUG_FILE_
			std::cout << "D->";
		#endif // _DEBUG_FILE_
			return ReadR( data, size, current_location - Delete_Node->size, PatchArray, PatchIndice-1 );
			}
		///State ...[xxx(...)...],,,... -> ...[xxx()...,,,]...
		///State ...[xxx(...]...),,,... -> ...[xxx(),,,...]...
		else if( Delete_Node->start_offset > current_location && Delete_Node->start_offset < current_location + size ){
			int first_part_size = Delete_Node->start_offset - current_location;
			int read_size=0;
		#ifdef _DEBUG_FILE_
			std::cout << "D->";
		#endif // _DEBUG_FILE_
			read_size += ReadR( data,						 first_part_size,	current_location,										PatchArray,  PatchIndice-1 );
		#ifdef _DEBUG_FILE_
			std::cout << "D->";
		#endif // _DEBUG_FILE_
			read_size += ReadR( data+first_part_size, size - first_part_size,	current_location + first_part_size - Delete_Node->size,	PatchArray,  PatchIndice-1 );
			return read_size;
			}
		///State ...[...]...(...)... -> ...[...]...()...
		else{
		#ifdef _DEBUG_FILE_
			std::cout << "D->";
		#endif // _DEBUG_FILE_
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
	if( Patch->flag_undo && !Patch->flag_commit )	// Already committed to disk, nothing to do here
		return;
	///State: ...[...(xxx]xxx)...
	if(current_location <= Patch->start_offset && current_location+size >= Patch->start_offset){
		int irq_loc = Patch->start_offset - current_location;
		///...[...(xxx)...]... //not neccessery, this line includes this state too
		int irq_size = (size - irq_loc > Patch->size) ? (Patch->size) : (size - irq_loc);

		//memcpy(data+irq_loc , Patch->flag_undo ? Patch->old_data : Patch->new_data, irq_size );
		Patch->Apply(data+irq_loc, irq_size, 0);
		}

	///State: ...(xxx[xxx)...]...
	else if (current_location <= Patch->start_offset + Patch->size && current_location+size >= Patch->start_offset + Patch->size){
		int irq_skipper = current_location - Patch->start_offset;	//skip this bytes from start
		int irq_size = Patch->size - irq_skipper;

		//memcpy(data, Patch->flag_undo ? Patch->old_data : Patch->new_data + irq_skipper, irq_size );
		Patch->Apply(data, irq_size, irq_skipper);
		}

	///State: ...(xxx[xxx]xxx)...
	else if(Patch->start_offset <= current_location && Patch->start_offset + Patch->size >= current_location+size){
		int irq_skipper = current_location - Patch->start_offset;	//skip this bytes from start

		//memcpy(data, Patch->flag_undo ? Patch->old_data : Patch->new_data + irq_skipper, size );
		Patch->Apply(data, size, irq_skipper);
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

bool FAL::AddDiffNode( DiffNode *new_node ){
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
	//DiffNode *rtn = NewNode( start_byte, data, size, injection );
	//ApplyXOR( rtn->new_data, size, start_byte );
	if( new_node != NULL ){
		DiffArray.Add( new_node );						//Add new node to tail
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

	if( BlockRWSize > 0 || FileType==FAL_Buffer || ProcessID > 0 )
		return ofs;

	return wxFile::Seek( ofs, mode );
	}

//Returns First Undo Node or Last
const DiffNode* FAL::GetFirstUndoNodeOrLast( void ){
	if( DiffArray.GetCount() == 0 )
		return NULL;
	for( unsigned i=0 ; i < DiffArray.GetCount() ; i++ )
		if( DiffArray.Item(i)->flag_undo )
			return DiffArray.Item(i);
	return DiffArray.Last();
	}
