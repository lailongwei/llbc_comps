// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef __LLBC_COMP_COM_MACRO_H__
#define __LLBC_COMP_COM_MACRO_H__

#include "llbc.h"

// The dllexport macro define.
#if LLBC_TARGET_PLATFORM_WIN32
 #ifndef LLBC_COMP_HIDDEN
  #define LLBC_COMP_HIDDEN
 #endif // LLBC_COMP_HIDDEN

#ifndef LLBC_COMP_EXPORT
  #ifdef __LLBC_COMP_EXPORT
   #define LLBC_COMP_EXPORTING 0
   #define LLBC_COMP_EXPORT __declspec(dllexport)
  #else
   #define LLBC_COMP_EXPORTING 1
   #define LLBC_COMP_EXPORT __declspec(dllimport)
   #endif
 #endif // LLBC_COMP_EXPORT
#else
 #ifndef LLBC_COMP_EXPORT
  #ifdef __LLBC_COMP_EXPORT
   #define LLBC_COMP_EXPORTING 1
   #define LLBC_COMP_EXPORT __attribute__((__visibility__("default")))
   #define LLBC_COMP_HIDDEN __attribute__((__visibility__("hidden")))
  #else
   #define LLBC_COMP_HIDDEN
   #endif
 #endif // LLBC_COMP_EXPORT
#endif

#endif  // !__LLBC_COMP_COM_MACRO_H__
