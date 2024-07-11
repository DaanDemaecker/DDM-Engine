add_library(rapidjson INTERFACE)

target_include_directories(rapidjson INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(D3D-Lite-Engine_Build PRIVATE rapidjson)