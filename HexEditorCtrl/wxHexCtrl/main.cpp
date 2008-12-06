#include "main.h"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

int idMenuQuit = wxNewId();
int idMenuAbout = wxNewId();

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(idMenuQuit, MyFrame::OnQuit)
    EVT_MENU(idMenuAbout, MyFrame::OnAbout)
END_EVENT_TABLE()

MyFrame::MyFrame(wxFrame *frame, const wxString& title)
    : wxFrame(frame, -1, title)
{
#if wxUSE_MENUS
    // create a menu bar
    wxMenuBar* mbar = new wxMenuBar();
    wxMenu* fileMenu = new wxMenu(_T(""));
    fileMenu->Append(idMenuQuit, _("&Quit\tAlt-F4"), _("Quit the application"));
    mbar->Append(fileMenu, _("&File"));
    hex_ctrl = new wxHexCtrl(this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_NO_VSCROLL);

    wxMenu* helpMenu = new wxMenu(_T(""));
    helpMenu->Append(idMenuAbout, _("&About\tF1"), _("Show info about this application"));
    mbar->Append(helpMenu, _("&Help"));

    SetMenuBar(mbar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar with some information about the used wxWidgets version
    CreateStatusBar(2);
    SetStatusText(_("Hello Code::Blocks user !"),0);
    SetStatusText(wxbuildinfo(short_f),1);
#endif // wxUSE_STATUSBAR

	wxString ch;
	for(int j=0 ; j < 90 ; j++ ){
			ch.Append(j);
	hex_ctrl->SetBinValue( ch.char_str(),90,true );
	}
}

MyFrame::~MyFrame()
{
}

void MyFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

