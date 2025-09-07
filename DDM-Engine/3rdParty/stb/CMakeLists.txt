add_library(stb INTERFACE)

target_include_directories(stb INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(${EXECUTABLE_NAME} PRIVATE stb)
