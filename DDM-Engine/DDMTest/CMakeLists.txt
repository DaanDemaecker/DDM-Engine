add_executable(DDMTest

"main.cpp"

"CustomComponents/SpectatorMovement.cpp"
"CustomComponents/Rotator.cpp"
"CustomComponents/ModelLoader.cpp"
"CustomComponents/MaterialSwitcher/MaterialSwitcher.cpp"
"CustomComponents/MaterialSwitcher/MaterialSwitchManager.cpp"
"CustomComponents/InfoComponent.cpp"
"CustomComponents/Audio/AudioTester.cpp"
)

# Include directories specific to this target
target_include_directories(DDMTest  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(DDMTest PUBLIC DDMEngine)


add_custom_command(TARGET DDMTest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:DDMTest>
        $<TARGET_FILE_DIR:DDMTest>
    COMMAND_EXPAND_LISTS
)