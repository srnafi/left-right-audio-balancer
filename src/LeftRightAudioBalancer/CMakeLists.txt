cmake_minimum_required(VERSION 3.20)
project(LeftRightAudioBalancer CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)

set(SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    CardRenderer.cpp
    CardRenderer.h
    Widgets.cpp
    Widgets.h
    VectorIcons.cpp
    VectorIcons.h
    GraphicsResources.cpp
    GraphicsResources.h
    AudioEngine.cpp
    AudioEngine.h
    AvrcpHelper.cpp
    AvrcpHelper.h
    SettingsStore.cpp
    SettingsStore.h
    AppState.h
    resource.h
    resource.rc
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
