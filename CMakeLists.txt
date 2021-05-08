cmake_minimum_required(VERSION 3.14.4)
project(wxHexEditor)

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)

add_subdirectory(mhash)
add_subdirectory(udis86)

find_package(wxWidgets QUIET REQUIRED aui adv net xml core base)
include(${wxWidgets_USE_FILE})

set(hex_editor_ctrl_sources
	src/FAL.cpp
	src/FAL.h
	src/HexDialogs.cpp
	src/HexDialogs.h
	src/HexEditorApp.cpp
	src/HexEditorApp.h
	src/HexEditor.cpp
	src/HexEditorFrame.cpp
	src/HexEditorFrame.h
	src/HexEditorGui.cpp
	src/HexEditorGui.h
	src/HexEditor.h
	src/HexPanels.cpp
	src/HexPanels.h
	src/HexEditorCtrl/HexEditorCtrl.cpp
	src/HexEditorCtrl/HexEditorCtrl.h
	src/HexEditorCtrl/HexEditorCtrlGui.cpp
	src/HexEditorCtrl/HexEditorCtrlGui.h
	src/HexEditorCtrl/wxHexCtrl/Tag.cpp
	src/HexEditorCtrl/wxHexCtrl/Tag.h
	src/HexEditorCtrl/wxHexCtrl/TagDialogGui.cpp
	src/HexEditorCtrl/wxHexCtrl/TagDialogGui.h
	src/HexEditorCtrl/wxHexCtrl/wxHexCtrl.cpp
	src/HexEditorCtrl/wxHexCtrl/wxHexCtrl.h
)

if (WIN32)
	list(APPEND hex_editor_ctrl_sources
		resources/resource.rc
		windrv.cpp
		windrv.h
	)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_executable(wxHexEditor ${hex_editor_ctrl_sources})
target_include_directories(wxHexEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
	wxHexEditor PRIVATE mhash udis ${wxWidgets_LIBRARIES}
)

if (WIN32)
	target_link_libraries(wxHexEditor PRIVATE mhash udis ${wxWidgets_LIBRARIES} uxtheme oleacc psapi ws2_32)
endif()
