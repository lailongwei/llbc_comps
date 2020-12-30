#ifndef _SQL_UTIL_H_
#define _SQL_UTIL_H_

#include <string>
#include <mysql.h>

class Record;
class BaseField;

class SqlUtil
{
public:
    static bool BuildUpdateSql(MYSQL *handle, Record *record, std::string &sql);
    static bool BuildDelSql(MYSQL *handle, Record *record, std::string &sql);
    static bool BuildInsertSql(MYSQL *handle, Record *record, std::string &sql);

private:
    static bool BuildCondition(MYSQL *handle, Record *record, std::string &cond);
    static bool BuildOperation(MYSQL *handle, Record *record, std::string &cond);
    static void BuildFormatSql(MYSQL *handle, const BaseField *field, char *format, int formatSize);
};
#endif
