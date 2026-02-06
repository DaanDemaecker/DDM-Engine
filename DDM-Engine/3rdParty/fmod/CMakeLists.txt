add_library(fmod SHARED IMPORTED)

set_target_properties(fmod PROPERTIES
    IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/debug/fmodL64_vc.lib"
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/dll/debug/fmodL64.dll"
)

target_link_libraries(DDMEngine PUBLIC fmod)

target_include_directories(DDMEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)