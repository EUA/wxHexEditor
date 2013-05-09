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
 * Name:      wxHexEditorApp.h
 * Purpose:   Defines Application Class
 * Author:    Death Knight (spamjunkeater@gmail.com)
 * Created:   2008-05-12
 * Copyright: Death Knight (wxhexeditor.sourceforge.net)
 * License:   GPL v2
 **************************************************************/

#ifndef WXHEXEDITORAPP_H
#define WXHEXEDITORAPP_H

#define _VERSION_ "0.22"
#define _VERSION_S_ _VERSION_ " Beta"
#ifdef __WXMSW__
	#define _VERSION_STR_ _VERSION_S_ " for Windows"
#elif defined(__WXOSX__)
	#define _VERSION_STR_ _VERSION_S_ " for Mac"
#elif defined(__WXGTK__)
	#define _VERSION_STR_ _VERSION_S_ " for Linux"
#else
	#define _VERSION_STR_ _VERSION_S_
#endif

#include <wx/app.h>
#include "HexEditorFrame.h"
class wxHexEditorApp : public wxApp {
   public:
      virtual bool OnInit();
      bool SetLanguage(void);
#ifdef _DEBUG_EVENTS_
      int FilterEvent(wxEvent &evt);
      void OnMouseMove(wxMouseEvent &event);
#endif
   private:
      class HexEditorFrame* frame;
      wxLocale myLocale;
   };

DECLARE_APP(wxHexEditorApp)

#endif // WXHEXEDITORAPP_H
