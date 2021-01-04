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

sint64 __Str2Int64(const char *str)
{
#ifdef WIN32
    return ::_atoi64(str);
#else
    return ::atoll(str);
#endif
}

template <typename DataType>
bool FieldTemplate<DataType>::SetValue(const char *value, uint32 len)
{
    switch (_info.type)
    {
    case FIELD_TYPE_TINY:
    case FIELD_TYPE_SHORT:
    case FIELD_TYPE_LONG:
    case FIELD_TYPE_LONGLONG:
    case MYSQL_TYPE_NEWDECIMAL:
    case FIELD_TYPE_TIMESTAMP:
        _val = value ? atoll(value) : 0;
        break;
    case FIELD_TYPE_DOUBLE:
    case FIELD_TYPE_FLOAT:
        _val = value ? atof(value) : 0;
    default:
        return false;
    }

    return true;
}

template <typename DataType>
sint64 FieldTemplate<DataType>::GetInt() const
{
    return _val;
}

template <typename DataType>
double FieldTemplate<DataType>::GetDouble() const
{
    return _val;
}

template <typename DataType>
LLBC_String &FieldTemplate<DataType>::GetStr() const
{
    assert(false);
    static LLBC_String empty;
    return empty;
}

template <typename DataType>
void FieldTemplate<DataType>::SetInt(sint64 val)
{
    _val = val;
    TagChanged(true);
}

template <typename DataType>
void FieldTemplate<DataType>::SetDouble(double val)
{
    _val = val;
    TagChanged(true);
}

template <typename DataType>
void FieldTemplate<DataType>::SetStr(const char *val)
{
    assert(false);
}

template <typename DataType>
void FieldTemplate<DataType>::SetBlob(const char *val, uint32 len)
{
    assert(false);
}


bool FieldTemplate<LLBC_String>::SetValue(const char *value, uint32 len)
{
    switch (_info.type)
    {
    case FIELD_TYPE_STRING:
    case FIELD_TYPE_VAR_STRING:
    case FIELD_TYPE_TINY_BLOB:
    case FIELD_TYPE_BLOB:
    case FIELD_TYPE_MEDIUM_BLOB:
    case FIELD_TYPE_LONG_BLOB:
    case FIELD_TYPE_DATE:
    case FIELD_TYPE_DATETIME:
        _val = value ? LLBC_String(value, len) : "";
        break;
    default:
        return false;
    }

    return true;
}

sint64 FieldTemplate<LLBC_String>::GetInt() const
{
    assert(false);
    return 0;
}

double FieldTemplate<LLBC_String>::GetDouble() const
{
    assert(false);
    return 0;
}

const LLBC_String &FieldTemplate<LLBC_String>::GetStr() const
{
    return _val;
}

void FieldTemplate<LLBC_String>::SetInt(sint64 val)
{
    assert(false);
}

void FieldTemplate<LLBC_String>::SetDouble(double val)
{
    assert(false);
}

void FieldTemplate<LLBC_String>::SetStr(const char *val)
{
    _val = val;
    TagChanged(true);
}

void FieldTemplate<LLBC_String>::SetBlob(const char *val, uint32 len)
{
    _val.swap(LLBC_String(val, len));
    TagChanged(true);
}