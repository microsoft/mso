# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads QUIET)
  if(Threads_FOUND)
    set_target_properties(Threads::Threads PROPERTIES IMPORTED_GLOBAL TRUE)
  endif()
endif()
