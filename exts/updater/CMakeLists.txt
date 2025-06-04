project(updater)

set(UPDATER_SOURCES Updater.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Release" AND WIN32)
	add_executable(updater WIN32 ${UPDATER_SOURCES})
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
	add_executable(updater ${UPDATER_SOURCES})
endif ()