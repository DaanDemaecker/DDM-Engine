add_library(FbxMaster INTERFACE)

target_include_directories(FbxMaster INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(D3D-Lite-Engine_Build PRIVATE FbxMaster)