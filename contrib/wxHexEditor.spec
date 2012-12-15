# Copyright (c) 2006-2011 Erdem U. Altinyurt
# Thanks for oc2pus
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments to spamjunkeater gmail.com

# norootforbuild

%define _prefix	/usr

Name:				wxHexEditor
Summary:		A hex editor for view / edit huge files and devices.
Version:		0.21
Release:		1
License:		GPL
Group:			Editors
URL:				http://wxhexeditor.sourceforge.net/
Source0:		%{name}-v%{version}-src.tar.bz2
BuildRoot:		%{_tmppath}/%{name}-%{version}-build
BuildRequires:	gcc-c++
BuildRequires:	pkgconfig
%if 0%{?suse_version} >= 1140
BuildRequires:  wxWidgets-devel
%else
BuildRequires:  wxGTK-devel >= 2.8.9
%endif
%if 0%{?suse_version}
BuildRequires:  fdupes update-desktop-files
%endif

%description
wxHexEditor is another GUI hex editor for open HUGE files and devices
in linux mainland. Designed for reverse engineering binary files.
With wxHexEditor, you can edit huge files and devices like (hdd/sdd).

This programs supports tagging parts of the file for taking notes and make it easier reverse engineer huge binary files.

wxHexEditor program code supports lots of operating system, because
it's writen by cross-platform API, wxWidgets.

%if %{defined mdkversion}
%else
%debug_package
%endif

%prep
%setup -q -n %{name}

%build
%if %{defined suse_version} && 0%{?suse_version} < 1030
		%__make -j1 \
			WXCONFIG=wx-config-2.8
%else
	%__make -j1 \
		WXCONFIG=wx-config
%endif

%install
%__install -dm 755 %{buildroot}%{_datadir}/pixmaps
%__install -dm 755 %{buildroot}%{_datadir}/applications
%__install -dm 755 %{buildroot}%{_datadir}/locale
%__rm -f %{buildroot}%{_datadir}/applications/%{name}.desktop
%__install -D -s -m 755 %{name} %{buildroot}%{_bindir}/%{name}
%__install -D -m 644 resources/%{name}.png %{buildroot}%{_datadir}/pixmaps/%{name}.png
%__rm -f locale/*/*.po
%__rm -f locale/*.po
%__cp -rf locale/* %{buildroot}%{_datadir}/locale
%if %{defined suse_version}
mv resources/%{name}.desktop ./
%suse_update_desktop_file -i %{name} TextEditor Editor
%else
%__install -D -m 644 resources/%{name}.desktop %{buildroot}%{_datadir}/applications/%{name}.desktop
%endif

%clean
[ -d "%{buildroot}" -a "%{buildroot}" != "" ] && %__rm -rf "%{buildroot}"

%files
%defattr(-,root,root)
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png

%changelog
* Wed Dec 12 2012 Erdem U. Altinyurt <spamjunkeater@gmail.com> - 0.21-
- Updated to version v0.21
* Wed Feb 29 2012 Erdem U. Altinyurt <spamjunkeater@gmail.com> - 0.20-
- Updated to version v0.20
* Sat Mar 19 2011 Erdem U. Altinyurt <spamjunkeater@gmail.com> - 0.10-
- fixed OpenSUSE 11.4 compilation
* Fri Apr 23 2010 Erdem U. Altinyurt <spamjunkeater@gmail.com> - 0.09-0
- closing to final
* Tue Feb 9 2009 Erdem U. Altinyurt <spamjunkeater@gmail.com> - 0.08-0
- initial release of rpm
