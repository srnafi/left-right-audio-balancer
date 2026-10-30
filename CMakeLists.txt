cmake_minimum_required(VERSION 3.20)
project(LeftRightAudioBalancer CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/CardRenderer.cpp
    src/Widgets.cpp
    src/VectorIcons.cpp
    src/GraphicsResources.cpp
    src/AudioEngine.cpp
    src/AvrcpHelper.cpp
    src/SettingsStore.cpp
    include/MainWindow.h
    include/CardRenderer.h
    include/Widgets.h
    include/VectorIcons.h
    include/GraphicsResources.h
    include/AudioEngine.h
    include/AvrcpHelper.h
    include/SettingsStore.h
    include/AppState.h
    include/resource.h
    res/resource.rc
)

add_executable(LeftRightAudioBalancer WIN32 ${SOURCES})

target_link_libraries(LeftRightAudioBalancer PRIVATE
    user32
    gdi32
    ole32
    shell32
    shlwapi
    advapi32
    d2d1
    dwrite
    dwmapi
)

set_target_properties(LeftRightAudioBalancer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)
