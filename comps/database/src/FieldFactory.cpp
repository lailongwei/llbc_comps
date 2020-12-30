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
