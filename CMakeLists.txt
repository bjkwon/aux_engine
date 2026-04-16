cmake_minimum_required(VERSION 3.20)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" AUXE_VERSION_RAW)
string(STRIP "${AUXE_VERSION_RAW}" AUXE_VERSION)
if(AUXE_VERSION STREQUAL "")
  set(AUXE_VERSION "0.0.0")
endif()

project(auxe VERSION ${AUXE_VERSION} LANGUAGES C CXX)

# -----------------------------
# Options
# -----------------------------
option(AUXE_BUILD_SHARED      "Build auxe as a shared library" ON)
option(AUXE_ENABLE_PORTAUDIO  "Enable PortAudio-dependent built-ins" OFF)
option(AUXE_BUILD_TESTS       "Build auxe regression tests" ON)

# -----------------------------
# Language / compile settings
# -----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/W3)
else()
  add_compile_options(-Wall -Wextra)
endif()

# -----------------------------
# Target type
# -----------------------------
if(AUXE_BUILD_SHARED)
  set(AUXE_LIB_TYPE SHARED)
else()
  set(AUXE_LIB_TYPE STATIC)
endif()

# -----------------------------
# Sources
#   NOTE: you consolidated builtins into src/func/*.cpp, so this matches that.
#   If you later reintroduce subfolders, change GLOB -> GLOB_RECURSE intentionally.
# -----------------------------
file(GLOB AUXE_ENGINE_CPP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.cpp")
file(GLOB AUXE_ENGINE_C   CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.c")

file(GLOB AUXE_API_CPP    CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/api/*.cpp")

file(GLOB AUXE_FUNC_CPP   CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/func/*.cpp")
file(GLOB AUXE_FUNC_C     CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/func/*.c")

file(GLOB AUXE_IIR_C      CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/iir/*.c")

# If you have generated parser/lexer sources, include them here if they are part of auxe:
file(GLOB AUXE_PARSE_C    CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/grammar/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/grammar/*.cpp"
)

add_library(auxe ${AUXE_LIB_TYPE}
  ${AUXE_ENGINE_CPP}
  ${AUXE_ENGINE_C}
  ${AUXE_API_CPP}
  ${AUXE_FUNC_CPP}
  ${AUXE_FUNC_C}
  ${AUXE_IIR_C}
  ${AUXE_PARSE_C}
)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/AuxeVersion.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/generated/AuxeVersion.h"
  @ONLY
)

# -----------------------------
# Includes
# -----------------------------
target_include_directories(auxe
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api
    ${CMAKE_CURRENT_SOURCE_DIR}/src/func
)

# -----------------------------
# MSVC: avoid std::byte vs Windows SDK byte ambiguity (your known workaround)
# -----------------------------
if(MSVC)
  target_compile_definitions(auxe PRIVATE _HAS_STD_BYTE=0)
  target_compile_definitions(auxe PRIVATE YY_NO_UNISTD_H=1)
endif()

if(MSVC)
  target_compile_options(auxe PRIVATE /wd4267)
  target_compile_options(auxe PRIVATE /wd4244)
  target_compile_options(auxe PRIVATE /wd4267)
  target_compile_options(auxe PRIVATE /wd4996)
endif()

# -----------------------------
# Windows DLL export macro (optional; keep if your headers use it)
# -----------------------------
if(WIN32 AND AUXE_BUILD_SHARED)
  target_compile_definitions(auxe PRIVATE AUXE_BUILD_DLL=1)
endif()

# -----------------------------
# Dependencies
#   auxe has NO file/codec deps.
#   Required: FFTW3 + libsamplerate
#   Optional: PortAudio
# -----------------------------

# Header-only JSON (auxe uses this in some parts; keep if you include it in engine code)
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(auxe PRIVATE nlohmann_json::nlohmann_json)

# Helper to link a lib robustly:
# 1) Try CMake package targets (vcpkg-friendly)
# 2) Fallback to find_library for system installs
function(auxe_link_first_found target)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs TARGETS LIBNAMES)
  cmake_parse_arguments(AL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  foreach(tgt IN LISTS AL_TARGETS)
    if(TARGET ${tgt})
      target_link_libraries(${target} PRIVATE ${tgt})
      return()
    endif()
  endforeach()

  foreach(lib IN LISTS AL_LIBNAMES)
    find_library(_found_${lib} NAMES ${lib})
    if(_found_${lib})
      target_link_libraries(${target} PRIVATE ${_found_${lib}})
      return()
    endif()
  endforeach()

  message(FATAL_ERROR "Could not find any of: targets=[${AL_TARGETS}] or libraries=[${AL_LIBNAMES}].")
endfunction()

# ---- FFTW3 ----
# Try common vcpkg target names, then fallback to lib name "fftw3".
find_package(fftw3 CONFIG QUIET)
find_package(FFTW3 CONFIG QUIET)
auxe_link_first_found(auxe
  TARGETS
    fftw3::fftw3
    FFTW3::fftw3
  LIBNAMES
    fftw3
)

# ---- libsamplerate ----
find_package(SampleRate CONFIG QUIET)
find_package(samplerate CONFIG QUIET)
auxe_link_first_found(auxe
  TARGETS
    samplerate::samplerate
    SampleRate::samplerate
    SampleRate::SampleRate
  LIBNAMES
    samplerate
    samplerate-0
)



# Linux extras (safe; does nothing on Windows)
if(UNIX AND NOT APPLE)
  target_link_libraries(auxe PRIVATE pthread m dl)
endif()

# -----------------------------
# Installation + CMake package export
# -----------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Install library
install(TARGETS auxe
  EXPORT auxeTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Install headers (entire include tree)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Export targets file
install(EXPORT auxeTargets
  FILE auxeTargets.cmake
  NAMESPACE auxe::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/auxe
)

# Generate a minimal auxeConfig.cmake without requiring a separate .in file in the repo
set(_auxe_config_in "${CMAKE_CURRENT_BINARY_DIR}/auxeConfig.cmake.in")
file(WRITE "${_auxe_config_in}"
"@PACKAGE_INIT@\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/auxeTargets.cmake\")\n"
)

configure_package_config_file(
  "${_auxe_config_in}"
  "${CMAKE_CURRENT_BINARY_DIR}/auxeConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/auxe
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/auxeConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/auxeConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/auxeConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/auxe
)

if(AUXE_BUILD_TESTS)
  enable_testing()

  add_executable(auxe_regression_debug_resume
    ${CMAKE_CURRENT_SOURCE_DIR}/test/regression_debug_resume.cpp
  )
  target_link_libraries(auxe_regression_debug_resume PRIVATE auxe)
  target_include_directories(auxe_regression_debug_resume PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

  add_test(
    NAME auxe_regression_debug_resume
    COMMAND auxe_regression_debug_resume
  )
endif()
