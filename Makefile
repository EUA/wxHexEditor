WXCONFIG = wx-config
#CC ?= `$(WXCONFIG) --cc`    #this doesn't look working here properly :(
#CXX ?= `$(WXCONFIG) --cxx`
CC = `$(WXCONFIG) --cc`
CXX = `$(WXCONFIG) --cxx`
LDFLAGS += -lgomp
WXCXXFLAGS= `$(WXCONFIG) --cxxflags` -Iudis86 -Imhash/include -MMD -fopenmp
WXLDFLAGS = `$(WXCONFIG) --libs` `$(WXCONFIG) --libs aui` `$(WXCONFIG) --libs core`
RC = `$(WXCONFIG) --rescomp`
#RC = x86_64-w64-mingw32-windres --define WX_CPU_AMD64
RCFLAGS = `$(WXCONFIG) --cxxflags | sed s/' '-m.*//g;`
MSGFMT = msgfmt

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
LANGUAGEDIRS=`ls -l ./locale | grep ^d | sed s/.*\ //g;`
LANGUAGES=$(wildcard locale/*/wxHexEditor.po)
RESOURCES= resources/resource.rc
RESOURCE_OBJ=$(RESOURCES:.rc=.o)
EXECUTABLE=wxHexEditor
EXECUTABLE_WIN=$(EXECUTABLE).exe
EXECUTABLE_DIR_MAC=$(EXECUTABLE).app

PREFIX      = /usr/local
BINDIR      = $(PREFIX)/bin
DATADIR     = $(PREFIX)/share
LOCALEDIR   = $(DATADIR)/locale

VERSION = 0.23 Beta Development

all:$(EXECUTABLE) langs

$(OBJECTS): $(LIBS) $(SOURCES)

MOBJECTS=$(LANGUAGES:.po=.mo)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) ${CXXFLAGS} ${OPTFLAGS} $(WXLDFLAGS) ${LDFLAGS} -o $@

.cpp.o: $(LIBS)
	$(CXX) -c $(WXCXXFLAGS) $(OPTFLAGS) $(CXXFLAGS) $(CPPFLAGS) $< -o $@

%.o : %.rc
	$(RC) $(RCFLAGS) $< -o $@

langs: $(MOBJECTS)

%.mo : %.po
	$(MSGFMT) $< -o $@

udis86/libudis86/.libs/libudis86.a:
	cd udis86;./autogen.sh
	cd udis86;./configure --host=$(HOST) CC=$(CC) CXX=$(CXX) CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" CPPFLAGS="$(CPPFLAGS)"
	cd udis86/libudis86; $(MAKE) $(MFLAGS)

mhash/lib/.libs/libmhash.a:
	cd mhash; ./configure --host=$(HOST) CC=$(CC) CXX=$(CXX) CFLAGS="$(CFLAGS)" CXXFLAGS="$(CXXFLAGS)" CPPFLAGS="$(CPPFLAGS)"
	cd mhash; $(MAKE) $(MFLAGS)

win: $(RESOURCES) $(EXECUTABLE_WIN)

#Stack override required for file comparison function...
$(EXECUTABLE_WIN): $(OBJECTS) $(RESOURCE_OBJ)
	$(CXX) $(OBJECTS) $(RESOURCE_OBJ) $(LIBS) ${CXXFLAGS} ${OPTFLAGS} $(WXLDFLAGS) ${LDFLAGS} -static -lwinpthread -static-libgcc -static-libstdc++ -Wl,--stack,32000000 -o $@

maclink: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LIBS) ${CXXFLAGS} ${OPTFLAGS} $(WXLDFLAGS) ${LDFLAGS} -lexpat -Wl,-stack_size,0x2000000 -o $(EXECUTABLE)

mac: maclink langs
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
	@for i in $(LANGUAGEDIRS); do \
		echo "mkdir -p wxHexEditor.app/Contents/Resources/locale/$$i/"; \
		mkdir -p wxHexEditor.app/Contents/Resources/locale/$$i; done
	@for i in $(LANGUAGEDIRS); do \
		echo "cp locale/$$i/wxHexEditor.mo wxHexEditor.app/Contents/Resources/locale/$$i/"; \
		cp locale/$$i/wxHexEditor.mo wxHexEditor.app/Contents/Resources/locale/$$i/; done

install:
	install -D -m 755 $(EXECUTABLE) $(DESTDIR)/$(BINDIR)/$(EXECUTABLE)
	install -D -m 644 resources/wxHexEditor.png $(DESTDIR)/$(DATADIR)/pixmaps/wxHexEditor.png
	install -D -m 644 resources/wxHexEditor.desktop $(DESTDIR)/$(DATADIR)/applications/wxHexEditor.desktop
	@for i in $(LANGUAGEDIRS); do \
	   echo "install -D -m 644 locale/$$i/wxHexEditor.mo $(DESTDIR)/$(LOCALEDIR)/$$i/LC_MESSAGES/wxHexEditor.mo"; \
	   install -D -m 644 locale/$$i/wxHexEditor.mo $(DESTDIR)/$(LOCALEDIR)/$$i/LC_MESSAGES/wxHexEditor.mo; done

uninstall:
	rm -f $(DESTDIR)/$(BINDIR)/$(EXECUTABLE)
	rm -f $(DESTDIR)/$(DATADIR)/pixmaps/wxHexEditor.png
	rm -f $(DESTDIR)/$(DATADIR)/applications/wxHexEditor.desktop
	rm -f $(DESTDIR)/$(LOCALEDIR)/*/LC_MESSAGES/wxHexEditor.mo

clean:
	rm -f $(OBJECTS)
	rm -f $(RESOURCE_OBJ)
	rm -f $(DEPENDS)
	rm -f $(EXECUTABLE)
	rm -f $(EXECUTABLE_WIN)
	rm -rf $(EXECUTABLE_DIR_MAC)
	rm -f locale/*/wxHexEditor.mo
distclean: clean
	cd udis86;$(MAKE) distclean
	cd mhash;$(MAKE) distclean

# include the auto-generated dependency files
-include $(DEPENDS)
