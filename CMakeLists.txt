cmake_minimum_required(VERSION 3.14.4)
project(wxHexEditor)

set (CMAKE_C_STANDARD 11)
set (CMAKE_CXX_STANDARD 17)

add_subdirectory(mhash)
add_subdirectory(udis86)

find_package(wxWidgets QUIET REQUIRED aui adv net xml core base)
include(${wxWidgets_USE_FILE})

file(GLOB hex_sources src/*.cpp src/*.h resources/*.rc)

file(
	GLOB hex_editor_ctrl_sources
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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
add_executable(wxHexEditor ${hex_sources} ${hex_editor_ctrl_sources})
target_include_directories(wxHexEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
	wxHexEditor PRIVATE mhash udis ${wxWidgets_LIBRARIES} uxtheme oleacc psapi ws2_32
)
