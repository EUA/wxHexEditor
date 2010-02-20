WXCONFIG = wx-config
CPP = `$(WXCONFIG) --cxx`
CXXFLAGS= `$(WXCONFIG) --cxxflags` -c ${CFLAGS}
LDFLAGS = `$(WXCONFIG) --libs`
RC = `$(WXCONFIG) --rescomp`
#RC = x86_64-w64-mingw32-windres --define WX_CPU_AMD64
#RCFLAGS = `$(WXCONFIG) --cxxflags`

SOURCES= src/HexEditorGui.cpp \
			src/FileDifference.cpp\
			src/HexDialogs.cpp\
			src/HexEditorApp.cpp\
			src/HexEditor.cpp\
			src/HexEditorCtrl/HexEditorCtrl.cpp\
			src/HexEditorCtrl/wxHexCtrl/TagDialogGui.cpp\
			src/HexEditorCtrl/wxHexCtrl/wxHexCtrl.cpp\
			src/HexEditorCtrl/wxHexCtrl/Tag.cpp\
			src/HexEditorCtrl/HexEditorCtrlGui.cpp\
			src/HexEditorFrame.cpp

OBJECTS=$(SOURCES:.cpp=.o)
RESOURCES= resources/resource.rc
RESOURCE_OBJ=$(RESOURCES:.rc=.o)
EXECUTABLE=wxHexEditor
EXECUTABLE_WIN=wxHexEditor.exe

DESTDIR		=
PREFIX		= $(DESTDIR)/usr
BINDIR	    = $(PREFIX)/bin
DATADIR	    = $(PREFIX)/share
LOCALEDIR   = $(DATADIR)/locale

VERSION = 0.08 Alpha

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CPP) $(CXXFLAGS) $< -o $@

%.o : %.rc
	$(RC) $(RCFLAGS) $< -o $@

win: $(SOURCES) $(RESOURCES) $(EXECUTABLE_WIN)

$(EXECUTABLE_WIN): $(OBJECTS) $(RESOURCE_OBJ)
	$(CPP) $(OBJECTS) $(RESOURCE_OBJ) $(LDFLAGS) -static-libgcc -o $@

maclink: $(OBJECTS)
	$(CPP) $(OBJECTS) $(LDFLAGS) -lexpat -o $(EXECUTABLE)

mac: $(SOURCES) maclink
	mkdir -p wxHexEditor.app/Contents
	mkdir -p wxHexEditor.app/Contents/MacOS
	mkdir -p wxHexEditor.app/Contents/Resources
	install -m 755 wxHexEditor wxHexEditor.app/Contents/MacOS/
	install -m 644 resources/wxHexEditor.icns wxHexEditor.app/Contents/Resources/
	printf "APPLHexE" > wxHexEditor.app/Contents/PkgInfo
	printf "\
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
\t<string>wxHexEditor v$(VERSION)</string>\n\
\
\t<key>CFBundleIconFile</key>\n\
\t<string>wxHexEditor.icns</string>\n\
\
\t<key>CFBundleIdentifier</key>\n\
\t<string>net.sourceforge.wxhexeditor</string>\n\
\
\t<key>CFBundleShortVersionString</key>\n\
\t<string>v$(VERSION)</string>\n\
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
\t<string> (c) 2006-2010, Erdem U. Altinyurt</string>\n\
\
</dict>\n\
</plist>\n\n" > wxHexEditor.app/Contents/Info.plist

install:
	install -D -m 755 wxHexEditor $(BINDIR)/wxHexEditor
	install -D -m 644 resources/wxHexEditor.png $(DATADIR)/pixmaps/wxHexEditor.png
	install -D -m 644 resources/wxHexEditor.desktop $(DATADIR)/applications/wxHexEditor.desktop

uninstall:
	rm -f $(BINDIR)/wxHexEditor
	rm -f $(DATADIR)/pixmaps/wxHexEditor.png
	rm -f $(DATADIR)/applications/wxHexEditor.desktop
clean:
	rm -f src/*.o src/HexEditorCtrl/*.o src/HexEditorCtrl/wxHexCtrl/*.o
	rm -f wxHexEditor
	rm -f wxHexEditor.exe
	rm -rf wxHexEditor.app

