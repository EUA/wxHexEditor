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

#ifndef DATAINTERPRETER_H
#define DATAINTERPRETER_H

class DataInterpreter : public InterpreterGui
{
	protected:
		struct union_data{
			union little_endian{
				int8_t bit8;
				int16_t bit16;
				int32_t bit32;
				int64_t bit64;
				uint8_t ubit8;
				uint16_t ubit16;
				uint32_t ubit32;
				uint64_t ubit64;
				float bitfloat;
				double bitdouble;
				} *lttdata;
			struct big_endian{
				int8_t *bit8;
				int16_t *bit16;
				int32_t *bit32;
				int64_t *bit64;
				uint8_t  *ubit8;
				uint16_t *ubit16;
				uint32_t *ubit32;
				uint64_t *ubit64;
				float 	*bitfloat;
				double *bitdouble;
				} bigdata;
			short datasize;
			char *bfr;
			char *mirrorbfr;
			}unidata;

	public:
	DataInterpreter(wxWindow* parent, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( -1,-1 ), int style = wxTAB_TRAVERSAL )
	:InterpreterGui( parent, id, pos, size, style){
		unidata.bfr = new char[8];
		unidata.mirrorbfr = new char[8];
		for( int i = 0; i < 8 ; i++ )
			unidata.bfr[i] = unidata.mirrorbfr[i]= 0;
		unidata.datasize = 8;
		Set( unidata.bfr, 8 );
		};

	void Set( const char* buffer, int size ){
		if( buffer == NULL ) return;
		//unidata mydata = reinterpret_cast< unidata* > (ch);
		if( buffer != unidata.bfr ){	//if its not constructor call
			if( unidata.bfr != NULL )
				delete unidata.bfr;
			if( unidata.mirrorbfr != NULL )
				delete unidata.mirrorbfr;
			unidata.bfr = new char[ size ];
			unidata.mirrorbfr = new char[ size ];
			memcpy( unidata.bfr, buffer, size );
			memcpy( unidata.mirrorbfr, buffer, size );
			unidata.datasize = size;
			}
		unidata.lttdata = reinterpret_cast< union_data::little_endian*>(unidata.bfr);

		for(int i = 0 ; i < unidata.datasize ; i++)	// make mirror image of mydata
			unidata.mirrorbfr[i]=unidata.bfr[unidata.datasize-i-1];

		unidata.bigdata.bit8 = reinterpret_cast< int8_t* >(unidata.mirrorbfr+(size - 1));
		unidata.bigdata.ubit8 = reinterpret_cast< uint8_t* >(unidata.mirrorbfr+(size - 1));
		unidata.bigdata.bit16 = reinterpret_cast< int16_t* >(unidata.mirrorbfr+(size - 2));
		unidata.bigdata.ubit16 = reinterpret_cast< uint16_t* >(unidata.mirrorbfr+(size - 2));
		unidata.bigdata.bit32 = reinterpret_cast< int32_t* >(unidata.mirrorbfr+(size - 4));
		unidata.bigdata.ubit32 = reinterpret_cast< uint32_t* >(unidata.mirrorbfr+(size - 4));
		unidata.bigdata.bit64 = reinterpret_cast< int64_t* >(unidata.mirrorbfr+(size - 8));
		unidata.bigdata.ubit64 = reinterpret_cast< uint64_t* >(unidata.mirrorbfr+(size - 8));
		unidata.bigdata.bitfloat = reinterpret_cast< float* >(unidata.mirrorbfr+(size - 4));
		unidata.bigdata.bitdouble = reinterpret_cast< double* >(unidata.mirrorbfr+(size - 8));

		Update();
		}

	void Update( ){
		if( m_check_bigendian->GetValue() ){
			union_data::big_endian *X = &unidata.bigdata;
			if( m_check_unsigned->GetValue() ){
				m_textctrl_8bit->ChangeValue( wxString::Format(wxT("%u"), *X->ubit8 ));
				m_textctrl_16bit->ChangeValue( wxString::Format(wxT("%u"), *X->ubit16 ));
				m_textctrl_32bit->ChangeValue( wxString::Format(wxT("%u"), *X->ubit32 ));
				m_textctrl_64bit->ChangeValue( wxString::Format(wxT("%llu"), *X->ubit64 ));
				}
			else {
				m_textctrl_8bit->ChangeValue( wxString::Format(wxT("%i"), *X->bit8 ));
				m_textctrl_16bit->ChangeValue( wxString::Format(wxT("%i"), *X->bit16 ));
				m_textctrl_32bit->ChangeValue( wxString::Format(wxT("%i"), *X->bit32 ));
				m_textctrl_64bit->ChangeValue( wxString::Format(wxT("%lld"), *X->bit64 ));
				}
			m_textctrl_float->ChangeValue( wxString::Format(wxT("%.14g"), *X->bitfloat ));
			m_textctrl_double->ChangeValue( wxString::Format(wxT("%.14g"), *X->bitdouble ));
			}

		else{
			union_data::little_endian *X = unidata.lttdata;
			if( m_check_unsigned->GetValue() ){
				m_textctrl_8bit->ChangeValue( wxString::Format(wxT("%u"), X->ubit8 ));
				m_textctrl_16bit->ChangeValue( wxString::Format(wxT("%u"), X->ubit16 ));
				m_textctrl_32bit->ChangeValue( wxString::Format(wxT("%u"), X->ubit32 ));
				m_textctrl_64bit->ChangeValue( wxString::Format(wxT("%llu"), X->ubit64 ));
				}
			else {
				m_textctrl_8bit->ChangeValue( wxString::Format(wxT("%i"), X->bit8 ));
				m_textctrl_16bit->ChangeValue( wxString::Format(wxT("%i"), X->bit16 ));
				m_textctrl_32bit->ChangeValue( wxString::Format(wxT("%i"), X->bit32 ));
				m_textctrl_64bit->ChangeValue( wxString::Format(wxT("%lld"), X->bit64 ));
				}
			m_textctrl_float->ChangeValue( wxString::Format(wxT("%.14g"), X->bitfloat ));
			m_textctrl_double->ChangeValue( wxString::Format(wxT("%.14g"), X->bitdouble ));
		}
	}
};

#endif

