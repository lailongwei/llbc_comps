#pragma once

#include "comp_com/Common.h"

class IRecord;
class IDatabase;

class LLBC_COMP_EXPORT RecordManipulator
{
public:
    // sync create a default record.
    static IRecord *MakeDefRecord(IDatabase* db, const char *tableName);

    // sync insert record.
    static bool Insert(IDatabase *db, IRecord *record);

    // sync update record.
    static bool Update(IDatabase *db, IRecord *record);

    // sync delete record.
    static bool Delete(IDatabase *db, IRecord *record);
};
