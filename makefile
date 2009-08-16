WXCONFIG = wx-config
CPP = `$(WXCONFIG) --cxx`
CXXFLAGS= `$(WXCONFIG) --cxxflags` -c ${CFLAGS}

LDFLAGS = `$(WXCONFIG) --libs`

SOURCES= src/HexEditorGui.cpp src/FileDifference.cpp src/HexDialogs.cpp src/HexEditorApp.cpp src/HexEditor.cpp src/HexEditorCtrl/HexEditorCtrl.cpp src/HexEditorCtrl/wxHexCtrl/TagDialogGui.cpp src/HexEditorCtrl/wxHexCtrl/wxHexCtrl.cpp src/HexEditorCtrl/wxHexCtrl/Tag.cpp src/HexEditorCtrl/HexEditorCtrlGui.cpp src/HexEditorFrame.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=wxHexEditor

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CPP) $(CXXFLAGS) $< -o $@

mac-all: $(SOURCES) maclink

maclink: $(CPP) $(OBJECTS) $(LDFLAGS) -lexpat -o $(EXECUTABLE)

mac: mac-all
	mkdir -p wxHexEditor.app/Contents
	mkdir -p wxHexEditor.app/Contents/MacOS
	mkdir -p wxHexEditor.app/Contents/Resources
	mv wxHexEditor wxHexEditor.app/Contents/MacOS/
	cp mondrian.icns wxHexEditor.app/Contents/Resources/
	echo "APPLHexE" > wxHexEditor.app/Contents/PkgInfo
	echo "\
<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n\
<plist version=\"1.0\">\n\
<dict>\n\
 	\t<key>CFBundleDevelopmentRegion</key>\n\
 	\t<string>English</string>\n\
\
  	\t<key>CFBundleExecutable</key>\n\
 	\t<string>wxHexEditor</string>\n\
\
	\t<key>CFBundleGetInfoString</key>\n\
	\t<string>wxHexEditor v0.06-svn Alpha</string>\n\
\
	\t<key>CFBundleIconFile</key>\n\
	\t<string>mondrian.icns</string>\n\
\
  	\t<key>CFBundleIdentifier</key>\n\
 	\t<string>net.sourceforge.wxhexeditor</string>\n\
\
  	\t<key>CFBundleShortVersionString</key>\n\
 	\t<string>v0.06-svn Alpha</string>\n\
\
  	\t<key>CFBundleInfoDictionaryVersion</key>\n\
 	\t<string>6.0</string>\n\
\
  	\t<key>CFBundleName</key>\n\
 	\t<string>wxHexEditor</string>\n\
\
  	\t<key>CFBundlePackageType</key>\n\
 	\t<string>APPL</string>\n\
\
  	\t<key>CFBundleSignature</key>\n\
 	\t<string>HexE</string>\n\
\
  	\t<key>CFBundleVersion</key>\n\
 	\t<string>1.0.0</string>\n\
\
	\t<key>DRURLs</key>\n\
	\t<string>http://wxhexeditor.sourceforge.net</string>\n\
\
	\t<key>NSMainNibFile</key>\n\
	\t<string>wxHexEditor</string>\n\
\
	\t<key>NSPrincipalClass</key>\n\
	\t<string>NSApplication</string>\n\
\
  	\t<key>NSHumanReadableCopyright</key>\n\
 	\t<string> (c) 2006-2009, Erdem U. Altinyurt</string>\n\
\
</dict>\n\
</plist>\n\n" > wxHexEditor.app/Contents/Info.plist

install:
	install -D -m 755 wxHexEditor $(DESTDIR)/usr/bin/wxHexEditor
	install -D -m 644 resources/wxHexEditor.png $(DESTDIR)/usr/share/pixmaps/wxHexEditor.png
	install -D -m 644 resources/wxHexEditor.desktop $(DESTDIR)/usr/share/applications/wxHexEditor.desktop

uninstall:
	rm $(DESTDIR)/usr/bin/wxHexEditor
	rm $(DESTDIR)/usr/share/applications/wxHexEditor.desktop
	rm $(DESTDIR)/usr/share/applications/wxHexEditor.desktop
clean:
	rm src/*.o src/HexEditorCtrl/*.o src/HexEditorCtrl/wxHexCtrl/*.o
	rm wxHexEditor
	rm -rf wxHexEditor.app

