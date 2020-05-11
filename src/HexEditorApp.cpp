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

#ifdef __WXMAC__
BEGIN_EVENT_TABLE(wxHexEditorApp, wxApp)
EVT_MENU(wxID_ABOUT, wxHexEditorApp::OnHelp)
EVT_MENU(wxID_PREFERENCES, wxHexEditorApp::OnHelp)
END_EVENT_TABLE()

void wxHexEditorApp::OnHelp(wxCommandEvent& evt)
{
	if( evt.GetId() == wxID_ABOUT )
		frame->OnHelpMenu(evt);
	else
		frame->OnOptionsMenu(evt);
}
#endif

class MyClient: public wxClient {
	public:
		void Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic);
	};

void MyClient::Connect(const wxString& sHost, const wxString& sService, const wxString& sTopic) {
	//wxConnection *m_connection = (wxConnection *)MakeConnection(sHost, sService, sTopic);
	//m_connection->Disconnect();
	MakeConnection(sHost, sService, sTopic);
	}

#include <wx/snglinst.h>
#include <wx/stdpaths.h>
#if !defined(__MINGW32__)
#include <sys/resource.h>
#endif
#include <wx/cmdline.h>

const wxCmdLineEntryDesc cmdLineDesc[] = {
		{ wxCMD_LINE_SWITCH, "h", "help", "show this help message",     	wxCMD_LINE_VAL_NONE,    wxCMD_LINE_OPTION_HELP },
		{ wxCMD_LINE_OPTION, "o", "offset", "open file and go to offset",	wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, "s", "select", "select given offset." "\r\n" \
	                     "      --select=<byte offset>" "\r\n" \
	                     "      --select=<start>:<end>" "\r\n" \
	                     "      --select=<start>+<length>",					wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
		{ wxCMD_LINE_OPTION, "c", "compare", "compare given two files", 	wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE   },
		{wxCMD_LINE_PARAM, NULL, NULL, "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
	wxCMD_LINE_DESC_END
	};

bool wxHexEditorApp::OnInit() {
//    wxLog::AddTraceMask("EventSource");
//    wxLog::AddTraceMask(wxTRACE_FSWATCHER);

	wxCmdLineParser parser(cmdLineDesc, argc, argv);
	parser.SetLogo( "wxHexEditor " _VERSION_S_ );
	if( parser.Parse()==-1 )
		return false;
	if( parser.Parse()==0 )
		if (parser.Found("c")){
			if( parser.GetParamCount()!=1){
				wxPrintf("Error: Only two filenames allowed for --compare option." "\r\n");
				return false;
				}
			if( parser.Found("s") || parser.Found("o") ){
				wxPrintf("Error: --compare option cannot mixed with other options" "\r\n");
				return false;
				}
			}

	wxImage::AddHandler(new wxPNGHandler);
	SetLanguage();
	bool SingleInstance;
	myConfigBase::Get()->Read( _T("SingleInstance"), &SingleInstance, false );
	if( SingleInstance ) {
		wxSingleInstanceChecker* m_checker = new wxSingleInstanceChecker;
		if ( m_checker->IsAnotherRunning() ) {
			delete m_checker;
			m_checker = NULL;

			MyClient *m_client = new MyClient;
			for(unsigned i = 0; i < parser.GetParamCount() ; i++ ) {
				m_client->Connect("localhost", "59147", "OPEN FILE:"+wxFileName(parser.GetParam(i)).GetLongPath());
//				m_client->Connect("localhost", "59147", "COMPARE FILES:"+wxFileName(parser.GetParam(i)).GetLongPath());
//				m_client->Connect("localhost", "59147", "RELOAD TAGS:"+wxFileName(parser.GetParam(i)).GetLongPath());
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

void wxHexEditorApp::PostAppInit() {
	wxCmdLineParser parser(cmdLineDesc, argc, argv);
	if ( parser.Parse()==0 ) {
		wxString compare_with;
		if (parser.Found("c", &compare_with)) {
			if( parser.GetParamCount()!=1) {
				return;
				}
			if( wxFileName(compare_with).FileExists() && wxFileName(parser.GetParam(0)).FileExists()) {
				::CompareDialog mcd( frame, compare_with, parser.GetParam(0) );
				mcd.ShowModal();
				}
			return;
			}
		}

	for(unsigned i = 0; i < parser.GetParamCount() ; i++ ) {
		wxFileName fn = wxFileName(parser.GetParam(i));
		//  if(fn.FileExists() || str.Lower().StartsWith(wxT("-pid")))
		frame->OpenFile(fn.GetFullPath());
		if ( parser.Parse()==0 ) {
			HexEditor* whx = frame->GetActiveHexEditor();
			wxString param;
			if (parser.Found("o", &param)) {
				whx->Goto( UnkFormatToUInt(param), true );
				}
			if (parser.Found("s", &param)) {
				uint64_t select_start=0;
				uint64_t select_end=0;
				if( param.Find(':') != -1){
					select_start= UnkFormatToUInt(param.BeforeFirst(':'));
					select_end	= UnkFormatToUInt(param.AfterFirst(':'));
					}
				else	if( param.Find('+')!=-1 ){
					select_start= UnkFormatToUInt(param.BeforeFirst('+'));
					select_end	= UnkFormatToUInt(param.AfterFirst('+'));
					if(select_end <= 0)
						continue;
					select_end+=select_start-1;
					}
				else{
					select_end=select_start=UnkFormatToUInt( param );
					}
				whx->Select(select_start,select_end);
				whx->UpdateCursorLocation();//To update statusBar.
				}
			}
		}
	}

// create the file system watcher here, because it needs an active loop
void wxHexEditorApp::OnEventLoopEnter(wxEventLoopBase* WXUNUSED(loop)) { // wxOVERRIDE
	static bool first_run=true;
	if(first_run) {
		first_run=false;
#if _FSWATCHER_
		if(frame->file_watcher == NULL ) {
			frame->file_watcher = new wxFileSystemWatcher();
			//Not need since all file operations are tracked by HexEditor class
			//frame->file_watcher->SetOwner(this);
			}
#endif // _FSWATCHER_
		//After wxFileSystemWatcher init, we can continue to file related initialization
		PostAppInit();
		}
	}

void wxHexEditorApp::SetLanguage(void) {
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
		if ( !myLocale.Init( langid, wxLOCALE_LOAD_DEFAULT ) ) {
			wxLogError(_T("This language is not supported by the system."));
			//return; //not return here for continue to load this program catalog
			}
	///And add catalogs
	myLocale.AddCatalog(_T("wxHexEditor"));
	}

#ifdef _DEBUG_EVENTS_
int wxHexEditorApp::FilterEvent(wxEvent &mevent) {
	if( mevent.IsKindOf(CLASSINFO(wxFocusEvent)) ) {
		wxFocusEvent* event = static_cast<wxFocusEvent*>(&mevent);
		std::cout << "Frame : " << frame << std::endl;
		std::cout << "event.GetEventType(): " << event->GetEventType() << std::endl;
		std::cout << "event.GetEventObject(): " << event->GetEventObject() << std::endl;
		}
	if( mevent.IsKindOf(CLASSINFO(wxMouseEvent)) ) {
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


void wxHexEditorApp::OnMouseMove(wxMouseEvent &event) {
	if( event.Moving() )
		std::cout << "HexEditorApp::OnMouseMove Coordinate X:Y = " << event.m_x	<< " " << event.m_y
		          << "\tLeft mouse button:" << event.LeftIsDown() << std::endl;
	else
		std::cout << "HexEditorApp::OnMouseMove Strange Event" << std::endl;
	event.Skip();
	return;
	}
#endif

uint64_t UnkFormatToUInt(wxString input ){
	long long unsigned int ret=0;
	input=input.Lower();
	if( input.Find('x')!=-1 )
		input.StartsWith('x') || input.StartsWith("0x") ? input.After('x').ToULongLong(&ret, 16): input.Before('x').ToULongLong(&ret, 16);
	else if( input.Lower().Find('o')!=-1 )
		input.StartsWith('o') || input.StartsWith("0o") ? input.After('o').ToULongLong(&ret, 8) : input.Before('o').ToULongLong(&ret, 8);
	else if( input.Lower().Find('b')!=-1 )
		input.StartsWith('b') || input.StartsWith("0b") ? input.After('b').ToULongLong(&ret, 2) : input.Before('b').ToULongLong(&ret, 2);
	else
		input.ToULongLong(&ret, 10);
	return ret;
	}

bool kMGT_ToUInt( wxString user_input, uint64_t *size ){
	wxRegEx filesz("^([0-9]+) *((?=[KMGT])([KMGT])(?:i?B)?|B?)$", wxRE_ICASE | wxRE_ADVANCED);
	if( filesz.Matches(user_input) ) {
		wxString num = filesz.GetMatch( user_input, 1 );
	   long long unsigned int xsize=0;
		user_input.ToULongLong( &xsize, 10 );
		*size=xsize;
		wxString multipler = filesz.GetMatch( user_input, 2 ).Upper();
		if(		multipler.StartsWith(wxT("T")) )		*size*=TB;
		else if( multipler.StartsWith(wxT("G")) )		*size*=GB;
		else if( multipler.StartsWith(wxT("M")) )		*size*=MB;
		else if( multipler.StartsWith(wxT("K")) )		*size*=KB;
		return true;
		}
	return false;
	}

#ifndef __WXMSW__
#include <sys/resource.h>
void SetStackLimit(void){
	const rlim_t kStackSize = 32L * 1024L * 1024L;
	struct rlimit rl;
	int result;

	result = getrlimit(RLIMIT_STACK, &rl);
	fprintf(stderr, "current stack limit = %ul\n", static_cast<unsigned int>(rl.rlim_cur) );
	if (result == 0) {
		if (rl.rlim_cur < kStackSize) {
			rl.rlim_cur = kStackSize;
			result = setrlimit(RLIMIT_STACK, &rl);
			if (result != 0) {
				fprintf(stderr, "setrlimit returned result = %d\n", result);
				}
			}
		}
	}
#endif // __WXMSW__
