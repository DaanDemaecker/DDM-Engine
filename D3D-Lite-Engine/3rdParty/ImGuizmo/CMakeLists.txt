add_library(ImGuizmo INTERFACE)

target_include_directories(ImGuizmo INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)


target_link_libraries(${EXECUTABLE_NAME} PRIVATE ImGuizmo)