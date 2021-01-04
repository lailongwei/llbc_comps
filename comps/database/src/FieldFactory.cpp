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

#include "FieldFactory.h"
#include "FieldTemplate.h"

BaseField *FieldFactory::CreateField(const DBFieldInfo &info)
{
    BaseField *p = nullptr;

    if ((info.attr & UNSIGNED_FLAG) == 0)
    {
        switch (info.type)
        {
        case FIELD_TYPE_TINY:
            p = new TinyIntField(info);
            break;
        case FIELD_TYPE_SHORT:
            p = new SmallIntField(info);
            break;
        case FIELD_TYPE_LONG:
            p = new IntField(info);
            break;
        case FIELD_TYPE_LONGLONG:
        case FIELD_TYPE_TIMESTAMP:
            p = new Int64Field(info);
            break;

        default:
            break;
        }
    }
    else
    {
        switch (info.type)
        {
        case FIELD_TYPE_TINY:
            p = new UTinyIntField(info);
            break;
        case FIELD_TYPE_SHORT:
            p = new USmallIntField(info);
            break;
        case FIELD_TYPE_LONG:
            p = new UIntField(info);
            break;
        case FIELD_TYPE_LONGLONG:
        case FIELD_TYPE_TIMESTAMP:
            p = new UInt64Field(info);
            break;
        default:
            break;
        }
    }

    if (p == nullptr)
    {
        switch (info.type)
        {
        case FIELD_TYPE_FLOAT:
        case FIELD_TYPE_DOUBLE:
        case MYSQL_TYPE_NEWDECIMAL:
            p = new DoubleField(info);
            break;
        case FIELD_TYPE_STRING:
        case FIELD_TYPE_VAR_STRING:
        case FIELD_TYPE_DATE:
        case FIELD_TYPE_DATETIME:
            p = new StringField(info);
            return p;
            break;
        case FIELD_TYPE_TINY_BLOB:
        case FIELD_TYPE_BLOB:
        case FIELD_TYPE_MEDIUM_BLOB:
        case FIELD_TYPE_LONG_BLOB:
            p = new BlobField(info);
            break;
        default:

            break;
        }
    }

    return p;
}
