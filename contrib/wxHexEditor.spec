# Copyright (c) 2006-2009 Erdem U. Altinyurt
# Thanks for oc2pus
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments to spamjunkeater gmail.com

# norootforbuild

%define _prefix	/usr

Name:				wxHexEditor
Summary:		A hex editor for view / edit huge files and devices.
Version:		0.07svn
Release:		1
License:		GPL
Group:			Editors
URL:				http://wxhexeditor.sourceforge.net/
Source0:		%{name}-v%{version}-src.tar.bz2
BuildRoot:		%{_tmppath}/%{name}-%{version}-build
BuildRequires:	dos2unix
BuildRequires:	gcc-c++
BuildRequires:	pkgconfig
%if %{defined suse_version}
BuildRequires:	update-desktop-files
%endif
BuildRequires:	wxGTK-devel >= 2.8

%description
wxHexEditor is another GUI hex editor for open HUGE files and devices
in linux mainland. Designed for reverse engineering binary files.
With wxHexEditor, you can edit huge files and devices like (hdd/sdd).

This programs supports tagging parts of the file for taking notes and make it easier reverse engineer huge binary files.

wxHexEditor program code supports lots of operating system, because
it's writen by cross-platform API, wxWidgets.

%debug_package

%prep
%setup -q -n %{name}_v%{version}

%build
%if %{defined suse_version} && 0%{?suse_version} < 1030
		%__make %{?jobs:-j%{jobs}} \
			WXCONFIG=wx-config-2.8
%else
	%__make %{?jobs:-j%{jobs}} \
		WXCONFIG=wx-config
%endif

%install
%__install -dm 755 %{buildroot}%{_datadir}/pixmaps
%__install -dm 755 %{buildroot}%{_datadir}/applications
#%makeinstall
%__rm -f %{buildroot}%{_datadir}/applications/%{name}.desktop
#%__rm -f resources/%{name}.desktop
%__install -D -s -m 755 %{name} %{buildroot}%{_bindir}/%{name}
%__install -D -m 644 resources/%{name}.png %{buildroot}%{_datadir}/pixmaps/%{name}.png
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
* Tue Feb 9 2009 Erdem U. Altinyurt <spamjunkeater@gmail.com> - 0.07svn-0
- initial release of rpm
