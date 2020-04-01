// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

/**
	Some globally useful debug-only macros
*/
#pragma once
#ifndef COMPILERADAPTERS_CPPMACROSDEBUG_H
#define COMPILERADAPTERS_CPPMACROSDEBUG_H

#include <cstdint>

/**
   Helper macros for passing file, line in DEBUG builds
*/
#if DEBUG
#define FILELINEPARAMSCOREUNUSED const char*, uint32_t
#define FILELINEPARAMSUNUSED , FILELINEPARAMSCOREUNUSED
#define FILELINEPARAMSOPTCORE const char* szFile = __FILE__, uint32_t iLine = __LINE__
#define FILELINEPARAMSOPT , FILELINEPARAMSOPTCORE
#define FILELINEPARAMSOPTUNUSEDCORE const char* = __FILE__, uint32_t = __LINE__
#define FILELINEPARAMSOPTUNUSED , FILELINEPARAMSOPTUNUSEDCORE
#define FILELINEPARAMSCORE const char* szFile, uint32_t iLine
#define FILELINEPARAMS , FILELINEPARAMSCORE
#define FILELINEVARSCORE szFile, iLine
#define FILELINEVARS , FILELINEVARSCORE
#define FILELINECORE __FILE__, __LINE__
#define FILELINE , FILELINECORE
#define __FILE_SHIP__ __FILE__
#define __LINE_SHIP__ __LINE__
#else
#define FILELINEPARAMSCOREUNUSED
#define FILELINEPARAMSUNUSED
#define FILELINEPARAMSOPTCORE
#define FILELINEPARAMSOPT
#define FILELINEPARAMSOPTUNUSEDCORE
#define FILELINEPARAMSOPTUNUSED
#define FILELINEPARAMSCORE
#define FILELINEPARAMS
#define FILELINEVARSCORE
#define FILELINEVARS
#define FILELINECORE
#define FILELINE
#define __FILE_SHIP__ NULL
#define __LINE_SHIP__ 0
#endif // DEBUG

/**
   Helper macros for behaving differently in debug vs. ship
*/
#if DEBUG
#define Debug(e) e
#define DebugOnly(e) e
#define DebugElse(s, t)	s
#define DebugList(e) (e),
#define DebugParam(e) , e
#define DebugFill(pb, cb) (memset((pb), 0xCC, (cb)))
#else
#define Debug(e)
#define DebugOnly(e)
#define DebugElse(s, t) t
#define DebugList(e) 
#define DebugParam(e)
#define DebugFill(pb, cb) (1)
#endif

#if defined(__GNUC__)
#ifdef DEBUG
#include <signal.h>
#define __debugbreak() raise(SIGTRAP)
#else // !DEBUG
#define __debugbreak()
#endif // DEBUG
#endif // __GNUC__

/**
   MsoDebugBreak can be used inside expressions.
	Otherwise code should use __debugbreak() directly.
*/
#ifdef DEBUG
__inline int32_t MsoDebugBreak(void) 
#ifdef __cplusplus
noexcept
#endif // __cplusplus
	{ __debugbreak(); return 0; }
#else
#define MsoDebugBreak() (0)
#endif

#endif
