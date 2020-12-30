-- @Author: Your name
-- @Date:   2020-12-26 15:11:08
-- @Last Modified by:   Your name
-- @Last Modified time: 2020-12-30 14:48:56
--  @file   premake5.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc components library premake script define.

-- #########################################################################
require "premake_com"

-- workspace define
workspace ("llbc_comps_" .. _ACTION)
    -- location define
    location ("../../build/" .. _ACTION)

    -- configurations define
    configurations { "release32", "debug32", "release64", "debug64" }

    -- architectures define
    filter { "configurations:*32" }
        architecture "x86"
    filter {}
    filter { "configurations:*64" }
        architecture "x86_64"
    filter {}

    -- some common used macros define
    filter { "configurations:debug*" }
        defines {
            "DEBUG"
        }
    filter {}

    -- enable symbols gen
    symbols "On"

    -- set characterset to MBCS
    filter { "language:c++" }
        characterset "MBCS"
    filter {}

    -- enable cpp11/14/17/latest support
    enable_cpp11_support()
    -- enable_cpp14_support()
    -- enable_cpp17_support()

    -- disable some warnings
    disable_unimportant_warnings()

-- ****************************************************************************
-- all component projects define.

-- component: example
project "example"
    generate_comp_project("example")

project "database"
    generate_comp_project("database")
    include_3rd_mysql("database")    
