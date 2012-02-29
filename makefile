WXCONFIG = wx-config
CC = `$(WXCONFIG) --cc`
CPP = `$(WXCONFIG) --cxx`
CXXFLAGS= `$(WXCONFIG) --cxxflags` -Iudis86 -Imhash/include -MMD -O2 -c ${OPTFLAGS}
LDFLAGS = `$(WXCONFIG) --libs`
RC = `$(WXCONFIG) --rescomp`
#RC = x86_64-w64-mingw32-windres --define WX_CPU_AMD64
RCFLAGS = `$(WXCONFIG) --cxxflags | sed s/' '-m.*//g;`
HOST=
SOURCES= src/HexEditorGui.cpp \
			src/FAL.cpp\
			src/HexDialogs.cpp\
			src/HexPanels.cpp\
			src/HexEditorApp.cpp\
			src/HexEditor.cpp\
			src/HexEditorCtrl/HexEditorCtrl.cpp\
			src/HexEditorCtrl/wxHexCtrl/TagDialogGui.cpp\
			src/HexEditorCtrl/wxHexCtrl/wxHexCtrl.cpp\
			src/HexEditorCtrl/wxHexCtrl/Tag.cpp\
			src/HexEditorCtrl/HexEditorCtrlGui.cpp\
			src/HexEditorFrame.cpp
LIBS = udis86/libudis86/.libs/libudis86.a mhash/lib/.libs/libmhash.a
OBJECTS=$(SOURCES:.cpp=.o)
DEPENDS=$(OBJECTS:.o=.d)
RESOURCES= resources/resource.rc
RESOURCE_OBJ=$(RESOURCES:.rc=.o)
EXECUTABLE=wxHexEditor
EXECUTABLE_WIN=$(EXECUTABLE).exe
EXECUTABLE_DIR_MAC=$(EXECUTABLE).app

PREFIX      = /usr/local
BINDIR      = $(PREFIX)/bin
DATADIR     = $(PREFIX)/share
LOCALEDIR   = $(DATADIR)/locale

VERSION = 0.20 Beta

all: $(EXECUTABLE)

$(OBJECTS): $(LIBS) $(SOURCES)

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(OBJECTS) $(LIBS) $(LDFLAGS) -lgomp -o $@

.cpp.o: $(LIBS)
	$(CPP) $(CXXFLAGS) $< -o $@

%.o : %.rc
	$(RC) $(RCFLAGS) $< -o $@

udis86/libudis86/.libs/libudis86.a:
	cd udis86;./autogen.sh
	cd udis86;./configure --host=$(HOST)
	cd udis86/libudis86; $(MAKE) $(MFLAGS)

mhash/lib/.libs/libmhash.a:
	cd mhash; ./configure --host=$(HOST)
	cd mhash; $(MAKE) $(MFLAGS)

win: $(RESOURCES) $(EXECUTABLE_WIN)

#Stack override required for file comparison function...
$(EXECUTABLE_WIN): $(OBJECTS) $(RESOURCE_OBJ)
	$(CPP) $(OBJECTS) $(RESOURCE_OBJ) $(LIBS) $(LDFLAGS) -static-libgcc -static-libstdc++ -Wl,--stack,32000000 -o $@

maclink: $(OBJECTS)
	$(CPP) $(OBJECTS) $(LIBS) $(LDFLAGS) -lexpat -Wl,-stack_size,0x2000000 -o $(EXECUTABLE)

mac: maclink
	mkdir -p $(EXECUTABLE_DIR_MAC)/Contents
	mkdir -p $(EXECUTABLE_DIR_MAC)/Contents/MacOS
	mkdir -p $(EXECUTABLE_DIR_MAC)/Contents/Resources
	install -m 755 wxHexEditor $(EXECUTABLE_DIR_MAC)/Contents/MacOS/
	install -m 644 resources/wxHexEditor.icns $(EXECUTABLE_DIR_MAC)/Contents/Resources/
	printf "APPLHexE" > $(EXECUTABLE_DIR_MAC)/Contents/PkgInfo
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
\t<string> (c) 2006-2011, Erdem U. Altinyurt</string>\n\
\
</dict>\n\
</plist>\n\n" > $(EXECUTABLE_DIR_MAC)/Contents/Info.plist

install:
	install -D -m 755 $(EXECUTABLE) $(BINDIR)/$(EXECUTABLE)
	install -D -m 644 resources/wxHexEditor.png $(DATADIR)/pixmaps/wxHexEditor.png
	install -D -m 644 resources/wxHexEditor.desktop $(DATADIR)/applications/wxHexEditor.desktop

uninstall:
	rm -f $(BINDIR)/$(EXECUTABLE)
	rm -f $(DATADIR)/pixmaps/wxHexEditor.png
	rm -f $(DATADIR)/applications/wxHexEditor.desktop
clean:
	rm -f $(OBJECTS)
	rm -f $(RESOURCE_OBJ)
	rm -f $(DEPENDS)
	rm -f $(EXECUTABLE)
	rm -f $(EXECUTABLE_WIN)
	rm -rf $(EXECUTABLE_DIR_MAC)
distclean: clean
	cd udis86;$(MAKE) distclean
	cd mhash;$(MAKE) distclean

# include the auto-generated dependency files
-include $(DEPENDS)
