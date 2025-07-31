cmake_minimum_required(VERSION 3.16)
project(KungFuChess_Observer)

# הגדרת תקן C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# הוספת תיקיית headers
include_directories(headers)

# Source files for main demo
set(DEMO_SOURCES
    src/ObserverDemo.cpp
    src/GameEventManager.cpp
    src/WhiteScoreTracker.cpp
    src/VoiceAnnouncer.cpp
)

# Source files for simple demo
set(SIMPLE_SOURCES
    src/SimpleDemo.cpp
    src/GameEventManager.cpp
    src/WhiteScoreTracker.cpp
    src/VoiceAnnouncer.cpp
)

# יצירת executables
add_executable(observer_demo ${DEMO_SOURCES})
add_executable(simple_demo ${SIMPLE_SOURCES})

# הוספת support לוינדוס
if(WIN32)
    target_compile_definitions(observer_demo PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(simple_demo PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# הדפסת מידע
message(STATUS "KungFu Chess Observer System - Build Configuration:")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Source files: ${SOURCES}")
