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

#ifndef __HexEditorFrame__
#define __HexEditorFrame__

#include "HexEditorGui.h"
#include "HexEditor.h"
#include "HexDialogs.h"
#include "InfoPanel.h"

#include <wx/filename.h>
#include <wx/dnd.h>
#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/aui/auibook.h>
class DnDFile;

class HexEditorFrame : public HexEditorGui {
	public:
		HexEditorFrame();
		HexEditorFrame(	wxWindow* parent, wxWindowID id = wxID_ANY );
		~HexEditorFrame();

	protected:
		void OnFileOpen( wxCommandEvent& event );
		void OnFileSave( wxCommandEvent& event );
		void OnFileSaveAs( wxCommandEvent& event );
		void OnFileClose( wxCommandEvent& event );
		void OnEditUndo( wxCommandEvent& event );
		void OnEditRedo( wxCommandEvent& event );
		void OnEditCopy( wxCommandEvent& event );
		void OnEditCut( wxCommandEvent& event );
		void OnEditPaste( wxCommandEvent& event );
		void OnEditFind( wxCommandEvent& event );
		void OnEditReplace( wxCommandEvent& event );
		void OnEditGoto( wxCommandEvent& event );
		void OnUpdateUI( wxUpdateUIEvent& event );
		void OnViewMenu( wxCommandEvent& event );
		void OnOptionsFileMode( wxCommandEvent& event );
		void OnQuit( wxCommandEvent& event );
		void OnAbout( wxCommandEvent& event );

		void ActionEnabler( void );
		void ActionDisabler( void );
		void OnNotebookTabSelection( wxAuiNotebookEvent& event );
		void OnNotebookTabClose( wxAuiNotebookEvent& event );

	private:
		void PrepareAUI( void );
		wxAuiNotebook *MyNotebook;
		wxAuiManager *MyAUI;
		DataInterpreter *MyInterpreter;
		InfoPanel *MyInfoPanel;
		wxAuiToolBar* Toolbar;
		friend class DnDFile;
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


#endif
