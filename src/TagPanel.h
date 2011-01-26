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


#include "HexEditor.h"
#include "HexEditorGui.h"
#include "HexEditorCtrl/wxHexCtrl/wxHexCtrl.h"

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#ifndef TAGPANEL_H
#define TAGPANEL_H

class TagPanel : public TagPanelGui{
	public:
	TagPanel(wxWindow* parent_, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxTAB_TRAVERSAL )
	:TagPanelGui( parent_, id, pos, size, style){
		};
	void Set( ArrayOfTAG& MainTagArray ){
		static wxMutex mutextag;
		mutextag.Lock();
		wxArrayString str;
		for(unsigned i = 0 ; i < MainTagArray.Count() ; i++)
			str.Add(MainTagArray.Item(i)->tag);

		TagPanelList->InsertItems(str,0);
		mutextag.Unlock();
		}

	void OnTagSelect( wxCommandEvent& event );

	void OnUpdate( wxCommandEvent& event ){
//		parent->
	}
};

#endif


