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

/******************This file By Charilaos Skandylas ********************/

#ifndef __WinDrv__
#define __WinDrv__
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;
/*
struct _opt {
	bool			list;
	bool			yes;
	unsigned int	quiet;
	ULONGLONG		start;
	ULONGLONG		end;
	bool			read;
	bool			kilobyte;
	unsigned int	refresh;
	};

typedef struct _opt t_opt;
static t_opt opt = {
	false,			// list
	0,				// quiet
	0,				// start
	0,				// end
	false,			// read
	false,			// kilobyte
	1,				// refresh
	};
*/
class windowsHDD
	{	private:
		vector<string> devicenames;
		void list_device(const WCHAR *format_str, const WCHAR *szTmp, int n);
		void list_devices();
	public:
		bool FakeDosNameForDevice (const WCHAR *lpszDiskFile, WCHAR *lpszDosDevice, WCHAR *lpszCFDevice, BOOL bNameOnly);
		bool RemoveFakeDosName (const WCHAR *lpszDiskFile, const WCHAR *lpszDosDevice);
		vector<string> getdevicenamevector();
	};
#endif // __WinDrv__
