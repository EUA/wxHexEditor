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

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;
struct _opt {
	bool				list;
	bool				yes;
	unsigned int	quiet;
	ULONGLONG		start;
	ULONGLONG		end;
	bool				read;
	bool				kilobyte;
	unsigned int	refresh;
	};
typedef struct _opt t_opt;
static t_opt opt = {
	false,			/* list */
	0,					/* quiet */
	0,					/* start */
	0,					/* end */
	false,			/* read */
	false,			/* kilobyte */
	1,					/* refresh */
	};
class windowsHDD
	{	private:
		vector<WCHAR*> devicenames;

		void list_device(WCHAR *format_str, WCHAR *szTmp, int n);
		void GetSizeString (LONGLONG size, wchar_t *str);
		bool RemoveFakeDosName (WCHAR *lpszDiskFile, WCHAR *lpszDosDevice);
		bool FakeDosNameForDevice (WCHAR *lpszDiskFile, WCHAR *lpszDosDevice, WCHAR *lpszCFDevice, BOOL bNameOnly);
		void list_devices();
	public:

		vector<WCHAR*> getdevicenamevector();
	};

bool windowsHDD::FakeDosNameForDevice (WCHAR *lpszDiskFile, WCHAR *lpszDosDevice, WCHAR *lpszCFDevice, BOOL bNameOnly)
	{
	if (wcsncmp(lpszDiskFile, L"\\\\", 2) == 0) {
		wcscpy(lpszCFDevice, lpszDiskFile);
		return 1;
		}

	BOOL bDosLinkCreated = TRUE;
	_snwprintf(lpszDosDevice, MAX_PATH, L"dskwipe%lu", GetCurrentProcessId());

	if (bNameOnly == FALSE)
		bDosLinkCreated = DefineDosDeviceW (DDD_RAW_TARGET_PATH, lpszDosDevice, lpszDiskFile);

	if (bDosLinkCreated == FALSE) {
		return 1;
		}
	else {
		_snwprintf(lpszCFDevice, MAX_PATH, L"\\\\.\\%s", lpszDosDevice);
		}
	return 0;
	}


bool windowsHDD::RemoveFakeDosName (WCHAR *lpszDiskFile, WCHAR *lpszDosDevice) {
	BOOL bDosLinkRemoved = DefineDosDeviceW (DDD_RAW_TARGET_PATH | DDD_EXACT_MATCH_ON_REMOVE |
	                       DDD_REMOVE_DEFINITION, lpszDosDevice, lpszDiskFile);
	if (bDosLinkRemoved == FALSE) {
		return 1;
		}
	return 0;
	}


void windowsHDD::GetSizeString (LONGLONG size, wchar_t *str) {
	static wchar_t *b, *kb, *mb, *gb, *tb, *pb;

	if (b == NULL) {
		if (opt.kilobyte) {
			kb = L"KiB";
			mb = L"MiB";
			gb = L"GiB";
			tb = L"TiB";
			pb = L"PiB";
			}
		else {
			kb = L"KB";
			mb = L"MB";
			gb = L"GB";
			tb = L"TB";
			pb = L"PB";
			}
		b = L"bytes";
		}

	DWORD kilo = opt.kilobyte ? 1024 : 1000;
	LONGLONG kiloI64 = kilo;
	double kilod = kilo;

	if (size > kiloI64 * kilo * kilo * kilo * kilo * 99)
		swprintf (str, L"%I64d %s", size/ kilo / kilo /kilo/kilo/kilo, pb);
	else if (size > kiloI64*kilo*kilo*kilo*kilo)
		swprintf (str, L"%.1f %s",(double)(size/kilod/kilo/kilo/kilo/kilo), pb);
	else if (size > kiloI64*kilo*kilo*kilo*99)
		swprintf (str, L"%I64d %s",size/kilo/kilo/kilo/kilo, tb);
	else if (size > kiloI64*kilo*kilo*kilo)
		swprintf (str, L"%.1f %s",(double)(size/kilod/kilo/kilo/kilo), tb);
	else if (size > kiloI64*kilo*kilo*99)
		swprintf (str, L"%I64d %s",size/kilo/kilo/kilo, gb);
	else if (size > kiloI64*kilo*kilo)
		swprintf (str, L"%.1f %s",(double)(size/kilod/kilo/kilo), gb);
	else if (size > kiloI64*kilo*99)
		swprintf (str, L"%I64d %s", size/kilo/kilo, mb);
	else if (size > kiloI64*kilo)
		swprintf (str, L"%.1f %s",(double)(size/kilod/kilo), mb);
	else if (size > kiloI64)
		swprintf (str, L"%I64d %s", size/kilo, kb);
	else
		swprintf (str, L"%I64d %s", size, b);
	}

void windowsHDD::list_device(WCHAR *format_str, WCHAR *szTmp, int n) {
	int nDosLinkCreated;
	HANDLE dev;
	DWORD dwResult;
	BOOL bResult;
	PARTITION_INFORMATION diskInfo;
	DISK_GEOMETRY driveInfo;
	WCHAR szDosDevice[MAX_PATH], szCFDevice[MAX_PATH];
	static LONGLONG deviceSize = 0;
	wchar_t size[100] = {0}, partTypeStr[1024] = {0}, *partType = partTypeStr;

	BOOL drivePresent = FALSE;
	BOOL removable = FALSE;

	drivePresent = TRUE;

	nDosLinkCreated = FakeDosNameForDevice (szTmp, szDosDevice,
	                                        szCFDevice, FALSE);

	dev = CreateFileW (szCFDevice, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

	bResult = DeviceIoControl (dev, IOCTL_DISK_GET_PARTITION_INFO, NULL, 0,
	                           &diskInfo, sizeof (diskInfo), &dwResult, NULL);

	// Test if device is removable
	if (/* n == 0 && */ DeviceIoControl (dev, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0,
	                                     &driveInfo, sizeof (driveInfo), &dwResult, NULL))
		removable = driveInfo.MediaType == RemovableMedia;

	RemoveFakeDosName(szTmp, szDosDevice);
	CloseHandle(dev);

	if (!bResult)
		return;
	WCHAR *buffer=new WCHAR[70];
	swprintf(buffer,L"%-30s", szTmp);
	devicenames.push_back(buffer);
	}


void windowsHDD::list_devices() {


	WCHAR *format_str = L"%-30s %9S %-9s %-20S\n";

	WCHAR szTmp[MAX_PATH];
	int i;

	for (i = 0; i < 64; i++) {
		_snwprintf(szTmp, sizeof(szTmp), L"\\\\.\\PhysicalDrive%d", i);
		list_device(format_str, szTmp, 0);
		}

	for (i = 0; i < 64; i++) {
		for (int n = 0; n <= 32; n++) {
			_snwprintf(szTmp, sizeof(szTmp), L"\\Device\\Harddisk%d\\Partition%d", i, n);
			list_device(format_str, szTmp, n);
			}
		}

	for (i = 0; i < 8; i++) {
		_snwprintf(szTmp, sizeof(szTmp), L"\\Device\\Floppy%d", i);
		list_device(format_str, szTmp, 0);
		}

	list_device(format_str, L"\\Device\\Ramdisk", 0);

	for (i = 0; i < 26; i++) {
		_snwprintf(szTmp, sizeof(szTmp), L"\\\\.\\%c:", 'A' + i);
		list_device(format_str, szTmp, 0);
		}
	}
vector<WCHAR*> windowsHDD::getdevicenamevector()
	{	list_devices();
	return devicenames;
	}
