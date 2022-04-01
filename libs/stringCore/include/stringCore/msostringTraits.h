/*-------------------------------------------------------------------------
  msoStringTraits.h

  Copyright (c) 1994 Microsoft Corporation

  Functions related to string and character characteristics.

  %%Owner: AskIntl
  %%Id: 96dc3da1-b7a4-424b-bdc2-7c21d7423405
-------------------------------------------------------------------------*/
#pragma once
#ifndef LIBLET_ROOT_MSOSTRINGTRAITS_H
#define LIBLET_ROOT_MSOSTRINGTRAITS_H
#include <compilerAdapters/functionDecorations.h>
#include <platformAdapters/windowsfirst.h>

//typedef struct IMsoMemHeap IMsoMemHeap;
//
//#define wchUnicodeBig 0xFFFE
//#define wchUnicodeLittle 0xFEFF
//#define lUTF8BOM 0xBFBBEFL
//#define lUTF8BOMMask 0xFFFFFFL
//
///*	Check whether if wch is an alphanumeric character. */
//LIBLET_PUBLICAPI BOOL MsoFAlphaNumWch(WCHAR wch) MSONOEXCEPT;
//LIBLET_PUBLICAPI int MsoFExtenderWch(WCHAR wch) MSONOEXCEPT;
//
///*	Check whether if ch/wch is a [hex]digit. */
///* MBSupport: LIBLET_PUBLICAPI*/ BOOL MsoFDigitCh(int ch) MSONOEXCEPT;
//LIBLET_PUBLICAPI BOOL MsoFDigitWch(WCHAR wch) MSONOEXCEPT;
//LIBLET_PUBLICAPI BOOL MsoFHexDigitWch(WCHAR wch) MSONOEXCEPT;
//LIBLET_PUBLICAPI int ValHexDigitWch(WCHAR wch) MSONOEXCEPT;
//
///*	Check whether if sz/wz contains all digits */
//LIBLET_PUBLICAPI BOOL MsoFSzAllDigitsSimple(const CHAR* sz) MSONOEXCEPT; // only cares about ANSI '0'-'9'
//LIBLET_PUBLICAPI BOOL MsoFWzAllDigits(const WCHAR* wz) MSONOEXCEPT;
//
//LIBLET_PUBLICAPI MSODLL_MANDATED_C_EXPORT BOOL MsoFPuncWch(WCHAR wch) MSONOEXCEPT;
//LIBLET_PUBLICAPI MSOEXTERN_C BOOL MsoFAlphaWch(WCHAR wch) MSONOEXCEPT;
//LIBLET_PUBLICAPI BOOL MsoFSpecChWch(WCHAR wch) MSONOEXCEPT;
//LIBLET_PUBLICAPI BOOL MsoFComplexMarkWch(WCHAR wch) MSONOEXCEPT;
//LIBLET_PUBLICAPI MSODLL_MANDATED_C_EXPORT BOOL MsoFSpaceWch(WCHAR wch) MSONOEXCEPT;
//LIBLET_PUBLICAPI MSOEXTERN_C BOOL MsoFSpaceCh(CHAR ch) MSONOEXCEPT;
//
//LIBLET_PUBLICAPI BOOL MsoFLowAsciiRgch(LPCSTR sz, int cch) MSONOEXCEPT;
//LIBLET_PUBLICAPI BOOL MsoFLowAsciiRgwch(LPCWSTR wz, int cch) MSONOEXCEPT;
//
//#ifdef DEBUG
//LIBLET_PUBLICAPI BOOL MsoFValidWtz(const WCHAR* wtz) MSONOEXCEPT;
//#else
//#define MsoFValidWtz(wtz) (1)
//#endif // DEBUG
//
///*	Returns fTrue if and only if wch is a FE char. */
//// C linkage required: Used by GetProcAddress
//LIBLET_PUBLICAPI MSOEXTERN_C BOOL MsoFFEWch(WCHAR wch) MSONOEXCEPT;
//
//LIBLET_PUBLICAPI BOOL MsoIsWchVietToneMark(WCHAR wch) MSONOEXCEPT;
///*	Returns fTrue if and only if wch is a Indic vowel. */
//LIBLET_PUBLICAPI BOOL MsoIsWchIndicVowel(WCHAR wch) MSONOEXCEPT;
//
///* PluggableUI: LIBLET_PUBLICAPI*/ int MsoIOFCTriggerFromXchXch(WCHAR xch1, WCHAR xch2) MSONOEXCEPT;
//
///*-----------------------------------------------------------------------------
//    %%owner:  DanJump
//    MetroFValidPartChar
//
//    Returns TRUE if the character is valid in a metro part name
//-----------------------------------------------------------------------------*/
//LIBLET_PUBLICAPI BOOL MetroFValidPartChar(const WCHAR wch) MSONOEXCEPT;

#define MsoFIsLowerFast(ch) ((unsigned)((ch) - 'a') <= 'z' - 'a')
#define ASCII_TOUPPER(x) (FBetween(x, 'a', 'z') ? (x) - ('a' - 'A') : (x))

#define UTF8_2_MAX 0x07ff // max UTF8 2-byte sequence (32 * 64 = 2048)
#define UTF8_1ST_OF_2 0xc0 // 110x xxxx
#define UTF8_1ST_OF_3 0xe0 // 1110 xxxx
#define UTF8_1ST_OF_4 0xf0 // 1111 0xxx
#define UTF8_TRAIL 0x80 // 10xx xxxx
#define UCS2_1ST_OF_2 0xd800 // 1101 10xx xxxx xxxx
#define UCS2_2ND_OF_2 0xdc00 // 1101 11xx xxxx xxxx

#define HIGH_6_MASK(u) ((u)&0xfc00)
#define MIDDLE_6_BIT(u) (((u)&0x0fc0) >> 6)
#define LOWER_6_BIT(u) ((u)&0x003f)
#define LOWER_5_BIT(u) ((u)&0x001f)
#define MIDDLE_4_BIT(u) (((u)&0x03c0) >> 6)
#define MIDLOW_4_BIT(u) (((u)&0x003c) >> 2)
#define LOWER_10_BIT(u) ((u)&0x03ff)
#define LOWER_4_BIT(u) ((u)&0x000f)
#define LOWER_2_BIT(u) ((u)&0x0003)
#define HIGH_5_MASK(a) ((a)&0xf8)
#define HIGH_4_MASK(a) ((a)&0xf0)
#define HIGH_3_MASK(a) ((a)&0xe0)
#define HIGH_2_MASK(a) ((a)&0xc0)
#define HIGH_1_MASK(a) ((a)&0x80)
#define BIT7(a) ((a)&0x80)

#ifdef __cplusplus
#include <type_traits>

namespace Mso::String {

template <typename T>
struct HexTraits
{
  static_assert(std::is_unsigned<T>::value);

  static constexpr const size_t CharCount = sizeof(T) * 2;
};

} // namespace Mso::String

#endif // C++

#endif // LIBLET_ROOT_MSOSTRINGTRAITS_H
