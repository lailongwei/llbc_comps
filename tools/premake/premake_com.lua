-- @Author: Your name
-- @Date:   2020-12-26 15:09:04
-- @Last Modified by:   Your name
-- @Last Modified time: 2020-12-30 15:02:14
--  @file   premake5_com.lua
--  @author Longwei Lai<lailongwei@126.com>
--  @brief  The llbc components library premake script common variables&functions define.
-- #########################################################################
-- Some global variables define.
IS_WINDOWS = string.match(_ACTION, 'vs') ~= nil
LLBC_COMPS_REPO_ROOT_DIR = path.getabsolute("../..") -- the llbc_comps root directory
COMPS_ROOT_DIR = LLBC_COMPS_REPO_ROOT_DIR .. "/comps" -- all comps root directory
THIRD_PARTIES_ROOT_DIR = LLBC_COMPS_REPO_ROOT_DIR .. "/3rds" -- 3rds root directory
THIRD_PARTY__LLBC_DIR = THIRD_PARTIES_ROOT_DIR .. "/llbc/llbc" -- llbc 3rd directory

-- Execute system command
function execute_cmd(cmd, raw)
    local f = assert(io.popen(cmd, 'r'))
    local s = assert(f:read('*a'))
    f:close()
    if raw then return s end
    s = string.gsub(s, '^%s+', '')
    s = string.gsub(s, '%s+$', '')
    s = string.gsub(s, '[\n\r]+', ' ')
    return s
end

-- PY target
PY=nil
if IS_WINDOWS then
    PY = "$(ProjectDir)../../tools/py.exe"
else
    local output = execute_cmd("python --version")
    if output:find("command not found") then
        error("python command not found")
    elseif output:find("Python 3") then
        if execute_cmd("which python2"):len() == 0 then
            error("Python3 is not yet supported, please install python2 and make sure python2 can be use")
        end
        PY = "python2"
    else
        PY = "python"
    end
end

-- ****************************************************************************
-- Common functional functions define

-- Find msbuild path
-- Thanks to 'vswhere project' for making it easy for everyone to find MSBuild
-- Link: https://github.com/microsoft/vswhere/wiki/Find-MSBuild
function find_msbuild_path()
	local vs_ver = string.sub(_ACTION, 3)
	local cmd = "cd /d \"%ProgramFiles(x86)%\\Microsoft Visual Studio\\Installer\" & "
    cmd = cmd .. "vswhere -all -requires Microsoft.Component.MSBuild -find MSBuild\\**\\Bin\\MSBuild.exe | findstr " .. vs_ver
    print(cmd)
    os.execute(cmd)
	local f = assert(io.popen(cmd))
	local msbuild_path = f:read("l")
    f:close()
    
	return msbuild_path
end

-- Enable multithread compile
function enable_multithread_comp()
    filter { "system:windows" }
        flags { "MultiProcessorCompile", "NoMinimalRebuild" }
    filter {}
end

-- Set optimize options.
function set_optimize_opts()
    filter { "configurations:debug*", "language:c++", "system:not windows" }
        buildoptions {
            "-ggdb -g",
        }
    filter {}

    filter { "configurations:debug*", "language:c++", "system:windows" }
        runtime "Debug"
        optimize "Debug"
    filter {}

    filter { "configurations:debug*", "language:not c++" }
        optimize "Debug"
    filter {}

    filter { "configurations:release*" }
        optimize "Speed"
    filter {}
end

-- enable cpp11 support
function enable_cpp11_support()
    filter { "system:not windows" }
        buildoptions {
            "-std=c++11",
        }
    filter {}
end

-- enable cpp14 support
function enable_cpp14_support()
	filter { "system:not windows" }
		buildoptions {
			"/std:c++14",
		}
	filter {}
    filter { "system:not windows" }
        buildoptions {
            "-std=c++14",
        }
    filter {}
end

function enable_cpp17_support()
	filter { "system:not windows" }
		buildoptions {
			"/std:c++17",
		}
	filter {}
    filter { "system:not windows" }
        buildoptions {
            "-std=c++17",
        }
    filter {}
end

-- disable some unimportant warnings
function disable_unimportant_warnings()
    filter { "system:windows", "language:c++" }
        disablewarnings { "4091" }
    filter {}
	
	filter { "system:not windows" }
        disablewarnings {
            "invalid-source-encoding",
        }
    filter {}
end

-- build llbc library
function build_llbc_library()
	prebuildmessage("Building llbc library...")
	local msbuild_path = nil
	if IS_WINDOWS then
        msbuild_path = find_msbuild_path()
	end
	
	-- windows platform build
	local llbc_dir=THIRD_PARTY__LLBC_DIR
	filter { "system:windows" }
		prebuildcommands(string.format("cd %s & echo %s | .\\WinPrebuild.bat 0", llbc_dir .. "/..", _ACTION))
	filter {}
	
	local sln_name=string.format("llbc_%s.sln", _ACTION)
	local msbuild_params_prefix = string.format("%s -target:llbc ", sln_name)
	filter { "system:windows", "configurations:debug32" }
		local win_dbg32_msbuild_params = msbuild_params_prefix .. "-property:Configuration=debug32 -property:Platform=x86"
		prebuildcommands(string.format("cd \"%s\\..\\build\\%s\" & \"%s\" %s", llbc_dir, _ACTION, msbuild_path, win_dbg32_msbuild_params))
	filter {}
	filter { "system:windows", "configurations:release32" }
		local win_rel32_msbuild_params = msbuild_params_prefix .. "-property:Configuration=release32 -property:Platform=x86"
		prebuildcommands(string.format("cd \"%s\\..\\build\\%s\" & \"%s\" %s", llbc_dir, _ACTION, msbuild_path, win_rel32_msbuild_params))
	filter {}
	filter { "system:windows", "configurations:debug64" }
		local win_dbg64_msbuild_params = msbuild_params_prefix .. "-property:Configuration=debug64 -property:Platform=x64"
		prebuildcommands(string.format("cd \"%s\\..\\build\\%s\" & \"%s\" %s", llbc_dir, _ACTION, msbuild_path, win_dbg64_msbuild_params))
	filter {}
	filter { "system:windows", "configurations:release64" }
		local win_rel64_msbuild_params = msbuild_params_prefix .. "-property:Configuration=release64 -property:Platform=x64"
		prebuildcommands(string.format("cd \"%s\\..\\build\\%s\" & \"%s\" %s", llbc_dir, _ACTION, msbuild_path, win_rel64_msbuild_params))
	filter {}
	
	-- non-windows platform build
	filter { "system:not windows", "configurations:debug32" }
		prebuildcommands(string.format("cd %s/.. && make core_lib config=debug32 -j8", llbc_dir))
	filter {}
	filter { "system:not windows", "configurations:release32" }
		prebuildcommands(string.format("cd %s/.. && make core_lib config=release32 -j8", llbc_dir))
	filter {}
	filter { "system:not windows", "configurations:debug64" }
		prebuildcommands(string.format("cd %s/.. && make core_lib config=debug64 -j8", llbc_dir))
	filter {}
	filter { "system:not windows", "configurations:release64" }
		prebuildcommands(string.format("cd %s/.. && make core_lib config=release64 -j8", llbc_dir))
	filter {}
end

-- generate component project
function generate_comp_project(comp_name)
    -- language, kind
    language "c++"
    kind "SharedLib"
	
	-- target dir
    local comp_dir = COMPS_ROOT_DIR .. "/" .. comp_name
    filter { "system:windows" }
        targetdir(comp_dir .. "/lib/" .. _ACTION .. "/$(Configuration)")
    filter {}
    filter { "system:not windows" }
        targetdir(comp_dir .. "/lib/" .. _ACTION .. "/$(config)")
    filter {}

	-- include llbc library
	include_3rd_llbc(comp_name, false)

    -- files
    files {
        comp_dir .. "/include/**.h",
		comp_dir .. "/include/**.hpp",
		comp_dir .. "/include/**.inl",
		
		comp_dir .. "/src/**.c",
		comp_dir .. "/src/**.cpp",
    }

    -- includedirs
    includedirs {
        comp_dir .. "/include"
    }

    -- debug target suffix define
    filter { "configurations:debug*" }
        targetsuffix "_debug"
    filter {}

    -- enable multithread compile
    enable_multithread_comp()

    -- optimize
    set_optimize_opts()
end

-- ****************************************************************************
-- all 3rds include functions define.

-- include 3rd:llbc
function include_3rd_llbc(comp_name, add_files)
	-- build llbc library first
	build_llbc_library()
	
	-- includedirs
	includedirs {
		THIRD_PARTY__LLBC_DIR .. "/include",
	}
	
	-- files
	if add_files then
		files {
			THIRD_PARTY__LLBC_DIR .. "/include/**.h",
			THIRD_PARTY__LLBC_DIR .. "/include/**.inl",
		}
	end

	-- set llbc libdirs
	local llbc_lib_root_dir = string.format("%s/../output/%s", THIRD_PARTY__LLBC_DIR, _ACTION)
	filter { "system:windows" }
		libdirs { llbc_lib_root_dir .. "/$(Configuration)" }
	filter {}
	filter { "system:not windows" }
		libdirs { llbc_lib_root_dir .. "/$(config)" }
	filter {}
	
	-- set llbc links
	filter { "system:windows", "configurations:debug*" }
		links { "libllbc_debug.lib" }
	filter {}
	filter { "system:windows", "configurations:release*" }
		links { "libllbc.lib" }
	filter {}
		filter { "system:not windows", "configurations:debug*" }
		links { "llbc_debug" }
	filter {}
	filter { "system:not windows", "configurations:release*" }
		links { "llbc" }
	filter {}
	
	-- link dl/pthread library
	filter { "system:linux" }
        links {
            "dl",
			"pthread",
        }
    filter {}
	
	-- link ws2_32 library
	filter { "system:windows" }
        links {
            "ws2_32",
        }
    filter {}
	
	-- copy library files to component library
	local comp_dir = COMPS_ROOT_DIR .. "/" .. comp_name
	postbuildmessage(string.format(comp_name, "Copying llbc library files to component[%s] library...", comp_name))
	filter { "system:windows", "configurations:debug*" }
		postbuildcommands("copy /Y " .. string.gsub(string.format("\"%s/$(Configuration)/libllbc_debug.dll\" \"%s/lib/%s/$(Configuration)/\"", llbc_lib_root_dir, comp_dir, _ACTION), "/", "\\"))
		postbuildcommands("copy /Y " .. string.gsub(string.format("\"%s/$(Configuration)/libllbc_debug.lib\" \"%s/lib/%s/$(Configuration)/\"", llbc_lib_root_dir, comp_dir, _ACTION), "/" ,"\\"))
		postbuildcommands("copy /Y " .. string.gsub(string.format("\"%s/$(Configuration)/libllbc_debug.pdb\" \"%s/lib/%s/$(Configuration)/\"", llbc_lib_root_dir, comp_dir, _ACTION), "/", "\\"))
	filter {}
	filter { "system:windows", "configurations:release*" }
		postbuildcommands("copy /Y " .. string.gsub(string.format("\"%s/$(Configuration)/libllbc.dll\" \"%s/lib/%s/$(Configuration)/\"", llbc_lib_root_dir, comp_dir, _ACTION), "/", "\\"))
		postbuildcommands("copy /Y " .. string.gsub(string.format("\"%s/$(Configuration)/libllbc.lib\" \"%s/lib/%s/$(Configuration)/\"", llbc_lib_root_dir, comp_dir, _ACTION), "/", "\\"))
		postbuildcommands("copy /Y " .. string.gsub(string.format("\"%s/$(Configuration)/libllbc.pdb\" \"%s/lib/%s/$(Configuration)/\"", llbc_lib_root_dir, comp_dir, _ACTION), "/", "\\"))
	filter {}
	
	filter { "system:linux", "configurations:debug*" }
		postbuildcommands(string.format("\\cp -rf \"%s/${Configuration}/libllbc_debug.so\" \"%s/lib/%s/$(config)/\"", llbc_lib_root_dir, comp_dir, _ACTION))
	filter {}
	filter { "system:not windows", "configurations:release*" }
		postbuildcommands(string.format("\\cp -rf \"%s/${Configuration}/libllbc.so\" \"%s/lib/%s/$(config)/\"", llbc_lib_root_dir, comp_dir, _ACTION))
	filter {}
	
	filter { "system:macosx", "configurations:debug*" }
		postbuildcommands(string.format("\\cp -rf \"%s/${Configuration}/libllbc_debug.dylib\" \"%s/lib/%s/$(config)/\"", llbc_lib_root_dir, comp_dir, _ACTION))
	filter {}
	filter { "system:macosx", "configurations:release*" }
		postbuildcommands(string.format("\\cp -rf \"%s/${Configuration}/libllbc.dylib\" \"%s/lib/%s/$(config)/\"", llbc_lib_root_dir, comp_dir, _ACTION))
	filter {}
end
