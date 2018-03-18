///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb  5 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "TagDialogGui.h"

///////////////////////////////////////////////////////////////////////////

TagDialogGui::TagDialogGui( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	TagTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	fgSizer1->Add( TagTextCtrl, 0, wxALL|wxEXPAND, 5 );

	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer( 0, 2, 0, 0 );
	fgSizer4->AddGrowableCol( 0 );
	fgSizer4->AddGrowableCol( 1 );
	fgSizer4->SetFlexibleDirection( wxVERTICAL );
	fgSizer4->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Font Color"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer4->Add( m_staticText1, 1, wxALIGN_CENTER, 10 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Note Color"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer4->Add( m_staticText2, 0, wxALIGN_CENTER, 5 );

	m_FontColourPicker = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer4->Add( m_FontColourPicker, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_NoteColourPicker = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 255, 0, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizer4->Add( m_NoteColourPicker, 1, wxEXPAND|wxRIGHT|wxLEFT, 5 );


	fgSizer1->Add( fgSizer4, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizerBottom;
	bSizerBottom = new wxBoxSizer( wxHORIZONTAL );

	m_CancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerBottom->Add( m_CancelButton, 0, wxALL, 5 );

	m_RemoveButton = new wxButton( this, wxID_REMOVE, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerBottom->Add( m_RemoveButton, 0, wxALL, 5 );

	m_SaveButton = new wxButton( this, wxID_SAVE, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SaveButton->SetDefault();
	bSizerBottom->Add( m_SaveButton, 0, wxALL, 5 );


	fgSizer1->Add( bSizerBottom, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL, 5 );


	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );

	// Connect Events
	m_CancelButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnClose ), NULL, this );
	m_RemoveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnDelete ), NULL, this );
	m_SaveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnSave ), NULL, this );
}

TagDialogGui::~TagDialogGui()
{
	// Disconnect Events
	m_CancelButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnClose ), NULL, this );
	m_RemoveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnDelete ), NULL, this );
	m_SaveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnSave ), NULL, this );

}
