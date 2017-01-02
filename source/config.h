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

#ifndef CLOUD_LUA_CPP_CONFIG_HEADER
#define CLOUD_LUA_CPP_CONFIG_HEADER

#include <string>
#include <sstream>
#include <unordered_map>
#include <memory>
#include <functional>
#include <typeinfo>
#include <assert.h>

#ifdef _DEBUG
#define LUACPP_DEBUG
#endif

#ifdef LUACPP_DEBUG

#define LUACPP_TRACE Cloud::Lua::DefaultTrace
#define LUACPP_ASSERT(expression, message)  assert(expression && (message))
#define LUACPP_ASSERT_MSG(message)          assert(false && (message))
#define LUACPP_ASSERT_NULL(variable)        assert(variable != 0 && "variable isn't allowed to be null!")

#else

#define LUACPP_TRACE
#define LUACPP_ASSERT(expression, message) 
#define LUACPP_ASSERT_MSG(message)
#define LUACPP_ASSERT_NULL(variable) 
#endif

#define LUACPP_UNUSED(variable) (variable)

using CLbool = bool;
using CLint = int;
using CLfloat = float;
using CLchar = char;
using CLsize_t = std::size_t;

namespace Cloud
{
    namespace Lua
    {
        using Stringstream = std::stringstream;

        template <class _T, class _D = std::default_delete<_T>>
        using UniquePtr = std::unique_ptr<_T, _D>;

        template <class _T>
        constexpr UniquePtr<_T> MakeUnique = std::make_unique<_T>;

        template <class _Fty>
        using Function = std::function<_Fty>;

        template <class _Kty, class _Ty>
        using UnorderedMap = std::unordered_map<_Kty, _Ty>;

        using String = std::string;
    }
}



#endif // CLOUD_LUA_CPP_CONFIG_HEADER