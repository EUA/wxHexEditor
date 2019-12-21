/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
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

/***************************************************************
 * Name:      wxHexEditorApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Death Knight
 * Created:   2008-05-12
 * Copyright: Erdem U. Altinyurt
 * License:...GPL
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "HexEditorApp.h"
IMPLEMENT_APP(wxHexEditorApp)

class MyClient: public wxClient
{
public:
    void Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic);
};

void MyClient::Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic)
{
    //wxConnection *m_connection = (wxConnection *)MakeConnection(sHost, sService, sTopic);
    //m_connection->Disconnect();
    MakeConnection(sHost, sService, sTopic);
}

#include <wx/snglinst.h>
#include <wx/stdpaths.h>

#include <sys/resource.h>
bool wxHexEditorApp::OnInit() {
//    wxLog::AddTraceMask("EventSource");
//    wxLog::AddTraceMask(wxTRACE_FSWATCHER);

	wxImage::AddHandler(new wxPNGHandler);
	SetLanguage();
	bool SingleInstance;
	myConfigBase::Get()->Read( _T("SingleInstance"), &SingleInstance, false );
	if( SingleInstance ){
		wxSingleInstanceChecker* m_checker = new wxSingleInstanceChecker;
		if ( m_checker->IsAnotherRunning() ){
			delete m_checker;
			m_checker = NULL;

			MyClient *m_client = new MyClient;
			for(int ii = 1; ii < argc; ++ii) {
				wxString str(argv[ii]);
				wxFileName fl(str);
				m_client->Connect("localhost", "59147", "OPEN FILE:"+fl.GetLongPath());
				}
			Exit();
			}
		}

	frame = new HexEditorFrame( 0L );
	// frame->Connect( wxEVT_MOTION,	wxMouseEventHandler(wxHexEditorApp::OnMouseMove),NULL, this);
	frame->Show();
	wxYield();

	///Moved file opening via argv to OnEventLoopEnter()->MyFrameInit() because of need to wait FileSystemWatcher initialization.
	return true;
	}

void wxHexEditorApp::MyAppInit(){
   // Open all of the files specified on the command line (assumes no flags)
	//processing --flags
	if(argc == 4) {
		wxString str(argv[1]);
		//Initializes comparison startup
		if(str.Lower().StartsWith(wxT("--compare"))){
			wxArrayString cfiles;
			for(int j = 1; j < argc; ++j){
				wxString str(argv[j]);
				if(!str.Lower().StartsWith(wxT("--compare")) )
					cfiles.Add(str);
				}

			::CompareDialog mcd( frame, cfiles[0], cfiles[1]);
			mcd.ShowModal();
			return;
			}
		}

	for(int ii = 1; ii < argc; ++ii) {
		wxString str(argv[ii]);
		wxFileName fn = wxFileName(str);
	//  if(fn.FileExists() || str.Lower().StartsWith(wxT("-pid")))
		frame->OpenFile(fn.GetFullPath());
		}
	}

    // create the file system watcher here, because it needs an active loop
void wxHexEditorApp::OnEventLoopEnter(wxEventLoopBase* WXUNUSED(loop))// wxOVERRIDE
	{
	static bool first_run=true;
	if(first_run){
		first_run=false;
#if _FSWATCHER_
		if(frame->file_watcher == NULL ){
			frame->file_watcher = new wxFileSystemWatcher();
			//Not need since all file operations are tracked by HexEditor class
			//frame->file_watcher->SetOwner(this);
			}
#endif // _FSWATCHER_
        //After wxFileSystemWatcher init, we can continue to file related initialization
		MyAppInit();
		}
	}

void wxHexEditorApp::SetLanguage(void){
	wxString lang = myConfigBase::Get()->Read( _T("Language"), wxEmptyString );

	if ( lang.IsEmpty() )
		lang= wxLocale::GetLanguageName( wxLocale::GetSystemLanguage() );

	if ( wxLocale::FindLanguageInfo( lang ) == NULL )
		lang=wxT("English"); //Defaulting to english.

	myConfigBase::Get()->Write( _T("Language"), lang );
	myConfigBase::Get()->Flush();

	///Add catalog paths
	wxFileName flnm(argv[0]); //take current path and search "locale" directory
	myLocale.AddCatalogLookupPathPrefix( flnm.GetPath(wxPATH_GET_SEPARATOR) + _T("locale") );
	//myLocale.AddCatalogLookupPathPrefix( _T("./locale") );

#ifdef _UNIX_
			myLocale.AddCatalogLookupPathPrefix( _T("/usr/share/locale/") );
#endif

#ifdef __WXMAC__
	myLocale.AddCatalogLookupPathPrefix( flnm.GetPath(wxPATH_GET_SEPARATOR) +
	_T("..") + wxFileName::GetPathSeparator() + _T("Resources") + wxFileName::GetPathSeparator() + _T("locale") );
#endif

	///init first
	int langid = wxLocale::FindLanguageInfo( lang )->Language;
	if( lang!=wxT("English") )  //Not init for English.
		if ( !myLocale.Init( langid, wxLOCALE_LOAD_DEFAULT ) ){
		wxLogError(_T("This language is not supported by the system."));
		//return; //not return here for continue to load this program catalog
		}
	///And add catalogs
	myLocale.AddCatalog(_T("wxHexEditor"));
	}

#ifdef _DEBUG_EVENTS_
int wxHexEditorApp::FilterEvent(wxEvent &mevent){
    if( mevent.IsKindOf(CLASSINFO(wxFocusEvent)) )
		{
		wxFocusEvent* event = static_cast<wxFocusEvent*>(&mevent);
		std::cout << "Frame : " << frame << std::endl;
		std::cout << "event.GetEventType(): " << event->GetEventType() << std::endl;
		std::cout << "event.GetEventObject(): " << event->GetEventObject() << std::endl;
		}
	if( mevent.IsKindOf(CLASSINFO(wxMouseEvent)) ){
		wxMouseEvent* event = static_cast<wxMouseEvent*>(&mevent);
		{
		std::cout << "Frame : " << frame << std::endl;
		std::cout << "event.GetEventType(): " << event->GetEventType() << std::endl;
		std::cout << "event.GetEventObject(): " << event->GetEventObject() << std::endl;

		if(event->GetEventType() == wxEVT_MOUSE_CAPTURE_LOST)
			std::cout << "event.wxEVT_MOUSE_CAPTURE_LOSTwxEVT_MOUSE_CAPTURE_LOSTwxEVT_MOUSE_CAPTURE_LOST(): " << std::endl;
		}
	}
	mevent.Skip( );
	return -1;
	}


void wxHexEditorApp::OnMouseMove(wxMouseEvent &event){
	if( event.Moving() )
		std::cout << "HexEditorApp::OnMouseMove Coordinate X:Y = " << event.m_x	<< " " << event.m_y
				  << "\tLeft mouse button:" << event.LeftIsDown() << std::endl;
	else
		std::cout << "HexEditorApp::OnMouseMove Strange Event" << std::endl;
	event.Skip();
	return;
	}
#endif
