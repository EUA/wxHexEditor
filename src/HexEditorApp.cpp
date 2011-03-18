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
*               home  : wxhexeditor.sourceforge.net                     *
*               email : spamjunkeater at gmail dot com                  *
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
IMPLEMENT_APP(wxHexEditorApp);

bool wxHexEditorApp::OnInit()
{
	 wxImage::AddHandler(new wxPNGHandler);

    frame = new HexEditorFrame( 0L );
//    frame	->Connect( wxEVT_MOTION,	wxMouseEventHandler(wxHexEditorApp::OnMouseMove),NULL, this);
    frame->Show();
    wxYield();
    // Open all of the files specified on the command line (assumes no flags)
    for(int ii = 1; ii < argc; ++ii){
        wxFileName fn = wxFileName(argv[ii]);

        if(fn.FileExists() and fn.IsFileReadable())
            frame->OpenFile(fn.GetFullPath());
        }
    return true;
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
