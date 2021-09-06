/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay, and     *
* Wolf Vollprecht                                                          *
* Copyright (c) 2018, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XLUA_CONFIG_HPP
#define XLUA_CONFIG_HPP

// Project version
#define XLUA_VERSION_MAJOR 0
#define XLUA_VERSION_MINOR 1
#define XLUA_VERSION_PATCH 4

// Composing the version string from major, minor and patch
#define XLUA_CONCATENATE(A, B) XLUA_CONCATENATE_IMPL(A, B)
#define XLUA_CONCATENATE_IMPL(A, B) A##B
#define XLUA_STRINGIFY(a) XLUA_STRINGIFY_IMPL(a)
#define XLUA_STRINGIFY_IMPL(a) #a

#define XLUA_VERSION XLUA_STRINGIFY(XLUA_CONCATENATE(XLUA_VERSION_MAJOR,   \
                 XLUA_CONCATENATE(.,XLUA_CONCATENATE(XLUA_VERSION_MINOR,   \
                                  XLUA_CONCATENATE(.,XLUA_VERSION_PATCH)))))

#ifdef _WIN32
    #ifdef XEUS_LUA_EXPORTS
        #define XEUS_LUA_API __declspec(dllexport)
    #else
        #define XEUS_LUA_API __declspec(dllimport)
    #endif
#else
    #define XEUS_LUA_API
#endif

#endif