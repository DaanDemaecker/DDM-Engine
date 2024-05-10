# Set the path to the FBX SDK
set(FBX_SDK_DIR "C:/Program Files/Autodesk/FBX/FBX SDK/2020.3.7")

add_library(FbxSdk INTERFACE)

# Include directories for FBX SDK
target_include_directories(FbxSdk INTERFACE ${FBX_SDK_DIR}/include)

# Link directories for FBX SDK
target_link_directories(FbxSdk INTERFACE ${FBX_SDK_DIR}/lib/x64/debug)

target_link_libraries(FbxSdk INTERFACE ${FBX_SDK_DIR}/lib/x64/debug/libfbxsdk.lib)

# Link your executable or library against the FBX SDK libraries
target_link_libraries(D3D-Lite-Engine_Build INTERFACE FbxSdk)