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
#include "HexEditorCtrl/HexEditorCtrl.h"
#include "HexEditorCtrl/wxHexCtrl/wxHexCtrl.h"

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#ifndef HEXPANELS_H
#define HEXPANELS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef __WXGTK__
	#include <sys/ioctl.h>
	//#include <dev/disk.h>
	#include <linux/fs.h>
#endif

#include <wx/filename.h>

#include <stdint.h>
#include <wx/memory.h>

class DataInterpreter : public InterpreterGui{
	public:
		DataInterpreter(wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxTAB_TRAVERSAL )
		:InterpreterGui( parent, id, pos, size, style){
			unidata.raw = unidata.mraw = NULL;
			};
		void Set( wxMemoryBuffer buffer );
		void Clear( void );
		void OnUpdate( wxCommandEvent& event );
	protected:
		struct unidata{
			char *raw, *mraw;	//big endian and little endian
			struct endian{
				int8_t *bit8;
				int16_t *bit16;
				int32_t *bit32;
				int64_t *bit64;
				uint8_t  *ubit8;
				uint16_t *ubit16;
				uint32_t *ubit32;
				uint64_t *ubit64;
				float  *bitfloat;
				double *bitdouble;
				} little, big;
			short size;
			char *mirrorbfr;
			}unidata;
	};

class InfoPanel : public InfoPanelGui{
	public:
	InfoPanel(wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxTAB_TRAVERSAL )
	:InfoPanelGui( parent, id, pos, size, style){
	}

	void Set( wxFileName flnm, uint64_t lenght, wxString AccessMode, int FD );
	void OnUpdate( wxCommandEvent& event ){
	}
};
class TagPanel : public TagPanelGui{
	public:
	TagPanel(wxWindow* parent_, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxTAB_TRAVERSAL )
	:TagPanelGui( parent_, id, pos, size, style){
		};

	void Set( ArrayOfTAG& MainTagArray );
	void OnTagSelect( wxCommandEvent& event );
	void OnUpdate( wxCommandEvent& event ){
//		parent->
	}
};

#endif


