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

#ifndef CLOUD_LUA_CPP_STATE_INLINE
#define CLOUD_LUA_CPP_STATE_INLINE

namespace Cloud
{
    template <>
    inline CLbool Cloud::LuaState::To(CLint stackIndex) const
    {
        return lua_toboolean(GetState(), stackIndex) != 0 ? true : false;
    }

    template <>
    inline CLint Cloud::LuaState::To(CLint stackIndex) const
    {
        return static_cast<CLint>(lua_tointeger(GetState(), stackIndex));
    }

    template <>
    inline CLfloat Cloud::LuaState::To(CLint stackIndex) const
    {
        return static_cast<CLfloat>(lua_tonumber(GetState(), stackIndex));
    }

    template <>
    inline const CLchar* Cloud::LuaState::To(CLint stackIndex) const
    {
        // TODO: might change stack value to str, so not const or thread-safe
        return lua_tostring(GetState(), stackIndex);
    }
}

#endif // CLOUD_LUA_CPP_STATE_INLINE