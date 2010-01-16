/***********************************(GPL)********************************
*   wxHexEditor is a hex edit tool for editing massive files in Linux   *
*   Copyright (C) 2006  Erdem U. Altinyurt                              *
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
*   if not, write to the Free Software	Foundation, Inc.,                *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : death_knight at gamebox.net                     *
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
    frame = new HexEditorFrame(0L);
//    frame	->Connect( wxEVT_MOTION,	wxMouseEventHandler(wxHexEditorApp::OnMouseMove),NULL, this);
    frame->Show();
    return true;
}
int wxHexEditorApp::FilterEvent(wxEvent &event){
#if defined(_DEBUG_) && _DEBUG_ > 2
	if( event.IsKindOf(CLASSINFO(wxMouseEvent)) )
//		if( static_cast<wxMouseEvent*>(&event)->Moving() )
//			std::cout << "Got Mouse Moving Event! " <<  std::endl;
		if(static_cast<wxMouseEvent*>(&event)->Entering())
			std::cout << "HexEditorApp::Filter Entering() Event" << std::endl;
		else if(static_cast<wxMouseEvent*>(&event)->Leaving()){
			std::cout << "HexEditorApp::Filter Leaving() Event" << std::endl;
			frame->TagHideAll();
			}
		else
			std::cout << "HexEditorApp::OnMouseMove Strange Event" << std::endl;
#endif
	event.Skip( );
	return -1;
	}
//
//void wxHexEditorApp::OnMouseMove(wxMouseEvent &event){
//#if defined(_DEBUG_) && _DEBUG_ > 1
//	if( event.Moving() )
//		std::cout << "HexEditorApp::OnMouseMove Coordinate X:Y = " << event.m_x	<< " " << event.m_y
//				  << "\tLeft mouse button:" << event.LeftIsDown() << std::endl;
//	else
//		std::cout << "HexEditorApp::OnMouseMove Strange Event" << std::endl;
//#endif
//	event.Skip();
//	return;
//	}
