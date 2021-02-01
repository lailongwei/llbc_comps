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

#include "modules/logic/Logic.h"
#include "app_common/Common.h"

#include "comps/database/include/DBMgrFactory.h"
#include "comps/database/include/IDBMgr.h"
#include "comps/database/include/RecordManipulator.h"

bool LogicComp::OnInitialize()
{
    return true;
}

void LogicComp::OnDestroy() {}

bool LogicComp::OnStart()
{
    DatabaseTest();
    return true;
}

void LogicComp::OnStop() {}

void LogicComp::OnUpdate() {}


void LogicComp::DatabaseTest()
{
    auto dbMgr = GetService()->GetComponent<IDBMgr>(ComponentName::DBMgr);
    auto defDB = dbMgr->GetDefaultDatabase();

    const char *createTableSql =
        u8"create table `llbc` ("
        "`id` bigint(20) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'Id',"
        "`varstr_type` varchar(255) CHARACTER SET utf8mb4 NOT NULL DEFAULT '',"
        "`str_type` char(32) CHARACTER SET utf8mb4 NOT NULL DEFAULT '',"
        "`tiny_type` tinyint(2) NOT NULL DEFAULT 0,"
        "`blob_type` blob NOT NULL,"
        "`int_type` int(20) NOT NULL DEFAULT 0,"
        "`float_type` float NOT NULL DEFAULT 0,"
        "`double_type` double NOT NULL DEFAULT 0,"
        "`datetime_type` datetime DEFAULT CURRENT_TIMESTAMP,"
        "PRIMARY KEY(`id`)"
        ") ENGINE = InnoDB;";

    if (!defDB)
        return;

    ASSERT(defDB->Query("drop table if exists `llbc`"));
    ASSERT(defDB->Query(createTableSql));

    struct DBBlobData
    {
        int a = 1;
        int64_t b = 2;
        int c = 3;
        char d = '*';
    };

    DBBlobData _t;
    const LLBC_String blob((char *) &_t, sizeof(DBBlobData));
    // const LLBC_String blob("this blob !!!");
    const LLBC_String varstr("this is varstr_type!!!");
    const LLBC_String str("this is str_type!!!");
    const float ft = 3.1415926;
    const double dt = 3.1415926;
    const LLBC_String datet("2020-01-01 12:00:00");

    for (int i = 0; i <= 100; ++i)
    {
        auto llbcRec = RecordManipulator::MakeDefRecord(defDB, "llbc");
        ASSERT(llbcRec);
        llbcRec->SetStr("varstr_type", varstr.c_str());
        llbcRec->SetStr("str_type", str.c_str());
        llbcRec->SetInt("tiny_type", 1);
        llbcRec->SetBlob("blob_type", blob.c_str(), blob.size());
        llbcRec->SetDouble("float_type", ft);
        llbcRec->SetDouble("double_type", dt);
        llbcRec->SetInt("int_type", i);
        llbcRec->SetStr("datetime_type", datet.c_str());
        ASSERT(RecordManipulator::Insert(defDB, llbcRec));
        delete llbcRec;
    }

    auto llbcRecSet = defDB->Query("select * from llbc limit 1", MODE::MODE_EDIT);
    auto &llbcRec = (*llbcRecSet)[0];
    auto &varstrType = llbcRec.GetStr("varstr_type");
    auto &strType = llbcRec.GetStr("str_type");
    auto tinyType = llbcRec.GetInt("tiny_type");
    auto &blobType = llbcRec.GetStr("blob_type");
    auto floatType = llbcRec.GetDouble("float_type");
    auto doubleType = llbcRec.GetDouble("double_type");
    auto intType = llbcRec.GetInt("int_type");
    auto &datetimeType = llbcRec.GetStr("datetime_type");

    DBBlobData *readBlob = (DBBlobData *) blobType.c_str();

    ASSERT(varstrType == varstr);
    ASSERT(strType == str);
    ASSERT(tinyType == 1);
    ASSERT(readBlob->b == _t.b);
    ASSERT(floatType - ft < 0.0001);
    ASSERT(doubleType - dt < 0.0001);
    ASSERT(datetimeType == datet);

    llbcRec.SetStr("varstr_type", "new this is varstr_type!!!");
    llbcRec.SetStr("str_type", "new this is str_type!!!");
    llbcRec.SetInt("tiny_type", 2);
    llbcRec.SetBlob("blob_type", blob.c_str(), blob.size());
    llbcRec.SetInt("float_type", ft);
    llbcRec.SetInt("double_type", dt);
    llbcRec.SetStr("datetime_type", datet.c_str());
    ASSERT(RecordManipulator::Update(defDB, &llbcRec));

    defDB->QueryAsync(1, "delete from llbc where id > 30", [](bool ret) { LLBC_PrintLine("delete result %d", ret); });

    defDB->QueryAsync(2, "select * from llbc", MODE::MODE_READ, [](IRecordset *recSet) {
        ASSERT(recSet);
        const auto count = recSet->GetSize();
        for (int i = 0; i < count; ++i)
        {
            auto &llbcRec = (*recSet)[i];
            auto id = llbcRec.GetInt("id");
            LLBC_PrintLine("mysql acync read id: %lld", id);
        }
        delete recSet;
    });
}

