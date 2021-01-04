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


#include "GameServer.h"

#include "llbc/comm/IService.h"
#include "comps/database/include/DBMgrFactory.h"

#include "Common/common.h"
#include "Modules/Logic/Logic.h"

GameApplication::GameApplication()
    : _gameSvc(nullptr)
{}

int GameApplication::OnStart(int argc, char *argv[])
{
    //create game service
    _gameSvc = LLBC_IService::Create(LLBC_IService::Normal, _name);

    // register component.
    _gameSvc->RegisterFacade<DBMgrFactory>();
    _gameSvc->RegisterFacade<LogicCompFactory>();

    _gameSvc->SetFPS(100);
    if (_gameSvc->Start(4) != LLBC_OK)
        return LLBC_FAILED;

    return LLBC_OK;
}

void GameApplication::OnWait() {}

void GameApplication::OnStop() {}

int main(int argc, char *argv[])
{
    g_GameServer.Start("GameServer", argc, argv);

    g_GameServer.Stop();
    return true;
}