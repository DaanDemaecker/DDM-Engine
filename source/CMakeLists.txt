# Set cmake version
cmake_minimum_required(VERSION 3.11)

# Project name
project(D3D-Lite-Engine)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0079 NEW)

add_subdirectory(D3D-Lite)
add_subdirectory(3rdParty)

