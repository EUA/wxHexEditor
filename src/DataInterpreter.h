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

#include "HexEditorGui.h"

#ifdef WX_GCH
#include <wx_pch.h>
#else
#include <wx/wx.h>
#endif

#ifndef DATAINTERPRETER_H
#define DATAINTERPRETER_H
#include <stdint.h>
class DataInterpreter : public InterpreterGui{
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

	public:
	DataInterpreter(wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxTAB_TRAVERSAL )
	:InterpreterGui( parent, id, pos, size, style){
		unidata.raw = unidata.mraw = NULL;
		};
	void Set( const char* buffer, int size ){
// TODO (death#1#): Add exception if size smaller than expected
		static wxMutex mutexset;
		mutexset.Lock();
		if( buffer == NULL )
			return;
		if( unidata.raw != NULL )
			delete unidata.raw;
		if( unidata.mraw != NULL )
			delete unidata.mraw;
		unidata.raw = new char[ size ];
		unidata.mraw = new char[ size ];
		memcpy( unidata.raw, buffer, size );
		memcpy( unidata.mraw, buffer, size );
		unidata.size = size;
		for(int i = 0 ; i < unidata.size ; i++)	// make mirror image of mydata
			unidata.mraw[i]=unidata.raw[unidata.size-i-1];

		unidata.little.bit8   = reinterpret_cast< int8_t*  >(unidata.raw);
		unidata.little.ubit8  = reinterpret_cast< uint8_t* >(unidata.raw);
		unidata.little.bit16  = reinterpret_cast< int16_t* >(unidata.raw);
		unidata.little.ubit16 = reinterpret_cast< uint16_t*>(unidata.raw);
		unidata.little.bit32  = reinterpret_cast< int32_t* >(unidata.raw);
		unidata.little.ubit32 = reinterpret_cast< uint32_t*>(unidata.raw);
		unidata.little.bit64  = reinterpret_cast< int64_t* >(unidata.raw);
		unidata.little.ubit64 = reinterpret_cast< uint64_t*>(unidata.raw);
		unidata.little.bitfloat = reinterpret_cast< float* >(unidata.raw);
		unidata.little.bitdouble = reinterpret_cast< double* >(unidata.raw);

		unidata.big.bit8   = reinterpret_cast< int8_t*  >(unidata.mraw+(size - 1));
		unidata.big.ubit8  = reinterpret_cast< uint8_t* >(unidata.mraw+(size - 1));
		unidata.big.bit16  = reinterpret_cast< int16_t* >(unidata.mraw+(size - 2));
		unidata.big.ubit16 = reinterpret_cast< uint16_t*>(unidata.mraw+(size - 2));
		unidata.big.bit32  = reinterpret_cast< int32_t* >(unidata.mraw+(size - 4));
		unidata.big.ubit32 = reinterpret_cast< uint32_t*>(unidata.mraw+(size - 4));
		unidata.big.bit64  = reinterpret_cast< int64_t* >(unidata.mraw+(size - 8));
		unidata.big.ubit64 = reinterpret_cast< uint64_t*>(unidata.mraw+(size - 8));
		unidata.big.bitfloat = reinterpret_cast< float* >(unidata.mraw+(size - 4));
		unidata.big.bitdouble = reinterpret_cast< double* >(unidata.mraw+(size - 8));

		wxCommandEvent event;
		OnUpdate( event );

		mutexset.Unlock();
		}
	void Clear( void ){
		m_textctrl_binary->Clear();
		m_textctrl_8bit ->Clear();
		m_textctrl_16bit->Clear();
		m_textctrl_32bit->Clear();
		m_textctrl_64bit->Clear();
		m_textctrl_float->Clear();
		m_textctrl_double->Clear();
		}

	void OnUpdate( wxCommandEvent& event ){
		unidata::endian *X = m_check_bigendian->GetValue() ?  &unidata.big : &unidata.little;
		int number = *X->ubit8;
		wxString bn;
		for(int i = 8 ; i > 0 ; i-- ){
			(((number>>(i-1)) & 0x01)==1) ? bn << wxT("1") : bn << wxT("0");
			if( i == 5 )
				bn.append(wxT(" "));
			}
		m_textctrl_binary ->ChangeValue( bn );
		if( m_check_unsigned->GetValue() ){
			m_textctrl_8bit ->ChangeValue( wxString::Format(wxT("%u"),  *X->ubit8 ));
			m_textctrl_16bit->ChangeValue( wxString::Format(wxT("%u"),  *X->ubit16 ));
			m_textctrl_32bit->ChangeValue( wxString::Format(wxT("%u"),  *X->ubit32 ));
			m_textctrl_64bit->ChangeValue( wxString::Format(wxT("%llu"),*X->ubit64 ));
			}
		else {
			m_textctrl_8bit ->ChangeValue( wxString::Format(wxT("%i"),  *X->bit8 ));
			m_textctrl_16bit->ChangeValue( wxString::Format(wxT("%i"),  *X->bit16 ));
			m_textctrl_32bit->ChangeValue( wxString::Format(wxT("%i"),  *X->bit32 ));
			m_textctrl_64bit->ChangeValue( wxString::Format(wxT("%lld"),*X->bit64 ));
			}
		m_textctrl_float ->ChangeValue( wxString::Format(wxT("%.14g"), *X->bitfloat ));
		m_textctrl_double->ChangeValue( wxString::Format(wxT("%.14g"), *X->bitdouble ));
	}
};

#endif

