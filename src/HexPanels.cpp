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
*               home  : www.wxhexeditor.org                             *
*               email : spamjunkeater@gmail.com                         *
*************************************************************************/

#include "HexEditorFrame.h"

void DataInterpreter::Set( wxMemoryBuffer buffer ){
// TODO (death#1#): Add exception if size smaller than expected
		static wxMutex mutexset;
#ifdef _DEBUG_MUTEX_
		std::cout << "DataInterpeter Set() Mutex Locked" << std::endl;
#endif
		mutexset.Lock();

		int size = buffer.GetDataLen();
		if( size == 0 ){
			wxBell();
			Clear();
			mutexset.Unlock();
			return;
			}
		if( unidata.raw != NULL )
			delete [] unidata.raw;
		if( unidata.mraw != NULL )
			delete [] unidata.mraw;
		unidata.raw = new char[ size ];
		unidata.mraw = new char[ size ];
		memcpy( unidata.raw, buffer.GetData(), size );
		memcpy( unidata.mraw, buffer.GetData(), size );
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
#ifdef _DEBUG_MUTEX_
		std::cout << "DataInterpeter Set() Mutex UnLocked" << std::endl;
#endif
		}

void DataInterpreter::OnCheckEdit( wxCommandEvent& event ){
	if( event.IsChecked() ){
		m_textctrl_binary->SetFocus();
		m_textctrl_binary->SetInsertionPoint(0);
// TODO (death#1#): Needed to activate INSERT mode when pressed to Edit check
//		wxKeyEvent emulate_insert(WXK_INSERT);
//		OnTextEdit( emulate_insert );
//
//		wxTextAttr at;
//		m_textctrl_binary->GetStyle( 0, at );
//		at.SetTextColour( *wxGREEN );
//		//m_textctrl_binary->SetStyle( 0,8, at );
//		m_textctrl_binary->SetDefaultStyle( at );
//		m_textctrl_binary->SetValue(m_textctrl_binary->GetValue());
		}
	else{
//		wxTextAttr at;
//		m_textctrl_binary->GetStyle( 0, at );
//		at.SetBackgroundColour( *wxBLACK );
//		//m_textctrl_binary->SetStyle( 0,8, at );
//		m_textctrl_binary->SetDefaultStyle( at );
//		m_textctrl_binary->SetValue(m_textctrl_binary->GetValue());
		}

	}
void DataInterpreter::OnTextMouse( wxMouseEvent& event ){
	if( event.ButtonDown() ) //Just allowed left mouse, setted up by wxFormBuilder.
		wxBell();
	else
		event.Skip();
	}
void DataInterpreter::OnTextEdit( wxKeyEvent& event ){
	if( ( event.GetKeyCode() == '0'
			or event.GetKeyCode() == '1'
			or event.GetKeyCode() == WXK_INSERT
			//or event.GetKeyCode() == WXK_DELETE
			or event.GetKeyCode() == WXK_END
			or event.GetKeyCode() == WXK_HOME
			or event.GetKeyCode() == WXK_LEFT
			or event.GetKeyCode() == WXK_RIGHT
			//or event.GetKeyCode() == WXK_BACK
			)
		and m_check_edit->IsChecked() ){

		event.Skip(); //make updates on binary text control

		//if binary data filled properly, update other text controls
		if(m_textctrl_binary->GetLineLength(0) == 8 and (event.GetKeyCode()=='1' or event.GetKeyCode()=='0')){
			int cursorat = m_textctrl_binary->GetInsertionPoint();
			if(event.GetKeyCode()=='1')
				unidata.raw[0] |= (1 << (7-cursorat));
			else
				unidata.raw[0] &= ~(1 << (7-cursorat));

			//unsigned long newlongbyte=0;
			//char newbyte = static_cast<char>(newlongbyte & 0xFF);
			wxMemoryBuffer buffer;
			//buffer.AppendByte( newbyte );
			buffer.AppendData( unidata.raw, unidata.size );
			//if(unidata.size > 1)
			//	buffer.AppendData( unidata.raw+1, unidata.size-1 );
			Set( buffer );
			m_textctrl_binary->SetInsertionPoint( cursorat );
			}
		}
	else if( event.GetKeyCode() == WXK_RETURN and m_textctrl_binary->GetLineLength(0) == 8 ){
		//Validation
			unsigned long newlongbyte=0;
			m_textctrl_binary->GetValue().ToULong( &newlongbyte, 2);
			char newbyte = static_cast<char>(newlongbyte & 0xFF);

			HexEditor *hx = static_cast< HexEditorFrame* >(GetParent())->GetActiveHexEditor();

			hx->FileAddDiff( hx->CursorOffset(), &newbyte, 1);						// add write node to file
			hx->Reload();	//Updates hex editor to show difference.
			}
	else
		wxBell();
	}

void DataInterpreter::Clear( void ){
		m_textctrl_binary->Clear();
		m_textctrl_8bit ->Clear();
		m_textctrl_16bit->Clear();
		m_textctrl_32bit->Clear();
		m_textctrl_64bit->Clear();
		m_textctrl_float->Clear();
		m_textctrl_double->Clear();
		}

void DataInterpreter::OnUpdate( wxCommandEvent& event ){
		unidata::endian *X = m_check_bigendian->GetValue() ?  &unidata.big : &unidata.little;
		int number = *X->ubit8;
		wxString bn;
		for(int i = 8 ; i > 0 ; i-- ){
			(((number>>(i-1)) & 0x01)==1) ? bn << wxT("1") : bn << wxT("0");
//		Disabled shaping due edit function.
//			if( i == 5 )
//				bn.append(wxT(" "));
			}
		m_textctrl_binary ->ChangeValue( bn );
		if( m_check_unsigned->GetValue() ){
			m_textctrl_8bit ->ChangeValue( wxString::Format(wxT("%u"),  *X->ubit8 ));
			m_textctrl_16bit->ChangeValue( wxString::Format(wxT("%u"),  *X->ubit16 ));
			m_textctrl_32bit->ChangeValue( wxString::Format(wxT("%u"),  *X->ubit32 ));
			m_textctrl_64bit->ChangeValue( wxString::Format(wxT("%"wxLongLongFmtSpec"u"),*X->ubit64 ));
			}
		else {
			m_textctrl_8bit ->ChangeValue( wxString::Format(wxT("%i"),  *X->bit8 ));
			m_textctrl_16bit->ChangeValue( wxString::Format(wxT("%i"),  *X->bit16 ));
			m_textctrl_32bit->ChangeValue( wxString::Format(wxT("%i"),  *X->bit32 ));
			m_textctrl_64bit->ChangeValue( wxString::Format(wxT("%"wxLongLongFmtSpec"d"),*X->bit64 ));
			}
		m_textctrl_float ->ChangeValue( wxString::Format(wxT("%.14g"), *X->bitfloat ));
		m_textctrl_double->ChangeValue( wxString::Format(wxT("%.14g"), *X->bitdouble ));
	}

void InfoPanel::Set( wxFileName flnm, uint64_t lenght, wxString AccessMode, int FD, wxString XORKey ){
		static wxMutex mutexinfo;
		mutexinfo.Lock();

		struct stat *sbufptr = new struct stat;
      fstat( FD, sbufptr );

		wxString info_string;
		info_string =  info_string + _("Name:") + wxT("\t")+flnm.GetFullName()+wxT("\n")+
							_("Path:")+wxT("\t")+flnm.GetPath()+wxT("\n")+
							_("Size:")+wxT("\t")+ wxFileName::GetHumanReadableSize( wxULongLong(lenght) ) +wxT("\n")+
							_("Access:")+wxT("\t")+AccessMode +wxT("\n")+
							_("Device:")+wxT("\t")+(S_ISREG( sbufptr->st_mode ) ? _("FILE") :
															S_ISDIR( sbufptr->st_mode ) ? _("DIRECTORY") :
															S_ISCHR( sbufptr->st_mode ) ? _("CHARACTER") :
															S_ISBLK( sbufptr->st_mode ) ? _("BLOCK") :
															S_ISFIFO( sbufptr->st_mode ) ? _("FIFO") :
														#ifndef __WXMSW__ //Windows has no link and socket files
															S_ISLNK( sbufptr->st_mode ) ? _("LINK") :
															S_ISSOCK( sbufptr->st_mode ) ? _("SOCKET") :
														#endif
														#ifdef __WXMSW__ //S_ISBLK doesn't work on windows.
															wxT("BLOCK")
														#else
															wxT("?")
														#endif
															)+wxT("\n");
		if(S_ISBLK( sbufptr->st_mode )
#ifdef __WXMSW__
			or (sbufptr->st_mode==0)	//Enable block size detection code on windows targets,
#endif
			){
			/*
			int block_size=0;
			uint64_t block_count=0;
		#ifdef __WXGTK__
			ioctl(FD, BLKSSZGET, &block_size);
			ioctl(FD, BLKGETSIZE64, &block_count);
		#elif defined (__WXMAC__)
			ioctl(FD, DKIOCGETBLOCKSIZE, &block_size);
			ioctl(FD, DKIOCGETBLOCKCOUNT, &block_count);
		#elif defined (__WXMSW__)
			DWORD dwResult;
			DISK_GEOMETRY driveInfo;
			DeviceIoControl ( (void*)_get_osfhandle(FD), IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0, &driveInfo, sizeof (driveInfo), &dwResult, NULL);
			block_size=driveInfo.BytesPerSector;
			block_count=driveInfo.TracksPerCylinder*driveInfo.SectorsPerTrack*driveInfo.Cylinders.QuadPart;
		#endif
			info_string += _("Sector Size: ") + wxString::Format(wxT("%u\n"), block_size);
			info_string += _("Sector Count: ") + wxString::Format(wxT("%"wxLongLongFmtSpec"u"), block_count/block_size);
			*/
			info_string += _("Sector Size: ") + wxString::Format(wxT("%u\n"), FDtoBlockSize( FD ));
			info_string += _("Sector Count: ") + wxString::Format(wxT("%"wxLongLongFmtSpec"u"), FDtoBlockCount( FD ));

			}

		if(XORKey != wxEmptyString)
			info_string += wxString(_("XORKey:")) + wxT("\t") + XORKey;

		m_InfoPanelText->SetLabel( info_string );

#ifdef _DEBUG_
		std::cout << flnm.GetPath().ToAscii() << ' ';
		if( S_ISREG( sbufptr->st_mode ))
			printf("regular file");
		else if( S_ISDIR( sbufptr->st_mode ))
			printf("directory");
		else if( S_ISCHR( sbufptr->st_mode ))
			printf("character device");
		else if( S_ISBLK( sbufptr->st_mode )){
			printf("block device");
			}
		else if( S_ISFIFO( sbufptr->st_mode ))
			printf("FIFO");
	#ifndef __WXMSW__
		else if( S_ISLNK( sbufptr->st_mode ))
			printf("symbolic link");
		else if( S_ISSOCK( sbufptr->st_mode ))
			printf("socket");
	#endif
		printf("\n");
#endif
//		S_IFMT 	0170000 	bitmask for the file type bitfields
//		S_IFSOCK 	0140000 	socket
//		S_IFLNK 	0120000 	symbolic link
//		S_IFREG 	0100000 	regular file
//		S_IFBLK 	0060000 	block device
//		S_IFDIR 	0040000 	directory
//		S_IFCHR 	0020000 	character device
//		S_IFIFO 	0010000 	FIFO
//		S_ISUID 	0004000 	set UID bit
//		S_ISGID 	0002000 	set-group-ID bit (see below)
//		S_ISVTX 	0001000 	sticky bit (see below)
//		S_IRWXU 	00700 	mask for file owner permissions
//		S_IRUSR 	00400 	owner has read permission
//		S_IWUSR 	00200 	owner has write permission
//		S_IXUSR 	00100 	owner has execute permission
//		S_IRWXG 	00070 	mask for group permissions
//		S_IRGRP 	00040 	group has read permission
//		S_IWGRP 	00020 	group has write permission
//		S_IXGRP 	00010 	group has execute permission
//		S_IRWXO 	00007 	mask for permissions for others (not in group)
//		S_IROTH 	00004 	others have read permission
//		S_IWOTH 	00002 	others have write permission
//		S_IXOTH 	00001 	others have execute permission
		delete sbufptr;
		mutexinfo.Unlock();
		}


void TagPanel::Clear( void ){
	mutextag.Lock();
	TagPanelList->Clear();
	mutextag.Unlock();
	}

void TagPanel::Set( ArrayOfTAG& TagArray ){
	mutextag.Lock();
	wxArrayString str;
	for(unsigned i = 0 ; i < TagArray.Count() ; i++)
		str.Add( TagArray.Item(i)->tag.IsEmpty() ?
// TODO (death#1#): wxLongLongFmtSpec need here!!!					wxString::Format(wxT("%d. Offset %"wxLongLongFmtSpec"u"),i+1, TagArray.Item(i)->start )
					: TagArray.Item(i)->tag );

	TagPanelList->Clear();
	TagPanelList->InsertItems(str,0);
	mutextag.Unlock();
	}

void TagPanel::OnTagSelect(wxCommandEvent& event) {
	//HexEditor* MyHexEditor = static_cast< HexEditorFrame* >(GetParent())->GetActiveHexEditor(); //if detached, parent changes!
	HexEditor* MyHexEditor = parent->GetActiveHexEditor();
	unsigned selection = TagPanelList->GetSelection();
	if( MyHexEditor->MainTagArray.Count() >= selection ) {
		TagElement *tg = MyHexEditor->MainTagArray.Item( selection );
		if(tg == NULL) {
#ifdef _DEBUG_
			std::cerr << "TagPanel::OnTagSelect Selection of tag " << selection << " returns NULL TAG" << std::endl;
#endif
			return;
			}
		MyHexEditor->Goto( tg->start );
		}
	}
void SearchPanel::OnClear( wxCommandEvent& event ){
	HexEditor* MyHexEditor = parent->GetActiveHexEditor();
	TagPanelList->Clear();
	MyHexEditor->HighlightArray.Clear();
	MyHexEditor->Reload();
	}

void SearchPanel::OnTagSelect(wxCommandEvent& event) {
	HexEditor* MyHexEditor = parent->GetActiveHexEditor();
	unsigned selection = TagPanelList->GetSelection();
	if( MyHexEditor->HighlightArray.Count() >= selection ) {
		TagElement *tg = MyHexEditor->HighlightArray.Item( selection );
		if(tg == NULL) {
#ifdef _DEBUG_
			std::cerr << "SearchPanel::OnTagSelect Selection of tag " << selection << " returns NULL TAG" << std::endl;
#endif
			return;
			}
		MyHexEditor->Goto( tg->start , true);
		}
	}

void ComparePanel::OnTagSelect(wxCommandEvent& event) {
	HexEditor* MyHexEditor = parent->GetActiveHexEditor();
	unsigned selection = TagPanelList->GetSelection();
	if( MyHexEditor->CompareArray.Count() >= selection ) {
		TagElement *tg = MyHexEditor->CompareArray.Item( selection );
		if(tg == NULL) {
#ifdef _DEBUG_
			std::cerr << "ComparePanel::OnTagSelect Selection of tag " << selection << " returns NULL TAG" << std::endl;
#endif
			return;
			}
		MyHexEditor->Goto( tg->start , true);
		}
	}

void DisassemblerPanel::Set( wxMemoryBuffer buff ){
	mybuff = buff;
	wxCommandEvent event;
	OnUpdate( event );
	}

void DisassemblerPanel::Clear(void){
	m_dasmCtrl->Clear();
	}

void DisassemblerPanel::OnUpdate( wxCommandEvent& event){
	ud_t ud_obj;
   ud_init(&ud_obj);
   ud_set_input_buffer(&ud_obj, reinterpret_cast<uint8_t*>(mybuff.GetData()), mybuff.GetDataLen() );
   ud_set_vendor(&ud_obj, (m_choiceVendor->GetSelection()) ? UD_VENDOR_AMD : UD_VENDOR_INTEL);
   ud_set_mode(&ud_obj, (m_choiceBitMode->GetSelection()+1)*16);
   ud_set_syntax(&ud_obj, (m_choiceASMType->GetSelection() ? UD_SYN_ATT : UD_SYN_INTEL ));
	wxString mydasm;
   while (ud_disassemble(&ud_obj))
		mydasm << wxString::FromAscii( ud_insn_asm(&ud_obj) ) << wxT("\n");
	m_dasmCtrl->ChangeValue( mydasm );
	}
