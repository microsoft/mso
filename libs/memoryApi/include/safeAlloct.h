// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#ifdef __cplusplus
#include <climits>
#include <cstddef>
#include <sal.h>
//#include <intsafe.h>
/*
 * MsoCbBufSizeT
 * This function returns the bytes needed to allocate a buffer for (cElements + cExtra)
 * instances of type T. This is a low level function, and typical usage would be to call
 * one of the wrappers that deal with (for example) WCHAR arrays.
 *
 * Note:  Strictly speaking, this isn't an exported MSO function, as it's instantiated by
 *        the caller, and simply picked up from the header.
 *
 * Arguments:
 * cElements - the number of elements needed
 * cExtra - any additional elements needed
 * cbMaxBufSize - optional argument for the maximum bytes allowed. Default is the maximum
 *            possible value for the architecture.
 *
 * Return:
 * Either a mathematically correct number of bytes required, or in the case of an error,
 * INVALID_BUFFER_SIZE
 *
 * Revision history:
 * 10/22/04 - dleblanc - created
 */

const size_t INVALID_BUFFER_SIZE = ((size_t)~0);
const size_t MAXIMUM_BUFFER_SIZE = ((size_t)INT_MAX);

template <typename T>
_Ret_range_(==, (cElements + cExtra) * sizeof(T)) size_t
    MsoCbBufSizeT(size_t cElements, size_t cExtra = 0, size_t cbMaxBufSize = MAXIMUM_BUFFER_SIZE) noexcept
{
  // first calculate maximum allowed
  // typical case is a compile time constant
  const size_t cMax = cbMaxBufSize / sizeof(T);
  const size_t cMaxAllowed = cMax - cExtra;

  if (cExtra > cMax || // cExtra is too large
      cElements > cMaxAllowed) // real check here
  {
#if DEBUG
    // TODO: This should be changed to a tagged function
    //__asm int 3
#endif
    // we have a problem
    return INVALID_BUFFER_SIZE;
  }

  return (cElements + cExtra) * sizeof(T);
}

/*
 * MsoCbBufSizeExT
 *
 * Variant of MsoCbBufSizeT in which you may specify an additional "cbExtra" size
 * (for array headers, etc.)
 *
 * Arguments:
 * cElements - the number of elements needed
 * cbExtra - any additional bytes needed
 * cExtra - any additional elements needed
 * cbMaxBufSize - optional argument for the maximum bytes allowed. Default is the maximum
 *            possible value for the architecture.
 *
 * Return:
 * Either a mathematically correct number of bytes required, or in the case of an error,
 * INVALID_BUFFER_SIZE
 *
 * Revision history:
 * 8/14/05 - LeeHu - created
 */

template <typename T>
_Ret_range_(==, (cElements + cExtra) * sizeof(T) + cbExtra) size_t MsoCbBufSizeExT(
    size_t cElements,
    size_t cbExtra,
    size_t cExtra = 0,
    size_t cbMaxBufSize = MAXIMUM_BUFFER_SIZE) noexcept
{
  // first calculate maximum allowed
  const size_t cbMax = cbMaxBufSize - cbExtra;
  const size_t cMax = cbMax / sizeof(T);
  const size_t cMaxAllowed = cMax - cExtra;

  if (cbExtra > cbMaxBufSize || // cbExtra is too large
      cExtra > cMax || // cExtra is too large
      cElements > cMaxAllowed //"real" buf size check here
  )
  {
#if DEBUG
    // TODO: This should be changed to a tagged function
    //__asm int 3
#endif
    // we have a problem
    return INVALID_BUFFER_SIZE;
  }

  return (cElements + cExtra) * sizeof(T) + cbExtra;
}

#endif // __cplusplus
