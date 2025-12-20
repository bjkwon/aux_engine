cmake_minimum_required(VERSION 3.20)

project(auxe LANGUAGES C CXX)

# Header-only JSON (still fine in auxe)
find_package(nlohmann_json CONFIG REQUIRED)

option(AUXE_BUILD_SHARED "Build auxe as a shared library" ON)
option(AUXE_ENABLE_PORTAUDIO "Enable PortAudio-dependent built-ins" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(AUXE_BUILD_SHARED)
  set(AUXE_LIB_TYPE SHARED)
else()
  set(AUXE_LIB_TYPE STATIC)
endif()

# ---- Sources ----
file(GLOB AUXE_ENGINE_CPP CONFIGURE_DEPENDS src/engine/*.cpp)
file(GLOB AUXE_ENGINE_C   CONFIGURE_DEPENDS src/engine/*.c)
file(GLOB AUXE_API_CPP    CONFIGURE_DEPENDS src/api/*.cpp)
file(GLOB AUXE_FUNC_CPP   CONFIGURE_DEPENDS src/func/*.cpp)
file(GLOB AUXE_IIR_C      CONFIGURE_DEPENDS src/iir/*.c)

add_library(auxe ${AUXE_LIB_TYPE}
  ${AUXE_ENGINE_CPP}
  ${AUXE_ENGINE_C}
  ${AUXE_API_CPP}
  ${AUXE_FUNC_CPP}
  ${AUXE_IIR_C}
)

# ---- Target deps ----
target_link_libraries(auxe PRIVATE nlohmann_json::nlohmann_json)

if (MSVC)
  target_compile_definitions(auxe PRIVATE _HAS_STD_BYTE=0)
endif()

target_include_directories(auxe
  PUBLIC
    # Public consumers should include as:  #include <auxe/aux2_core.h>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    # Engine code currently includes "aux2_core.h" directly.
    # Keeping this during the transition avoids a mass-edit.
    ${CMAKE_CURRENT_SOURCE_DIR}/include/auxe
    ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
    ${CMAKE_CURRENT_SOURCE_DIR}/src/api
    ${CMAKE_CURRENT_SOURCE_DIR}/src/func
    ${CMAKE_CURRENT_SOURCE_DIR}/include/thirdparty
)

# Export macro for Windows DLL builds
if(WIN32 AND AUXE_BUILD_SHARED)
  target_compile_definitions(auxe PRIVATE AUXE_BUILD_DLL)
endif()

# ---- External dependencies (system libs) ----
include(CheckLibraryExists)

# Helper: link a library if found; otherwise error with a clear message.
function(auxe_link_required target libname)
  if(TARGET ${libname})
    target_link_libraries(${target} PRIVATE ${libname})
    return()
  endif()
  find_library(_lib_${libname} NAMES ${libname})
  if(_lib_${libname})
    target_link_libraries(${target} PRIVATE ${_lib_${libname}})
  else()
    message(FATAL_ERROR "Required library '${libname}' not found. Install it (or provide a toolchain/vcpkg).")
  endif()
endfunction()

# Keep DSP/math deps (not file-format I/O):
auxe_link_required(auxe samplerate)
auxe_link_required(auxe fftw3)

# Optional real-time audio device I/O (UI-like; OFF by default here)
if(AUXE_ENABLE_PORTAUDIO)
  auxe_link_required(auxe portaudio)
  target_compile_definitions(auxe PRIVATE AUXE_ENABLE_PORTAUDIO=1)
else()
  target_compile_definitions(auxe PRIVATE AUXE_ENABLE_PORTAUDIO=0)
endif()

if(UNIX AND NOT APPLE)
  target_link_libraries(auxe PRIVATE pthread m dl)
endif()

# ---- Install (optional but useful for consumers) ----
include(GNUInstallDirs)
install(TARGETS auxe
  EXPORT auxeTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES include/auxe/aux2_core.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/auxe)

install(EXPORT auxeTargets
  FILE auxeTargets.cmake
  NAMESPACE auxe::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/auxe
)
