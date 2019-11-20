cmake_minimum_required(VERSION 3.15)
project(OctojumpProceduralGenerator)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(x64)
include_directories(x64/Debug)

add_executable(OctojumpProceduralGenerator
        x64/Debug/OctojumpProceduralGenerator.pch
        IHM.cpp
        IHM.h
        IO.cpp
        IO.h
        MapGenerator.cpp
        MapGenerator.h
        OctojumpProceduralGenerator.cpp
        pch.cpp
        pch.h
        Rectangle.h)
