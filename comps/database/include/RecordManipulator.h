#pragma once

class IRecord;
class IDatabase;

class RecordManipulator
{
public:
    // async create a default record.
    static IRecord *MakeDefRecord(IDatabase* db, const char *tableName);

    // async insert record.
    static bool Insert(IDatabase *db, IRecord *record);

    // async update record.
    static bool Update(IDatabase *db, IRecord *record);

    // async delete record.
    static bool Delete(IDatabase *db, IRecord *record);
};
