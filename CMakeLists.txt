cmake_minimum_required(VERSION 3.15)
project(hotreload VERSION 0.0.1)

# set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20)

# Linux-specific linker flags (from the example)
if("${CMAKE_SYSTEM}" MATCHES "Linux")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-Bsymbolic")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-Bsymbolic")
endif()

# JUCE setup
set(JUCE_DIR "/Users/aidan/dev/cpp/juce_cmake/JUCE")
add_subdirectory(${JUCE_DIR} JUCE)

# NanoVG setup
set(NANOVG_DIR vendor/nanovg)

if(APPLE)
    file(GLOB_RECURSE NANOVG_SRC
        ${NANOVG_DIR}/*.h
        ${NANOVG_DIR}/*.c
        ${NANOVG_DIR}/*.m
    )
else()
    file(GLOB_RECURSE NANOVG_SRC
        ${NANOVG_DIR}/*.h
        ${NANOVG_DIR}/*.c
    )
endif()

add_library(nanovg STATIC ${NANOVG_SRC})
target_include_directories(nanovg PUBLIC ${NANOVG_DIR})

# Enable automatic reference counting for macOS Metal support
if(APPLE)
    set_property(TARGET nanovg APPEND_STRING PROPERTY
        COMPILE_FLAGS "-fobjc-arc")
endif()

# Add binary data (font, images, etc.)
juce_add_binary_data(${PROJECT_NAME}Resources
    SOURCES
        assets/fonts/Roboto-Light.ttf
)

# Plugin configuration
juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME "MyCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE ftwn
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "${PROJECT_NAME}"
)

# Source files
target_sources(${PROJECT_NAME}
    PRIVATE
    source/PluginProcessor.cpp
    source/PluginEditor.cpp
    source/PluginProcessor.h
    source/PluginEditor.h
    source/JucePlatformLayer.cpp
    source/JucePlatformLayer.h
    source/ui.h
    source/ui.cpp
    source/ui_impl.h
    source/ui_impl.cpp
    source/CharmApp.h
    source/CharmApp.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_artefacts/JuceLibraryCode
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JucePlugin_Build_VST=0
    JUCE_DISABLE_JUCE_VERSION_PRINTING=1
    DONT_SET_USING_JUCE_NAMESPACE=1
)

# find_path(FFTW_INCLUDE_DIR fftw3.h
# HINTS /opt/homebrew/include /usr/local/include)

# find_library(FFTW_LIBRARY NAMES fftw3f
#     HINTS /opt/homebrew/lib /usr/local/lib)

# if(FFTW_INCLUDE_DIR AND FFTW_LIBRARY)
#     message(STATUS "Found FFTW: ${FFTW_LIBRARY}")
#     include_directories(${FFTW_INCLUDE_DIR})
#     set(FFTW_LIBRARIES ${FFTW_LIBRARY})
# else()
#     message(FATAL_ERROR "FFTW not found")
# endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${FFTW_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${FFTW_LIBRARIES})

# Platform-specific definitions
if(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC NANOVG_GL2_IMPLEMENTATION)
    target_compile_definitions(nanovg PUBLIC NANOVG_GL2_IMPLEMENTATION)

else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC NANOVG_GLES2_IMPLEMENTATION)
    target_compile_definitions(nanovg PUBLIC NANOVG_GLES2_IMPLEMENTATION)
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    juce::juce_audio_utils
    juce::juce_opengl
    nanovg
    ${PROJECT_NAME}Resources
)

# Platform-specific linking
if(APPLE)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        "-framework Metal"
    )
endif()

if("${CMAKE_SYSTEM}" MATCHES "Linux")
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
        -lGL -lGLEW -lGLX
    )
endif()

# Custom run target to launch the standalone app after building
if(APPLE)
    add_custom_target(run
        COMMAND open ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_artefacts/Standalone/${PROJECT_NAME}.app
        DEPENDS ${PROJECT_NAME}_Standalone
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Launching ${PROJECT_NAME} standalone app..."
    )
endif()
