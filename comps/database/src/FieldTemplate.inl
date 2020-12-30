
int64_t __Str2Int64(const char *str)
{
#ifdef WIN32
    return ::_atoi64(str);
#else
    return ::atoll(str);
#endif
}

template <typename DataType>
bool FieldTemplate<DataType>::SetValue(const char *value, uint32_t len)
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
int64_t FieldTemplate<DataType>::GetInt() const
{
    return _val;
}

template <typename DataType>
double FieldTemplate<DataType>::GetDouble() const
{
    return _val;
}

template <typename DataType>
std::string &FieldTemplate<DataType>::GetStr() const
{
    assert(false);
    static std::string empty;
    return empty;
}

template <typename DataType>
void FieldTemplate<DataType>::SetInt(int64_t val)
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
void FieldTemplate<DataType>::SetBlob(const char *val, uint32_t len)
{
    assert(false);
}


bool FieldTemplate<std::string>::SetValue(const char *value, uint32_t len)
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
        _val = value ? std::string(value, len) : "";
        break;
    default:
        return false;
    }

    return true;
}

int64_t FieldTemplate<std::string>::GetInt() const
{
    assert(false);
    return 0;
}

double FieldTemplate<std::string>::GetDouble() const
{
    assert(false);
    return 0;
}

const std::string &FieldTemplate<std::string>::GetStr() const
{
    return _val;
}

void FieldTemplate<std::string>::SetInt(int64_t val)
{
    assert(false);
}

void FieldTemplate<std::string>::SetDouble(double val)
{
    assert(false);
}

void FieldTemplate<std::string>::SetStr(const char *val)
{
    _val = val;
    TagChanged(true);
}

void FieldTemplate<std::string>::SetBlob(const char *val, uint32_t len)
{
    _val.swap(std::string(val, len));
    TagChanged(true);
}