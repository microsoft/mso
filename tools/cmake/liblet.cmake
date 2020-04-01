# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# Store the liblet.cmake file directory in a variable
set(LIBLET_CMAKE_DIR ${CMAKE_CURRENT_LIST_DIR})

###########################################
# Input parameters
###########################################

# Set the target platform we compile code for.
set(MSO_LIBLET_PLATFORM WIN32 CACHE STRING "Liblet platform")
set_property(CACHE MSO_LIBLET_PLATFORM PROPERTY STRINGS ANDROID IOS LINUX MAC WIN32 WINRT)
message(STATUS "Liblet platform: ${MSO_LIBLET_PLATFORM}")

# Set the target platform we compile code for.
set(MSO_LIBLET_TEST GTEST CACHE STRING "Liblet test runner")
set_property(CACHE MSO_LIBLET_TEST PROPERTY STRINGS GTEST XCTEST)
message(STATUS "Liblet test runner: ${MSO_LIBLET_TEST}")

# we will use the network to fetch Google Test sources
# make it possible to disable unit tests when not on network
option(MSO_ENABLE_UNIT_TESTS "Enable unit tests" ON)
message(STATUS "Enable testing: ${MSO_ENABLE_UNIT_TESTS}")

# For each include file we generate a simple .cpp file to check that it is self-contained.
# We allow to disable it because it takes extra compile time
option(MSO_ENABLE_INCLUDES_CHECKS "Enable checking of include files" ON)
message(STATUS "Enable includes checks: ${MSO_ENABLE_INCLUDES_CHECKS}")

###########################################
# Compiler settings
###########################################

# require C++14
set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_EXTENSIONS OFF)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(WIN_TARGET_ARCH $ENV{VSCMD_ARG_TGT_ARCH})
  if(${WIN_TARGET_ARCH} STREQUAL "")
    message(FATAL_ERROR "Error: environment variable VSCMD_ARG_TGT_ARCH is not set (require for Windows builds)")
  endif()
endif()

function(liblet LIBLET_TARGET)
  # DEPENDS         - liblet dependencies for all platforms
  # DEPENDS_ANDROID - liblet dependencies for Android
  # DEPENDS_APPLE   - liblet dependencies for Apple platforms (macOS or iOS)
  # DEPENDS_IOS     - liblet dependencies for iOS
  # DEPENDS_LINUX   - liblet dependencies for Linux
  # DEPENDS_MAC     - liblet dependencies for macOS
  # DEPENDS_POSIX   - liblet dependencies for Android, Apple and Linux platforms
  # DEPENDS_WIN     - liblet dependencies for all Windows platforms
  # DEPENDS_WIN32   - liblet dependencies for Win32
  # DEPENDS_WINRT   - liblet dependencies for WinRT
  _liblet_platform_args(${LIBLET_TARGET} "DEPENDS" ${ARGN})

  # We always create STATIC library since INTERFACE libraries have limitations
  # such as not allowing custom commands.
  message(STATUS "Defining liblet Mso::${LIBLET_TARGET}")
  add_library(${LIBLET_TARGET} STATIC)
  add_library(Mso::${LIBLET_TARGET} ALIAS ${LIBLET_TARGET})

  _liblet_set_platform_definitions(${LIBLET_TARGET})

  target_link_libraries(${LIBLET_TARGET} PUBLIC ${${LIBLET_TARGET}_DEPENDS})

  # Process public headers if include/CMakeLists.txt exists
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/CMakeLists.txt")
    add_subdirectory(include)
  endif()

  # Process source files if src/CMakeLists.txt exists
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt")
    add_subdirectory(src)
  else()
    target_sources(${LIBLET_TARGET} PRIVATE ${LIBLET_CMAKE_DIR}/empty.cpp)
  endif()

  # Process test files if tests/CMakeLists.txt exists and tests are enabled
  if(MSO_ENABLE_UNIT_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
  endif()

  # Install target
  install(
    TARGETS ${LIBLET_TARGET}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${LIBLET_TARGET})
endfunction()

function(liblet_includes)
  # INCLUDES         - includes for all platforms
  # INCLUDES_ANDROID - includes for Android
  # INCLUDES_APPLE   - includes for Apple platforms (macOS or iOS)
  # INCLUDES_IOS     - includes for iOS
  # INCLUDES_LINUX   - includes for macOS
  # INCLUDES_MAC     - includes for Linux
  # INCLUDES_POSIX   - includes for Android and Apple platforms
  # INCLUDES_WIN     - includes for all Windows platforms
  # INCLUDES_WIN32   - includes for Win32
  # INCLUDES_WINRT   - includes for WinRT
  _liblet_platform_args(${LIBLET_TARGET} "INCLUDES" ${ARGN})

  # Export the public includes directory
  target_include_directories(${LIBLET_TARGET}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
  )

  # Add the includes sources for showing in IDE.
  target_sources(${LIBLET_TARGET} PUBLIC ${${LIBLET_TARGET}_INCLUDES})

  # Install target include sources
  foreach(FILE ${${LIBLET_TARGET}_INCLUDES})
    get_filename_component(DIR ${FILE} DIRECTORY)
    install(
      FILES ${FILE}
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${DIR}
    )
  endforeach()

  if(${MSO_ENABLE_INCLUDES_CHECKS} AND ${LIBLET_TARGET}_INCLUDES)
    _liblet_verify_includes(${LIBLET_TARGET} "${${LIBLET_TARGET}_INCLUDES}")
  endif()
endfunction()

function(liblet_sources)
  # SOURCES         - sources for all platforms
  # SOURCES_ANDROID - sources for Android
  # SOURCES_APPLE   - sources for Apple platforms (macOS or iOS)
  # SOURCES_IOS     - sources for iOS
  # SOURCES_LINUX   - sources for Linux
  # SOURCES_MAC     - sources for macOS
  # SOURCES_POSIX   - sources for Android and Apple platforms
  # SOURCES_WIN     - sources for all Windows platforms
  # SOURCES_WIN32   - sources for Win32
  # SOURCES_WINRT   - sources for WinRT
  _liblet_platform_args(${LIBLET_TARGET} "SOURCES" ${ARGN})

  # Add the source files to target sources.
  target_sources(${LIBLET_TARGET} PRIVATE ${${LIBLET_TARGET}_SOURCES})
endfunction()

function(liblet_tests)
  # SOURCES         - test sources for all platforms
  # SOURCES_ANDROID - test sources for Android
  # SOURCES_APPLE   - test sources for Apple platforms (macOS or iOS)
  # SOURCES_IOS     - test sources for iOS
  # SOURCES_LINUX   - test sources for Linux
  # SOURCES_MAC     - test sources for macOS
  # SOURCES_POSIX   - test sources for Android and Apple platforms
  # SOURCES_WIN     - test sources for all Windows platforms
  # SOURCES_WIN32   - test sources for Win32
  # SOURCES_WINRT   - test sources for WinRT
  # DEPENDS         - additional dependencies for all platforms
  # DEPENDS_ANDROID - additional dependencies for Android
  # DEPENDS_APPLE   - additional dependencies for Apple platforms (macOS or iOS)
  # DEPENDS_IOS     - additional dependencies for iOS
  # DEPENDS_LINUX   - additional dependencies for Linux
  # DEPENDS_MAC     - additional dependencies for macOS
  # DEPENDS_POSIX   - additional dependencies for Android and Apple platforms
  # DEPENDS_WIN     - additional dependencies for all Windows platforms
  # DEPENDS_WIN32   - additional dependencies for Win32
  # DEPENDS_WINRT   - additional dependencies for WinRT
  set(LIBLET_TESTS_TARGET ${LIBLET_TARGET}_tests)

  _liblet_platform_args(${LIBLET_TESTS_TARGET} "SOURCES;DEPENDS" ${ARGN})

  if(${MSO_LIBLET_TEST} STREQUAL GTEST)
    add_executable(${LIBLET_TESTS_TARGET} ${${LIBLET_TESTS_TARGET}_SOURCES})
    add_test(
      NAME ${LIBLET_TESTS_TARGET}
      COMMAND $<TARGET_FILE:${LIBLET_TESTS_TARGET}>
    )
  elseif(${MSO_LIBLET_TEST} STREQUAL XCTEST)
    find_package(XCTest REQUIRED)
    xctest_add_bundle(${LIBLET_TESTS_TARGET} ${LIBLET_TARGET} ${${LIBLET_TESTS_TARGET}_SOURCES})
#[[
    set(INFO_PLIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
    configure_file(${LIBLET_CMAKE_DIR}/XCTestInfo.plist.in ${INFO_PLIST_FILE} @ONLY)
    set_target_properties(${LIBLET_TESTS_TARGET} PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST "${INFO_PLIST_FILE}"
    )
]]
    xctest_add_test(XCTest.${LIBLET_TARGET} ${LIBLET_TESTS_TARGET})
  endif()

  _liblet_set_platform_definitions(${LIBLET_TESTS_TARGET})

  target_link_libraries(${LIBLET_TESTS_TARGET}
    PRIVATE
      Mso::${LIBLET_TARGET}
      Mso::motifCpp
      ${${LIBLET_TESTS_TARGET}_DEPENDS}
  )

  if(${MSO_LIBLET_PLATFORM} STREQUAL WIN32 OR ${MSO_LIBLET_PLATFORM} STREQUAL WINRT)
    install(
      TARGETS ${LIBLET_TESTS_TARGET}
      DESTINATION ${MSO_LIBLET_PLATFORM}/${CMAKE_BUILD_TYPE}/${WIN_TARGET_ARCH}
      COMPONENT tests
    )
  else()
    install(
      TARGETS ${LIBLET_TESTS_TARGET}
      DESTINATION ${MSO_LIBLET_PLATFORM}/${CMAKE_BUILD_TYPE}
      COMPONENT tests
    )
  endif()
endfunction()

function(_liblet_platform_args RESULT_PREFIX ARG_PREFIXES)
  set(ARG_KEYWORDS ${ARG_PREFIXES})
  foreach(ARG_PREFIX ${ARG_PREFIXES})
    list(APPEND ARG_KEYWORDS
      ${ARG_PREFIX}_ANDROID
      ${ARG_PREFIX}_APPLE
      ${ARG_PREFIX}_IOS
      ${ARG_PREFIX}_LINUX
      ${ARG_PREFIX}_MAC
      ${ARG_PREFIX}_POSIX
      ${ARG_PREFIX}_WIN
      ${ARG_PREFIX}_WIN32
      ${ARG_PREFIX}_WINRT
    )
  endforeach()

  cmake_parse_arguments(ARG "" "" "${ARG_KEYWORDS}" ${ARGN})

  foreach(ARG_PREFIX ${ARG_PREFIXES})
    set(RESULT ${ARG_${ARG_PREFIX}})
    if (${MSO_LIBLET_PLATFORM} STREQUAL ANDROID)
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_ANDROID})
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_POSIX})
    elseif(${MSO_LIBLET_PLATFORM} STREQUAL IOS)
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_APPLE})
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_IOS})
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_POSIX})
    elseif(${MSO_LIBLET_PLATFORM} STREQUAL LINUX)
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_LINUX})
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_POSIX})
    elseif(${MSO_LIBLET_PLATFORM} STREQUAL MAC)
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_APPLE})
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_MAC})
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_POSIX})
    elseif(${MSO_LIBLET_PLATFORM} STREQUAL WIN32)
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_WIN})
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_WIN32})
    elseif(${MSO_LIBLET_PLATFORM} STREQUAL WINRT)
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_WIN})
      list(APPEND RESULT ${ARG_${ARG_PREFIX}_WINRT})
    else()
      message(FATAL_ERROR "Unknown MSO_LIBLET_PLATFORM: \"${MSO_LIBLET_PLATFORM}\"")
    endif()

    # Set result as a variable in caller's scope.
    set(${RESULT_PREFIX}_${ARG_PREFIX} ${RESULT} PARENT_SCOPE)
  endforeach()
endfunction()

function(_liblet_set_platform_definitions TARGET)
  # Set platform definitions used by code
  if (${MSO_LIBLET_PLATFORM} STREQUAL ANDROID)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_ANDROID)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_POSIX)
  elseif(${MSO_LIBLET_PLATFORM} STREQUAL IOS)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_APPLE)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_IOS)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_POSIX)
  elseif(${MSO_LIBLET_PLATFORM} STREQUAL LINUX)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_LINUX)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_POSIX)
  elseif(${MSO_LIBLET_PLATFORM} STREQUAL MAC)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_APPLE)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_MAC)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_POSIX)
  elseif(${MSO_LIBLET_PLATFORM} STREQUAL WIN32)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_WINDOWS)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_WIN32)
  elseif(${MSO_LIBLET_PLATFORM} STREQUAL WINRT)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_WINDOWS)
    target_compile_definitions(${TARGET} PRIVATE MS_TARGET_WINRT)
  else()
    message(FATAL_ERROR "Unknown MSO_LIBLET_PLATFORM: \"${MSO_LIBLET_PLATFORM}\"")
  endif()

  if (CMAKE_BUILD_TYPE STREQUAL Debug)
    target_compile_definitions(${TARGET} PRIVATE DEBUG _DEBUG)
  endif()

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${TARGET}
      PRIVATE
        -fms-extensions
        -fms-compatibility-version=19.00
        -Wno-ignored-attributes
        -Wno-nonportable-include-path
    )
  endif()

  if (CMAKE_CXX_STANDARD EQUAL 17)
    target_compile_definitions(${TARGET} PRIVATE _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
  endif()
endfunction()

function(_liblet_verify_includes LIBLET_TARGET LIBLET_INCLUDES)
  # Declare LIBLET_VERIFY_INCLUDES_TARGET be dependent on the LIBLET_TARGET
  set(LIBLET_VERIFY_INCLUDES_TARGET ${LIBLET_TARGET}_verify_includes)
  if(NOT TARGET ${LIBLET_VERIFY_INCLUDES_TARGET})
    add_library(${LIBLET_VERIFY_INCLUDES_TARGET} OBJECT)
    target_link_libraries(${LIBLET_VERIFY_INCLUDES_TARGET} PUBLIC ${LIBLET_TARGET})
    _liblet_set_platform_definitions(${LIBLET_VERIFY_INCLUDES_TARGET})

    # Add a custom target that is added to default build target using ALL.
    # It is to ensure that LIBLET_VERIFY_INCLUDES_TARGET is always compiled.
    add_custom_target(${LIBLET_VERIFY_INCLUDES_TARGET}_custom ALL DEPENDS ${LIBLET_VERIFY_INCLUDES_TARGET})
  endif()

  # Create a directory for generated files
  set(LIBLET_VERIFY_INCLUDES_DIR "${CMAKE_CURRENT_BINARY_DIR}/verify")
  file(MAKE_DIRECTORY "${LIBLET_VERIFY_INCLUDES_DIR}")

  foreach(LIBLET_INCLUDE ${LIBLET_INCLUDES})
    get_filename_component(LIBLET_INCLUDE_NAME ${LIBLET_INCLUDE} NAME)
    set(LIBLET_INCLUDE_FILE "${LIBLET_VERIFY_INCLUDES_DIR}/${LIBLET_INCLUDE_NAME}.cpp")

    # Generate source file to verify header and add it to the target
    # The template file verify_header.cpp.in uses the @LIBLET_INCLUDE@ substitution
    configure_file(${LIBLET_CMAKE_DIR}/verify_include.cpp.in ${LIBLET_INCLUDE_FILE} @ONLY)
    target_sources(${LIBLET_VERIFY_INCLUDES_TARGET} PRIVATE ${LIBLET_INCLUDE_FILE})
  endforeach()
endfunction()

# Write a function to detect cycles
function(_liblet_find_all_targets OUT_TARGET_LIST)
  set(DIR_LIST "${CMAKE_CURRENT_LIST_DIR}")
  set(TARGETS "")

  while(DIR_LIST)
    # Pop the first directory from the queue
    list(GET DIR_LIST 0 CURRENT_DIR)
    list(REMOVE_AT DIR_LIST 0)

    # Read the current directory targets
    get_property(CURRENT_DIR_TARGETS DIRECTORY "${CURRENT_DIR}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(CURRENT_DIR_TARGET ${CURRENT_DIR_TARGETS})
      get_property(IS_IMPORTED TARGET ${CURRENT_DIR_TARGET} PROPERTY IMPORTED)
      if(NOT IS_IMPORTED)
        list(APPEND TARGETS "${CURRENT_DIR_TARGETS}")
      endif()
    endforeach()

    # Add the subdirectories into the queue
    get_property(CURRENT_SUBDIRS DIRECTORY "${CURRENT_DIR}" PROPERTY SUBDIRECTORIES)
    if(CURRENT_SUBDIRS)
      list(APPEND DIR_LIST "${CURRENT_SUBDIRS}")
    endif()
  endwhile()

  # return the targets in the variable named ${outname}
  set(${OUT_TARGET_LIST} "${TARGETS}" PARENT_SCOPE)
endfunction()

function(_liblet_detect_dependency_cycles_internal TARGET STACK)
  get_property(ALIASED_TARGET TARGET ${TARGET} PROPERTY ALIASED_TARGET)
  if(ALIASED_TARGET)
    get_property(IS_VISITED TARGET ${ALIASED_TARGET} PROPERTY LIBLET_CYCLE_VISITED)
  else()
    get_property(IS_VISITED TARGET ${TARGET} PROPERTY LIBLET_CYCLE_VISITED)
  endif()
  if(IS_VISITED)
    return()
  endif()

  get_property(DEPENDENTS TARGET ${TARGET} PROPERTY LINK_LIBRARIES)

  foreach(DEPENDENT IN LISTS DEPENDENTS)
    if (TARGET DEPENDENT)
      get_property(IS_IMPORTED TARGET ${DEPENDENT} PROPERTY IMPORTED)
      if(NOT IS_IMPORTED)
        # If a dependency is in the stack of visited nodes, we found a cycle
        if(DEPENDENT IN_LIST STACK)
          message(FATAL_ERROR "Dependency cycle detected: ${STACK};${DEPENDENT}")
        else()
          # No cycle so far, we need to go deeper!
          _liblet_detect_dependency_cycles_internal(${DEPENDENT} "${STACK};${DEPENDENT}")
        endif()
      endif()
    endif()
  endforeach()

  # Mark the node visited
  get_property(ALIASED_TARGET TARGET ${TARGET} PROPERTY ALIASED_TARGET)
  if (ALIASED_TARGET)
    set_property(TARGET ${ALIASED_TARGET} PROPERTY LIBLET_CYCLE_VISITED TRUE)
  else()
    set_property(TARGET ${TARGET} PROPERTY LIBLET_CYCLE_VISITED TRUE)
  endif()
endfunction()

function(liblet_detect_dependency_cycles)
  _liblet_find_all_targets(MY_ALL_TARGETS)
  message("All targets: ${MY_ALL_TARGETS}")

  foreach(MY_TARGET IN LISTS MY_ALL_TARGETS)
    if(MY_TARGET)
      _liblet_detect_dependency_cycles_internal("${MY_TARGET}" "${MY_TARGET}")
    endif()
  endforeach()
endfunction()
