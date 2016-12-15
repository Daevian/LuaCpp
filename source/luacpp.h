/*
* LuaCpp
* 
* Copyright (c) 2016 Robin Doeleman
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE 
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CLOUD_LUA_CPP_HEADER
#define CLOUD_LUA_CPP_HEADER

#include "utility.h"

#include "src/lua.hpp"

#include <sstream>
#include <memory>
#include <functional>
#include <typeinfo>
#include <unordered_map>
#include "../../core_libraries/debugging/DebugLog.h"
#include "../../core_libraries/utilities/DefinesMacros.h"

namespace Cloud
{
    namespace Lua
    {
        enum class ErrorCode : int
        {
            Ok          = LUA_OK,
            Yield       = LUA_YIELD,
            ErrRun      = LUA_ERRRUN,
            ErrSyntax   = LUA_ERRSYNTAX,
            ErrMem      = LUA_ERRMEM,
            ErrGcMM     = LUA_ERRGCMM,
            ErrErr      = LUA_ERRERR,
            ErrFile     = LUA_ERRFILE,
        };

        enum class Type : int
        {
            None            = LUA_TNONE,
            Nil             = LUA_TNIL,
            Boolean         = LUA_TBOOLEAN,
            LightUserData   = LUA_TLIGHTUSERDATA,
            Number          = LUA_TNUMBER,
            String          = LUA_TSTRING,
            Table           = LUA_TTABLE,
            Function        = LUA_TFUNCTION,
            UserData        = LUA_TUSERDATA,
            Thread          = LUA_TTHREAD,
        };

        struct Deleter
        {
            void operator()(lua_State* ptr)
            {
                lua_close(ptr);
                ptr = nullptr;
            }
        };

        typedef std::unique_ptr<lua_State, Deleter> StateUniquePtr;

        int LuaPrint(lua_State* state);
        int LuaPanic(lua_State* state);
        
        StateUniquePtr NewState();
        StateUniquePtr NewStateAndSetup();
    }

}

#endif // CLOUD_LUA_CPP_HEADER