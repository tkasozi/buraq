cmake_minimum_required(VERSION 3.27)

project(PowershellPlugin LANGUAGES CXX)

add_library(PowershellPlugin SHARED
		PowershellPlugin.cpp Powershell.h
)

target_include_directories(PowershellPlugin PUBLIC
		"${CMAKE_SOURCE_DIR}/include"
)
