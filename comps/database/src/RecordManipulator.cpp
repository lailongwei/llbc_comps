#include "RecordManipulator.h"
#include "SqlUtil.h"
#include "Database.h"

IRecord *RecordManipulator::MakeDefRecord(IDatabase *db, const char *tableName)
{
    return ((MysqlDB *) db)->MakeDefRecord(tableName);
}

bool RecordManipulator::Insert(IDatabase *db, IRecord *record)
{
    return ((MysqlDB *) db)->Insert(record);
}

bool RecordManipulator::Update(IDatabase *db, IRecord *record)
{
    return ((MysqlDB *) db)->Update(record);
}

bool RecordManipulator::Delete(IDatabase *db, IRecord *record)
{
    return ((MysqlDB *) db)->Delete(record);
}
