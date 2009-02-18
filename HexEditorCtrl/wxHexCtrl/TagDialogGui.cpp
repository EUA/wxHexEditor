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
	
	wxFlexGridSizer* fgSizerBottom;
	fgSizerBottom = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerBottom->SetFlexibleDirection( wxBOTH );
	fgSizerBottom->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxFlexGridSizer* fgSizerBottomLeft;
	fgSizerBottomLeft = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizerBottomLeft->AddGrowableCol( 1 );
	fgSizerBottomLeft->SetFlexibleDirection( wxHORIZONTAL );
	fgSizerBottomLeft->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Font Color :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizerBottomLeft->Add( m_staticText1, 1, wxALIGN_CENTER|wxLEFT|wxRIGHT, 10 );
	
	m_FontColourPicker = new wxColourPickerCtrl( this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerBottomLeft->Add( m_FontColourPicker, 0, wxALL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Note Color :"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	fgSizerBottomLeft->Add( m_staticText2, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_NoteColourPicker = new wxColourPickerCtrl( this, wxID_ANY, wxColour( 255, 0, 0 ), wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE );
	fgSizerBottomLeft->Add( m_NoteColourPicker, 0, wxALL, 5 );
	
	fgSizerBottom->Add( fgSizerBottomLeft, 1, wxALIGN_CENTER|wxEXPAND, 5 );
	
	wxBoxSizer* bSizerBottomRight;
	bSizerBottomRight = new wxBoxSizer( wxVERTICAL );
	
	m_SaveButton = new wxButton( this, wxID_SAVE, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SaveButton->SetDefault(); 
	bSizerBottomRight->Add( m_SaveButton, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_RemoveButton = new wxButton( this, wxID_REMOVE, _("Remove"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerBottomRight->Add( m_RemoveButton, 0, wxALIGN_CENTER|wxALL, 5 );
	
	fgSizerBottom->Add( bSizerBottomRight, 1, wxEXPAND, 5 );
	
	fgSizer1->Add( fgSizerBottom, 1, wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );
	
	// Connect Events
	m_SaveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnSave ), NULL, this );
	m_RemoveButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnDelete ), NULL, this );
}

TagDialogGui::~TagDialogGui()
{
	// Disconnect Events
	m_SaveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnSave ), NULL, this );
	m_RemoveButton->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( TagDialogGui::OnDelete ), NULL, this );
}
