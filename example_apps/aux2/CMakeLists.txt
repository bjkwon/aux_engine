cmake_minimum_required(VERSION 3.21)

project(aux2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(AUX2_USE_INSTALLED_AUXE "Link against an installed auxe package" ON)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(portaudio CONFIG REQUIRED)

# -------------------------
# auxe dependency
# -------------------------
if(AUX2_USE_INSTALLED_AUXE)
  find_package(auxe CONFIG REQUIRED)  # provides auxe::auxe
else()
  add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../.. ${CMAKE_BINARY_DIR}/auxe_build)
endif()

# -------------------------
# aux2 executable
# -------------------------
add_executable(aux2
  main.cpp
  auxenv.cpp
  utils.cpp
)

target_include_directories(aux2 PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(aux2 PRIVATE
  nlohmann_json::nlohmann_json
  portaudio
)

if(AUX2_USE_INSTALLED_AUXE)
  target_link_libraries(aux2 PRIVATE auxe::auxe)
else()
  target_link_libraries(aux2 PRIVATE auxe)
endif()

if(MSVC)
  target_compile_definitions(aux2 PRIVATE _HAS_STD_BYTE=0)
endif()

# -------------------------
# Install (exe + runtime DLL closure)
# -------------------------
include(GNUInstallDirs)

# Install the executable itself
install(TARGETS aux2
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
  # Where aux2.exe will be after build (multi-config VS generator)
  set(_aux2_exe "$<TARGET_FILE:aux2>")

  install(CODE [[
    message(STATUS "Collecting runtime DLL dependencies for aux2...")

    # Always install deps into <prefix>/bin
    set(_dest "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin")
    file(MAKE_DIRECTORY "${_dest}")

    # Search dirs to resolve DLLs
    set(_search_dirs "")
    list(APPEND _search_dirs "$<TARGET_FILE_DIR:aux2>")
    list(APPEND _search_dirs "C:/Users/bjehk/Documents/dev/aux_engine/install/bin")
    if(DEFINED ENV{VCPKG_ROOT})
      list(APPEND _search_dirs "$ENV{VCPKG_ROOT}/installed/x64-windows/bin")
    endif()

    file(GET_RUNTIME_DEPENDENCIES
      EXECUTABLES "$<TARGET_FILE:aux2>"
      RESOLVED_DEPENDENCIES_VAR _resolved
      UNRESOLVED_DEPENDENCIES_VAR _unresolved
      DIRECTORIES ${_search_dirs}

      # Exclude API-set forwarders etc.
      PRE_EXCLUDE_REGEXES
        "api-ms-win-.*"
        "ext-ms-.*"

      # Exclude Windows/system DLLs (note: CMake uses forward slashes in paths)
      POST_EXCLUDE_REGEXES
        "^[A-Za-z]:/Windows/.*"
        "^[A-Za-z]:/WINDOWS/.*"
        ".*/System32/.*"
        ".*/system32/.*"
    )

    if(_unresolved)
      message(STATUS "Unresolved deps (ignored): ${_unresolved}")
    endif()

    foreach(dll IN LISTS _resolved)
      message(STATUS "Installing runtime dep: ${dll}")
      file(INSTALL
        DESTINATION "${_dest}"
        TYPE SHARED_LIBRARY
        FILES "${dll}"
      )
    endforeach()
  ]])
endif()

# -------------------------
# Build-tree convenience: copy runtime DLLs next to aux2.exe after build
# -------------------------
if(WIN32)
  add_custom_command(TARGET aux2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Post-build: copying runtime DLLs next to aux2.exe (if any)..."
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:aux2> $<TARGET_FILE_DIR:aux2>
    COMMAND_EXPAND_LISTS
  )
endif()
