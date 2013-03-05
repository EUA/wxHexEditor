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
 * Copyright: Death Knight (wxhexeditor.sourceforge.net)
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

bool wxHexEditorApp::OnInit() {
   wxImage::AddHandler(new wxPNGHandler);
   SetLanguage();
   frame = new HexEditorFrame( 0L );
//    frame	->Connect( wxEVT_MOTION,	wxMouseEventHandler(wxHexEditorApp::OnMouseMove),NULL, this);
   frame->Show();
   wxYield();
   // Open all of the files specified on the command line (assumes no flags)

	//processing --flags
   if(argc == 4)
		for(int ii = 1; ii < argc; ++ii){
			wxString str(argv[ii]);

			//Initializes comparison startup
			if(str.Lower().StartsWith(wxT("--compare"))){
				wxArrayString cfiles;
				for(int j = 1; j < argc; ++j){
					wxString str(argv[j]);
					if(not str.Lower().StartsWith(wxT("--compare")) )
						cfiles.Add(str);
					}

				::CompareDialog *mcd = new CompareDialog( frame, cfiles[0], cfiles[1]);
				mcd->ShowModal();
				#ifndef __WXOSX__ // TODO: This might leak memory but OSX magically give error if I Destroy this.. Really Weird. Please help to fix this.
				mcd->Destroy();
				#endif

				}
			return true;
			}

   for(int ii = 1; ii < argc; ++ii) {
      wxString str(argv[ii]);
      wxFileName fn = wxFileName(str);

 //  if(fn.FileExists() or str.Lower().StartsWith(wxT("-pid")))
      frame->OpenFile(fn.GetFullPath());
      }

   return true;
   }

bool wxHexEditorApp::SetLanguage(void){
	if ( ! wxConfigBase::Get()->Read(_T("Language")).IsEmpty() ){
		int langid = wxConfigBase::Get()->Read( _T("Language"), -1 );
		if ( !myLocale.Init( langid, wxLOCALE_CONV_ENCODING) ){
				wxLogError(_T("This language is not supported by the system."));
				return false;
			}
			wxFileName flnm(argv[0]);
			myLocale.AddCatalogLookupPathPrefix( flnm.GetPath(wxPATH_GET_SEPARATOR) + _T("locale") );
//#ifdef _UNIX_
//			myLocale.AddCatalogLookupPathPrefix( _T("/usr/local/share/locale/") );
//#endif
#ifdef __WXMAC__
			myLocale.AddCatalogLookupPathPrefix( flnm.GetPath(wxPATH_GET_SEPARATOR) +
			_T("..") + wxFileName::GetPathSeparator() + _T("Resources") + wxFileName::GetPathSeparator() + _T("locale") );
#endif
			myLocale.AddCatalog(_T("wxHexEditor"));
		return true;
		}
	return false;
	}

#ifdef _DEBUG__EVENTS_
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
