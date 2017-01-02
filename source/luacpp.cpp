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

#include "luacpp.h"

int Cloud::Lua::LuaPrint(lua_State* state)
{
    int nargs = lua_gettop(state);
    Lua::Stringstream stream;

    for (int i = 1; i <= nargs; ++i)
    {
        stream << lua_tostring(state, i);
    }

    LUACPP_TRACE(stream.str().c_str());

    return 0;
}

int Cloud::Lua::LuaPanic(lua_State* state)
{
    LUACPP_TRACE("PANIC: unprotected error in call to Lua API:\n(%s)\n", lua_tostring(state, -1));
    return 0;  /* return to Lua to abort */
}

Cloud::Lua::StateUniquePtr Cloud::Lua::NewState()
{
    auto* state = luaL_newstate();
    if (state)
    {
        lua_atpanic(state, &LuaPanic);
    }

    return StateUniquePtr(state);
}

Cloud::Lua::StateUniquePtr Cloud::Lua::NewStateAndSetup()
{
    auto luaState = NewState();

    // open standard libs
    auto* s = luaState.get();
    luaL_openlibs(s);


    // redirect print function
    const luaL_Reg printlib[] = {
        { "print", Lua::LuaPrint },
        { nullptr, nullptr }, /* end of array */
    };

    lua_getglobal(s, "_G");
    luaL_setfuncs(s, printlib, 0);
    lua_pop(s, 1);


    return luaState;
}

void Cloud::Lua::DefaultTrace(const CLchar* output, ...)
{
    va_list argList;
    va_start(argList, output);

    printf(output, argList);
    printf("\n");

    va_end(argList);
}
