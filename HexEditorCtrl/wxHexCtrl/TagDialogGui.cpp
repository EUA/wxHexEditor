///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 17 2008)
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
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	TagTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	fgSizer1->Add( TagTextCtrl, 0, wxEXPAND|wxTOP, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Font Color :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer2->Add( m_staticText1, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 10 );
	
	FontBitmapButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,25 ), wxBU_AUTODRAW );
	fgSizer2->Add( FontBitmapButton, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Note Color :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizer2->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	NoteBitmapButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxSize( -1,25 ), wxBU_AUTODRAW );
	fgSizer2->Add( NoteBitmapButton, 0, wxALL|wxEXPAND, 5 );
	
	bSizer5->Add( fgSizer2, 1, wxALIGN_CENTER|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button1 = new wxButton( this, wxID_SAVE, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button1->SetDefault(); 
	bSizer2->Add( m_button1, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_button2 = new wxButton( this, wxID_REMOVE, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_button2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	bSizer5->Add( bSizer2, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( bSizer5, 0, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	
	// Connect Events
	FontBitmapButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnFontColor ), NULL, this );
	NoteBitmapButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnNoteColor ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnSave ), NULL, this );
	m_button2->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnDelete ), NULL, this );
}

TagDialogGui::~TagDialogGui()
{
	// Disconnect Events
	FontBitmapButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnFontColor ), NULL, this );
	NoteBitmapButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnNoteColor ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnSave ), NULL, this );
	m_button2->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnDelete ), NULL, this );
}
