/***********************************(GPL)********************************
*	wxHexEditor is a hex edit tool for editing massive files in Linux   *
*	Copyright (C) 2006  Erdem U. Altinyurt                              *
*                                                                       *
*	This program is free software; you can redistribute it and/or       *
*	modify it under the terms of the GNU General Public License         *
*	as published by the Free Software Foundation; either version 2      *
*	of the License, or any later version.                               *
*                                                                       *
*	This program is distributed in the hope that it will be useful,     *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*	GNU General Public License for more details.                        *
*                                                                       *
*	You should have received a copy of the GNU General Public License   *
*	along with this program;                                            *
*   if not, write to the Free Software	Foundation, Inc.,               *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA        *
*                                                                       *
*               home  : wxhexeditor.sourceforge.net                     *
*               email : death_knight at gamebox.net                     *
*************************************************************************/

#include "HexEditorGui.h"

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#ifndef INFOPANEL_H
#define INFOPANEL_H

class InfoPanel : public InfoPanelGui{
	public:
	InfoPanel(wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxTAB_TRAVERSAL )
	:InfoPanelGui( parent, id, pos, size, style){
		};

	void Set( wxFileName flnm, wxString AccessMode ){
		static wxMutex mutexinfo;
		mutexinfo.Lock();

		m_name->SetLabel( flnm.GetFullName() );
		m_path->SetLabel( flnm.GetPath() );
// TODO (death#1#): Problem on block device sizes
		m_size->SetLabel( flnm.GetSize().ToString() );

		m_access->SetLabel( AccessMode );

		wxArrayString a;
		wxExecute( (wxT("ls -l \"") + flnm.GetFullPath() + wxT("\"") ), a );
//		system( (wxT("ls -l \"") + flnm.GetFullPath() + wxT("\"") ).ToAscii()  );
		std::cout << a.Item(0).ToAscii() << std::endl;
		switch(a.Item(0).GetChar(0)){
			case '-':
				m_device->SetLabel(wxT("FILE"));
				break;
			case 'b':
				m_device->SetLabel(wxT("BLOCK"));
				break;
			case 'c':
				m_device->SetLabel(wxT("CHARACTER"));
				break;
			default:
				m_device->SetLabel(wxT("UNKNOWN"));
			}
		mutexinfo.Unlock();
		}

	void OnUpdate( wxCommandEvent& event ){
	}
};

#endif

