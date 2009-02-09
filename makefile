WXCONFIG = wx-config
CPP = `$(WXCONFIG) --cxx`
CXXFLAGS= `$(WXCONFIG) --cxxflags` -c -Os
LDFLAGS = `$(WXCONFIG) --libs`

SOURCES= HexEditorGui.cpp FileDifference.cpp HexDialogs.cpp HexEditorApp.cpp HexEditor.cpp HexEditorCtrl/HexEditorCtrl.cpp HexEditorCtrl/wxHexCtrl/TagDialogGui.cpp HexEditorCtrl/wxHexCtrl/wxHexCtrl.cpp HexEditorCtrl/wxHexCtrl/Tag.cpp HexEditorCtrl/HexEditorCtrlGui.cpp HexEditorFrame.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=wxHexEditor

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CPP) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CPP) $(CXXFLAGS) $< -o $@
install:
	mkdir -p $(DESTDIR)/usr/bin
	cp wxHexEditor $(DESTDIR)/usr/bin


uninstall:
	rm $(DESTDIR)/usr/bin/wxHexEditor

clean:
	rm *.o HexEditorCtrl/*.o HexEditorCtrl/wxHexCtrl/*.o
	rm wxHexEditor

