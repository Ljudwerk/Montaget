cmake_minimum_required(VERSION 3.22)

project(Montaget VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Lägsta macOS-version")
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Universal binary")

option(MO_BUILD_TOOLS "Bygg MOSnapshot (bilder av panelen) och MORender (renderingstest)" ON)

# --- Beroenden -------------------------------------------------------------
# JUCE och clap-juce-extensions hämtas från nätet första gången. Finns redan en hämtad kopia i ett
# syskonprojekt (Kroppsmaskinens eller KrangBox byggmappar) används den i stället, så att inget behöver
# hämtas. En egen sökväg kan alltid anges:
#   cmake -B build -DFETCHCONTENT_SOURCE_DIR_JUCE=/sökväg/till/JUCE
# En sökväg som inte (längre) finns ignoreras, så att en gammal CMakeCache inte stoppar bygget.
include(FetchContent)

function(mo_hitta_lokal_kopia var underkatalog)
    if(DEFINED ${var} AND NOT "${${var}}" STREQUAL "" AND EXISTS "${${var}}/CMakeLists.txt")
        return()
    endif()
    set(hittad "")
    foreach(projekt rostverket pannrummet kropps-maskinen krang-box)
        foreach(bygg build-dev build)
            set(kandidat "${CMAKE_CURRENT_SOURCE_DIR}/../${projekt}/${bygg}/_deps/${underkatalog}")
            if(hittad STREQUAL "" AND EXISTS "${kandidat}/CMakeLists.txt")
                get_filename_component(hittad "${kandidat}" REALPATH)
            endif()
        endforeach()
    endforeach()
    if(hittad STREQUAL "")
        message(STATUS "Montaget: ${underkatalog} hämtas från nätet")
    else()
        message(STATUS "Montaget: använder ${hittad}")
    endif()
    set(${var} "${hittad}" CACHE PATH "Lokal kopia (tomt = hämta)" FORCE)
endfunction()

mo_hitta_lokal_kopia(FETCHCONTENT_SOURCE_DIR_JUCE juce-src)
mo_hitta_lokal_kopia(FETCHCONTENT_SOURCE_DIR_CLAP-JUCE-EXTENSIONS clap-juce-extensions-src)

FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE)

FetchContent_Declare(clap-juce-extensions
    GIT_REPOSITORY https://github.com/free-audio/clap-juce-extensions.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
    GIT_SUBMODULES_RECURSE TRUE)

FetchContent_MakeAvailable(JUCE clap-juce-extensions)

# Det delade biblioteket (header-only), bredvid den här mappen.
set(KRANG_LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../krang-libs" CACHE PATH "Sökväg till krang-libs")
add_subdirectory(${KRANG_LIBS_DIR} krang-libs)

# --- Pluginet --------------------------------------------------------------
set(MO_FORMATS VST3 Standalone)
if(APPLE)
    list(APPEND MO_FORMATS AU)
endif()

juce_add_plugin(Montaget
    COMPANY_NAME                "Montaget"
    BUNDLE_ID                   "org.montaget.mo85"
    PLUGIN_MANUFACTURER_CODE    Mntg
    PLUGIN_CODE                 Mo85
    PRODUCT_NAME                "Montaget"
    IS_SYNTH                    TRUE
    NEEDS_MIDI_INPUT            TRUE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    # Appen läser ljudkortets ingång: utan texten stänger macOS appen när den ber om mikrofonen.
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT  "Montaget spelar in ljud från mikrofonen eller ljudkortets ingång till sina platser."
    COPY_PLUGIN_AFTER_BUILD     FALSE
    VST3_CATEGORIES             Instrument Sampler
    AU_MAIN_TYPE                kAudioUnitType_MusicDevice
    FORMATS                     ${MO_FORMATS})

clap_juce_extensions_plugin(TARGET Montaget
    CLAP_ID "org.montaget.mo85"
    CLAP_FEATURES instrument sampler)

add_dependencies(Montaget_All Montaget_CLAP)

# Appen: eget fönster (krang/app/Fristaende.h) i stället för JUCE:s standardfönster.
target_sources(Montaget_Standalone PRIVATE Source/app/App.cpp)
target_compile_definitions(Montaget_Standalone PRIVATE JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=1)

set(MO_SOURCES
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_sources(Montaget PRIVATE ${MO_SOURCES})
target_include_directories(Montaget PRIVATE Source)

set(MO_DEFINITIONS
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0)   # Tillåtet under AGPLv3 / JUCE:s fria licens

target_compile_definitions(Montaget PUBLIC ${MO_DEFINITIONS})

target_link_libraries(Montaget
    PRIVATE
        krang::libs
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# --- Verktyg ---------------------------------------------------------------
if(MO_BUILD_TOOLS)
    # Båda verktygen bygger in samma processor och panel som pluginet.
    foreach(tool MOSnapshot MORender)
        juce_add_console_app(${tool} PRODUCT_NAME "${tool}")
        target_sources(${tool} PRIVATE tools/${tool}.cpp ${MO_SOURCES})
        target_include_directories(${tool} PRIVATE Source)
        target_compile_definitions(${tool} PRIVATE ${MO_DEFINITIONS} JUCE_MODAL_LOOPS_PERMITTED=1)
        target_link_libraries(${tool}
            PRIVATE krang::libs juce::juce_audio_utils juce::juce_audio_formats juce::juce_dsp
            PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
    endforeach()
endif()
