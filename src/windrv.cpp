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

#include "windrv.h"

using namespace std;

bool windowsHDD::FakeDosNameForDevice (
	const WCHAR *lpszDiskFile, WCHAR *lpszDosDevice, WCHAR *lpszCFDevice,
	BOOL bNameOnly
)	{
	if (wcsncmp(lpszDiskFile, L"\\\\", 2) == 0) {
		wcscpy(lpszCFDevice, lpszDiskFile);
		return 1;
		}

	BOOL bDosLinkCreated = TRUE;
	_snwprintf(lpszDosDevice, MAX_PATH, L"wxhxed%lu", GetCurrentProcessId());

	if (bNameOnly == FALSE)
		bDosLinkCreated = DefineDosDeviceW (DDD_RAW_TARGET_PATH, lpszDosDevice, lpszDiskFile);

	if (bDosLinkCreated == FALSE)
		return 1;
	else
		_snwprintf(lpszCFDevice, MAX_PATH, L"\\\\.\\%s", lpszDosDevice);

	return 0;
	}


bool windowsHDD::RemoveFakeDosName (const WCHAR *lpszDiskFile, const WCHAR *lpszDosDevice) {
	BOOL bDosLinkRemoved = DefineDosDeviceW (DDD_RAW_TARGET_PATH | DDD_EXACT_MATCH_ON_REMOVE |
	                       DDD_REMOVE_DEFINITION, lpszDosDevice, lpszDiskFile);
	if (bDosLinkRemoved == FALSE)
		return 1;
	return 0;
	}

void windowsHDD::list_device(const WCHAR *format_str, const WCHAR *szTmp, int n) {
	int nDosLinkCreated;
	HANDLE dev = INVALID_HANDLE_VALUE;
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

	nDosLinkCreated = FakeDosNameForDevice (szTmp, szDosDevice, szCFDevice, FALSE);

	dev = CreateFileW (szCFDevice, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);

	bResult = DeviceIoControl (dev, IOCTL_DISK_GET_PARTITION_INFO, NULL, 0, &diskInfo, sizeof (diskInfo), &dwResult, NULL);

	// Test if device is removable
	if (// n == 0 &&
	   DeviceIoControl (dev, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &driveInfo, sizeof (driveInfo), &dwResult, NULL))
		removable = driveInfo.MediaType == RemovableMedia;

	RemoveFakeDosName(szTmp, szDosDevice);
	CloseHandle(dev);

	if (!bResult)
		return;
	char* ascii = new char[wcslen(szTmp) + 1];
	memset(ascii, 0, wcslen(szTmp) + 1);
	wcstombs( ascii, szTmp, wcslen(szTmp) );
	printf( "Device Found: %s \r\n", ascii );
	devicenames.push_back(ascii);
	}

void windowsHDD::list_devices() {
	const WCHAR *format_str = L"%-30s %9S %-9s %-20S\n";
	WCHAR szTmp[MAX_PATH];
	int i;
	for (i = 0; i < 64; i++) {
		_snwprintf(szTmp, sizeof(szTmp), L"\\\\.\\PhysicalDrive%d", i);
		list_device(format_str, szTmp, 0);
		}

	list_device(format_str, L"\\Device\\Ramdisk", 0);

//	for (i = 0; i < 64; i++) {
//		for (int n = 0; n <= 32; n++) {
//			_snwprintf(szTmp, sizeof(szTmp), L"\\Device\\Harddisk%d\\Partition%d", i, n);
//			list_device(format_str, szTmp, n);
//			}
//		}

//	for (i = 0; i < 8; i++) {
//		_snwprintf(szTmp, sizeof(szTmp), L"\\Device\\Floppy%d", i);
//		list_device(format_str, szTmp, 0);
//		}

//	for (i = 0; i < 26; i++) {
//		_snwprintf(szTmp, sizeof(szTmp), L"\\\\.\\%c:", 'A' + i);
//		list_device(format_str, szTmp, 0);
//		}

	//Add logical Drives here directly.
	uint32_t drives=GetLogicalDrives();
	for(int i=2; i<32 ; i++) {	//i=2 drops A: and B: flopies if available
		if((drives>>i) & 0x01 ) {
			//printf("%c: ", 'A' + i);
			_snwprintf(szTmp, sizeof(szTmp), L"\\\\.\\%c:", 'A' + i);
			char* ascii = new char[wcslen(szTmp) + 1];
			memset(ascii, 0, wcslen(szTmp) + 1);
			wcstombs( ascii, szTmp, wcslen(szTmp) );
			printf( "Device Found: %s \r\n", ascii );
			//list_device(format_str, szTmp, 0);
			if( GetDriveTypeA(ascii+4) != 4 )	//ascii+4 for strip out Z: , !=4 checks if its Network Drive
				devicenames.push_back(ascii);
			}
		}


	///What about https://msdn.microsoft.com/en-us/library/windows/desktop/aa364425(v=vs.85).aspx
	///FindFirstVolume? FindNextVolume? FindCloseVolume?
	//HANDLE WINAPI FindFirstVolume(  _Out_ LPTSTR lpszVolumeName,  _In_  DWORD  cchBufferLength );
//	CHAR volname[256];
//	HANDLE dvar = FindFirstVolume( volname, 256 );
//	printf( "\r\nFindFirstVolume is: %s \r\n", volname );
//	while( FindNextVolume( dvar, volname, 256 ) )
//		printf( "FindNextVolume is: %s \r\n", volname );
//
//	int d = GetLogicalDriveStrings(  256, volname );
//	for(int i=0; i<d ; i++){
//		if(volname[i]==0)
//			printf(" ");
//		else
//			printf("%c", volname[i]);
//		}
//	printf("\r\n");
//	uint32_t drives=GetLogicalDrives();
//	for(int i=0; i<32 ; i++){
//		if((drives>>i) & 0x01 )
//			printf("%c: ", 'A' + i);
//		}
	}

vector<string> windowsHDD::getdevicenamevector() {
	list_devices();
	return devicenames;
	}
