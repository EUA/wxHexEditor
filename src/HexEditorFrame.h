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


#ifndef __HexEditorFrame__
#define __HexEditorFrame__

#include "HexEditorGui.h"
#include "HexEditor.h"
#include "HexEditorApp.h"
#include "HexDialogs.h"
#include "HexPanels.h"
#include "../resources/wxhex.xpm"
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/dnd.h>
#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/aui/auibook.h>
#include <wx/config.h>
#include <wx/url.h>
#include <wx/textdlg.h>
#include <wx/mstream.h>

#if defined __WXMAC__
#include "../resources/osx/png2c.h"
#elif defined __WXMSW__
#include "../resources/win/png2c.h"
#endif

#define wxGetBitmapFromMemory(name) _wxGetBitmapFromMemory(name ## _png, sizeof(name ## _png))
inline wxBitmap _wxGetBitmapFromMemory(const unsigned char *data, int length) {
   wxMemoryInputStream is(data, length);
   return wxBitmap(wxImage(is, wxBITMAP_TYPE_ANY, -1), -1);
 }

class DnDFile;
class HexEditorFrame : public HexEditorGui {
	public:
		HexEditorFrame();
		HexEditorFrame(	wxWindow* parent, wxWindowID id = wxID_ANY );
		~HexEditorFrame();
		void TagHideAll();
		void OpenFile(wxFileName flname);
		HexEditor* GetActiveHexEditor(void);

	protected:
		void OnMenuEvent( wxCommandEvent& event );
		void OnDeviceMenu( wxCommandEvent& event );
		void OnUpdateUI( wxUpdateUIEvent& event );
		void OnViewMenu( wxCommandEvent& event );
		void OnOptionsFileMode( wxCommandEvent& event );
		void OnClose( wxCloseEvent& event );
		void OnQuit( wxCommandEvent& event );
		void OnAbout( wxCommandEvent& event );
		void OnActivate( wxActivateEvent& event );

		void ActionEnabler( void );
		void ActionDisabler( void );
		void OnNotebookTabSelection( wxAuiNotebookEvent& event );
		void OnNotebookTabClose( wxAuiNotebookEvent& event );

	private:
		void PrepareAUI( void );
		//wxAuiNotebook *MyNotebook;
		wxAuiManager *MyAUI;
		DataInterpreter *MyInterpreter;
		InfoPanel *MyInfoPanel;
		TagPanel *MyTagPanel;
		TagPanel *MySearchPanel;
#ifdef _WX_AUIBAR_H_
		wxAuiToolBar* Toolbar;
#else
		wxToolBar* Toolbar;
#endif
		friend class DnDFile;
	};

class HexEditorArtProvider : public wxArtProvider{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size);
	};


class DnDFile : public wxFileDropTarget{
	public:
		DnDFile( HexEditorFrame* myHexFramework) {
			HexFramework = myHexFramework;
			}
	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
	private:
		wxAuiNotebook *m_pOwner;
		HexEditorFrame *HexFramework;
	};

class VersionChecker : public UpdateDialog_Gui {
	public:
		VersionChecker( wxString _url, wxString current_version, wxWindow *parent = NULL, wxWindowID id = 1  );
		void OnChkDisplay( wxCommandEvent& event );
	};

#endif
