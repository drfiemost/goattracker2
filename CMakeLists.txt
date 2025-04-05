cmake_minimum_required(VERSION 3.9)
project(goattracker2)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" OR
        "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options("$<$<CONFIG:DEBUG>:-Wall;-Wextra;-pedantic>")
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result)
    message(STATUS "IPO supported by compiler, enabled for release builds")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
else()
    message(WARNING "IPO is not supported: ${output}")
endif()

option(INLINING "enable inlining of functions" ON)
option(BRANCH_HINTS "enable static branch prediction hints" ON)
option(NEW_FILTER "enable new 8580 filter implementation" ON)

if(NEW_FILTER)
    set(FILTER_SRC src/resid/filter8580new.cc)
else()
    set(FILTER_SRC src/resid/filter.cc)
endif()

set(GOATTRK2_SOURCES
    ${CMAKE_BINARY_DIR}/src/goatdata.c
    src/goattrk2.c
    src/gsong.c
    src/gorder.c
    src/gpattern.c
    src/ginstr.c
    src/gtable.c
    src/gplay.c
    src/gdisplay.c
    src/gfile.c
    src/greloc.c
    src/ghelp.c
    src/gsound.c
    src/gconsole.c
    src/gsid.cpp
    src/resid/dac.cc
    src/resid/envelope.cc
    src/resid/extfilt.cc
    ${FILTER_SRC}
    src/resid/pot.cc
    src/resid/sid.cc
    src/resid/wave.cc
    src/resid/voice.cc
    src/asm/asmtab.c
    src/asm/chnkpool.c
    src/asm/expr.c
    src/asm/lexyy.c
    src/asm/log.c
    src/asm/membuf.c
    src/asm/membufio.c
    src/asm/namedbuf.c
    src/asm/parse.c
    src/asm/pc.c
    src/asm/vec.c
    src/bme/bme_gfx.c
    src/bme/bme_snd.c
    src/bme/bme_win.c
    src/bme/bme_mou.c
    src/bme/bme_kbd.c
    src/bme/bme_io.c
    src/bme/bme_end.c
    src/bme/bme.c
    src/rtmidi/rtmidi_c.h
    src/rtmidi/rtmidi_c.cpp
    src/rtmidi/RtMidi.h
    src/rtmidi/RtMidi.cpp)

if(MINGW)
    list(APPEND GOATTRK2_SOURCES
        src/goattrk2.rc
    )
endif(MINGW)

# it would be nice not having to link things like resid, however the source is
# not ready for that
set(GT2RELOC_SOURCES
    ${CMAKE_BINARY_DIR}/src/goatdata.c
    src/gt2reloc.c
    src/gsong.c
    src/gorder.c
    src/gpattern.c
    src/ginstr.c
    src/gtable.c
    src/gplay.c
    src/gdisplay.c
    src/gfile.c
    src/ghelp.c
    src/gsound.c
    src/gconsole.c
    src/gsid.cpp
    src/resid/dac.cc
    src/resid/envelope.cc
    src/resid/extfilt.cc
    ${FILTER_SRC}
    src/resid/pot.cc
    src/resid/sid.cc
    src/resid/wave.cc
    src/resid/voice.cc
    src/asm/asmtab.c
    src/asm/chnkpool.c
    src/asm/expr.c
    src/asm/lexyy.c
    src/asm/log.c
    src/asm/membuf.c
    src/asm/membufio.c
    src/asm/namedbuf.c
    src/asm/parse.c
    src/asm/pc.c
    src/asm/vec.c
    src/bme/bme_gfx.c
    src/bme/bme_snd.c
    src/bme/bme_win.c
    src/bme/bme_mou.c
    src/bme/bme_kbd.c
    src/bme/bme_io.c
    src/bme/bme_end.c
    src/bme/bme.c)

set(MOD2SNG_SOURCES
    src/mod2sng.c
    src/bme/bme_end.c)

set(INS2SND2_SOURCES
    src/ins2snd2.c
    src/bme/bme_end.c)

set(SNGSPLI2_SOURCES
    src/sngspli2.c
    src/bme/bme_end.c)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/bme)
include_directories(${CMAKE_SOURCE_DIR}/src/asm)
include_directories(${CMAKE_BINARY_DIR}/src/resid)

find_package(SDL2 REQUIRED)

get_target_property(SDL2_INCLUDE_DIRS SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)

add_executable(datafile src/bme/datafile.c src/bme/bme_end.c)
add_executable(dat2inc src/bme/dat2inc.c)

target_include_directories(datafile PUBLIC ${SDL2_INCLUDE_DIRS})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/src)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/goattrk2.dat
  COMMAND datafile ${CMAKE_BINARY_DIR}/src/goattrk2.dat goattrk2.seq
  DEPENDS datafile
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src
  )

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/goatdata.c
  COMMAND dat2inc goattrk2.dat goatdata.c
  DEPENDS dat2inc ${CMAKE_BINARY_DIR}/src/goattrk2.dat
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src
  )

add_executable(goattrk2 ${GOATTRK2_SOURCES})
add_executable(gt2reloc ${GT2RELOC_SOURCES})
add_executable(mod2sng ${MOD2SNG_SOURCES})
add_executable(ins2snd2 ${INS2SND2_SOURCES})
add_executable(sngspli2 ${SNGSPLI2_SOURCES})

add_custom_target(goatdata DEPENDS ${CMAKE_BINARY_DIR}/src/goatdata.c)
add_dependencies(goattrk2 goatdata)
add_dependencies(gt2reloc goatdata)

target_include_directories(mod2sng PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(ins2snd2 PUBLIC ${SDL2_INCLUDE_DIRS})
target_include_directories(sngspli2 PUBLIC ${SDL2_INCLUDE_DIRS})

if(MINGW)
    set(LIB_MINGW mingw32)
endif()

if(TARGET SDL2::SDL2main)
    target_link_libraries(goattrk2 ${LIB_MINGW} SDL2::SDL2main)
    target_link_libraries(gt2reloc ${LIB_MINGW} SDL2::SDL2main)
endif()

OPTION(STATIC "Static build" OFF)
IF(STATIC)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
    target_link_libraries(goattrk2 SDL2::SDL2-static)
    target_link_libraries(gt2reloc SDL2::SDL2-static)
ELSE(STATIC)
    target_link_libraries(goattrk2 SDL2::SDL2)
    target_link_libraries(gt2reloc SDL2::SDL2)
ENDIF(STATIC)

find_package(Jack)
IF(JACK_FOUND)
    target_compile_definitions(goattrk2 PUBLIC USE_JACK __UNIX_JACK__)
    include_directories(${JACK_INCLUDE_DIR})
    target_link_libraries(goattrk2 ${JACK_LIBRARY})
    set(use_midi ON)
ENDIF(JACK_FOUND)

find_package(ALSA)
if (ALSA_FOUND)
    target_compile_definitions(goattrk2 PUBLIC __LINUX_ALSA__)
    include_directories(${ALSA_INCLUDE_DIR})
    target_link_libraries(goattrk2 ${ALSA_LIBRARY} pthread)
    set(use_midi ON)
endif(ALSA_FOUND)

if(APPLE)
    find_library(COREMIDI CoreMIDI)
    find_library(COREAUDIO CoreAudio)
    find_library(COREFOUNDATION CoreFoundation)
    target_compile_definitions(goattrk2 PUBLIC __MACOSX_CORE__)
    target_link_libraries(goattrk2 ${COREMIDI} ${COREAUDIO} ${COREFOUNDATION} pthread)
    set(use_midi ON)
endif(APPLE)

if(WIN32 OR MINGW)
    target_compile_definitions(goattrk2 PUBLIC __WINDOWS_MM__)
    target_link_libraries(goattrk2 winmm)
    set(use_midi ON)
endif()

if (use_midi)
    target_compile_definitions(goattrk2 PUBLIC USE_MIDI_INPUT)
endif(use_midi)

find_package(ExSID)
if (EXSID_FOUND)
    target_compile_definitions(goattrk2 PUBLIC USE_EXSID)
    include_directories(${EXSID_INCLUDE_DIR})
    target_link_libraries(goattrk2 ${EXSID_LIBRARY})
endif(EXSID_FOUND)

# create siddefs.h for resid

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("int main() { __builtin_expect(0, 0); }" HAVE_BUILTIN_EXPECT)
check_cxx_source_compiles("include <math.h>\nint main() { double x = log1p(1.0); }" HAVE_LOG1P)

macro(get_bool var)
     if(${ARGN})
         set(${var} 1)
     else()
         set(${var} 0)
     endif()
endmacro()

get_bool(RESID_INLINING ${INLINING})
set(RESID_INLINE "inline")
get_bool(RESID_BRANCH_HINTS ${BRANCH_HINTS})
get_bool(NEW_8580_FILTER ${NEW_FILTER})
set(HAVE_BOOL "1")
get_bool(HAVE_BUILTIN_EXPECT ${HAVE_BUILTIN_EXPECT})
get_bool(HAVE_LOG1P ${HAVE_LOG1P})

configure_file(${CMAKE_SOURCE_DIR}/src/resid/siddefs.h.in ${CMAKE_BINARY_DIR}/src/resid/siddefs.h @ONLY)
