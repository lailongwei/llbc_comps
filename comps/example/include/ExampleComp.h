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

#pragma once

#include "comp_com/Common.h"

/**
 * \brief The example component class define.
 */
class LLBC_COMP_EXPORT ExampleComp : public LLBC_IFacade
{
public:
    virtual bool OnInitialize();

public:
    void Foo(const LLBC_String &arg);

public:
    int Call_Foo(const LLBC_Variant &arg, LLBC_Variant &ret);
};

/**
 * \brief The example component factory class define.
 */
class LLBC_COMP_EXPORT ExampleCompFactory : public LLBC_IFacadeFactory
{
public:
    virtual LLBC_IFacade *Create() const;
};

/**
 * \brief The facade dynamic create function define.
 */
LLBC_EXTERN LLBC_COMP_EXPORT void *llbc_create_facade_ExampleComp();
