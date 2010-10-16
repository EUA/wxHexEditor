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
*   if not, write to the Free Software Foundation, Inc.,                *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : spamjunkeater at gmail dot com                  *
*************************************************************************/

#include "HexEditorGui.h"

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __WXGTK__
	#include <sys/ioctl.h>
	//#include <dev/disk.h>
	#include <linux/fs.h>
#endif

#ifndef INFOPANEL_H
#define INFOPANEL_H

class InfoPanel : public InfoPanelGui{
	public:
	InfoPanel(wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxTAB_TRAVERSAL )
	:InfoPanelGui( parent, id, pos, size, style){
		};

	void Set( wxFileName flnm, uint64_t lenght, wxString AccessMode, int FD ){
		static wxMutex mutexinfo;
		mutexinfo.Lock();

		struct stat *sbufptr = new struct stat;
      fstat( FD, sbufptr );

		wxString info_string;
		info_string =	_("Name:\t")+flnm.GetFullName()+wxT("\n")+
							_("Path:\t")+flnm.GetPath()+wxT("\n")+
							_("Size:\t")+ wxFileName::GetHumanReadableSize( wxULongLong(lenght) ) +wxT("\n")+
							_("Access:\t")+ AccessMode +wxT("\n")+
							_("Device:\t")+ (S_ISREG( sbufptr->st_mode ) ? _("FILE") :
												 S_ISDIR( sbufptr->st_mode ) ? _("DIRECTORY") :
												 S_ISCHR( sbufptr->st_mode ) ? _("CHARACTER") :
												 S_ISBLK( sbufptr->st_mode ) ? _("BLOCK") :
												 S_ISFIFO( sbufptr->st_mode ) ? _("FIFO") :
												#ifndef __WXMSW__ //Windows has no link and socket files
												 S_ISLNK( sbufptr->st_mode ) ? _("LINK") :
												 S_ISSOCK( sbufptr->st_mode ) ? _("SOCKET") :
												#endif
												 wxT("?")
												 )+wxT("\n");

		if(S_ISBLK( sbufptr->st_mode )){
			#ifdef __WXGTK__//Windows follows different pattern
			int block_size=0;
			int64_t block_count=0;
			//int error = ioctl(FD, DKIOCGETBLOCKCOUNT, &block_count);
			int error = ioctl(FD, BLKSSZGET, &block_size);
				if(error)
					std::cerr << "Can't get block size of " << flnm.GetFullName().ToAscii() << strerror(errno) << errno << std::endl;
				else
					info_string += _("Sector Size: ") + wxString::Format(wxT("%d\n"), block_size);
				error  = ioctl(FD, BLKGETSIZE64, &block_count);
				if (error)
					std::cerr << "Can't get block count of " << flnm.GetFullName().ToAscii() << strerror(errno) << errno << std::endl;
				else
					info_string += _("Sector Count: ") + wxString::Format(wxT("%d"), block_count/block_size);
			#endif
			}

		m_InfoPanelText->SetLabel( info_string );

#ifdef _DEBUG_
		std::cout << flnm.GetPath().ToAscii() << ' ';
		if( S_ISREG( sbufptr->st_mode ))
			printf("regular file");
		else if( S_ISDIR( sbufptr->st_mode ))
			printf("directory");
		else if( S_ISCHR( sbufptr->st_mode ))
			printf("character device");
		else if( S_ISBLK( sbufptr->st_mode )){
			printf("block device");
			}
		else if( S_ISFIFO( sbufptr->st_mode ))
			printf("FIFO");
	#ifndef __WXMSW__
		else if( S_ISLNK( sbufptr->st_mode ))
			printf("symbolic link");
		else if( S_ISSOCK( sbufptr->st_mode ))
			printf("socket");
	#endif
		printf("\n");
#endif
//		S_IFMT 	0170000 	bitmask for the file type bitfields
//		S_IFSOCK 	0140000 	socket
//		S_IFLNK 	0120000 	symbolic link
//		S_IFREG 	0100000 	regular file
//		S_IFBLK 	0060000 	block device
//		S_IFDIR 	0040000 	directory
//		S_IFCHR 	0020000 	character device
//		S_IFIFO 	0010000 	FIFO
//		S_ISUID 	0004000 	set UID bit
//		S_ISGID 	0002000 	set-group-ID bit (see below)
//		S_ISVTX 	0001000 	sticky bit (see below)
//		S_IRWXU 	00700 	mask for file owner permissions
//		S_IRUSR 	00400 	owner has read permission
//		S_IWUSR 	00200 	owner has write permission
//		S_IXUSR 	00100 	owner has execute permission
//		S_IRWXG 	00070 	mask for group permissions
//		S_IRGRP 	00040 	group has read permission
//		S_IWGRP 	00020 	group has write permission
//		S_IXGRP 	00010 	group has execute permission
//		S_IRWXO 	00007 	mask for permissions for others (not in group)
//		S_IROTH 	00004 	others have read permission
//		S_IWOTH 	00002 	others have write permission
//		S_IXOTH 	00001 	others have execute permission
		delete sbufptr;
		mutexinfo.Unlock();
		}

	void OnUpdate( wxCommandEvent& event ){
	}
};

#endif

