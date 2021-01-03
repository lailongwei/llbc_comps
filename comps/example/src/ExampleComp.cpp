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

#include "comp_com/Export.h"
#include "ExampleComp.h"

bool ExampleComp::OnInitialize()
{
    AddMethod("Foo", &ExampleComp::Call_Foo);
    return true;
}

void ExampleComp::Foo(const LLBC_String &arg)
{
    std::cout <<"ExampleComp::Foo(const LLBC_String &) called, arg: " <<arg <<std::endl;
}

int ExampleComp::Call_Foo(const LLBC_Variant &arg, LLBC_Variant &ret)
{
    Foo(arg);
    ret = arg;
}

LLBC_IFacade *ExampleCompFactory::Create() const
{
    return LLBC_New(ExampleComp);
}

void *llbc_create_facade_ExampleComp()
{
    return ExampleCompFactory().Create();
}
