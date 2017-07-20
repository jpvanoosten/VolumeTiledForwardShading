#pragma once

/*
 *  Copyright(c) 2015 Jeremiah van Oosten
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files(the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions :
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

/**
 *  @file EngineDefines.h
 *  @date December 20, 2015
 *  @author Jeremiah
 *
 *  @brief Engine defines. Define ENGINE_IMPORTS in your own project to import DLL symbols.
 */

#define _KB(x) (x * 1024)
#define _MB(x) (x * 1024 * 1024)

#define _64KB _KB(64)
#define _1MB _MB(1)
#define _2MB _MB(2)
#define _4MB _MB(4)
#define _8MB _MB(8)
#define _16MB _MB(16)
#define _32MB _MB(32)
#define _64MB _MB(64)
#define _128MB _MB(128)
#define _256MB _MB(256)

#ifdef ENGINE_EXPORTS
#   define ENGINE_DLL __declspec(dllexport)
#   define ENGINE_EXTERN
#elif ENGINE_IMPORTS
#   define ENGINE_DLL __declspec(dllimport)
#   define ENGINE_EXTERN extern
#else
#   define ENGINE_DLL
#   define ENGINE_EXTERN
#endif

 // Stringize macro for numerical macro types.
 // Inserts a macro as a string.
 // Used by the STRINGIZE macro and should not be used directly.
#define STRINGIZE2(x) #x
 // Expand the macro into a string.
#define STRINGIZE(x) STRINGIZE2(x)

// Expand a macro to a wide character string.
#define _W2(x) L ## x
#define _W(x) _W2(x)
