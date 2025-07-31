cmake_minimum_required(VERSION 3.16)
project(KungFuChess)

set(CMAKE_CXX_STANDARD 17)

# OpenCV
set(OpenCV_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/OpenCV_451/include")
set(OpenCV_LIBS "${CMAKE_CURRENT_SOURCE_DIR}/OpenCV_451/bin/opencv_world451.lib")
include_directories(${OpenCV_INCLUDE_DIRS})

# Shared library
file(GLOB_RECURSE SHARED_SOURCES 
    "shared/core/Board.cpp"
    "shared/core/Game.cpp"
    "shared/game_logic/*.cpp"
    "shared/graphics/Graphics.cpp"
    "shared/graphics/img/OpenCvImg.cpp"
    "shared/observer/src/BlackMovesTable.cpp"
    "shared/observer/src/BlackScoreTracker.cpp"
    "shared/observer/src/GameEventManager.cpp"
    "shared/observer/src/SoundManager.cpp"
    "shared/observer/src/TemplateInstantiations.cpp"
    "shared/observer/src/VoiceAnnouncer.cpp"
    "shared/observer/src/WhiteMovesTable.cpp"
    "shared/observer/src/WhiteScoreTracker.cpp"
)

add_library(shared_lib ${SHARED_SOURCES})
target_include_directories(shared_lib PUBLIC shared shared/utils/json)
target_link_libraries(shared_lib ${OpenCV_LIBS})

# Server executable
add_executable(server 
    server/main_server.cpp
    server/network/GameServer.cpp
    server/network/GameServerLogic.cpp
)
target_include_directories(server PRIVATE server shared)
target_link_libraries(server shared_lib winmm)

# Client executable  
add_executable(client
    client/main_client.cpp
    client/network/GameClient.cpp
)
target_include_directories(client PRIVATE client shared)
target_link_libraries(client shared_lib ${OpenCV_LIBS} winmm)

# Tests
file(GLOB TEST_SOURCES "tests/test_*.cpp")
add_executable(tests tests/test_main.cpp ${TEST_SOURCES})
target_include_directories(tests PRIVATE shared shared/utils/json tests)
target_link_libraries(tests shared_lib ${OpenCV_LIBS})