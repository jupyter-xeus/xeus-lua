/***************************************************************************
* Copyright (c) 2021, Thorsten Beier                                       *                                                       *
* Copyright (c) 2021, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_LUA_CONFIG_HPP
#define XEUS_LUA_CONFIG_HPP

// Project version
#define XEUS_LUA_VERSION_MAJOR 0
#define XEUS_LUA_VERSION_MINOR 6
#define XEUS_LUA_VERSION_PATCH 2

// Composing the version string from major, minor and patch
#define XEUS_LUA_CONCATENATE(A, B) XEUS_LUA_CONCATENATE_IMPL(A, B)
#define XEUS_LUA_CONCATENATE_IMPL(A, B) A##B
#define XEUS_LUA_STRINGIFY(a) XEUS_LUA_STRINGIFY_IMPL(a)
#define XEUS_LUA_STRINGIFY_IMPL(a) #a

#define XEUS_LUA_VERSION XEUS_LUA_STRINGIFY(XEUS_LUA_CONCATENATE(XEUS_LUA_VERSION_MAJOR,   \
                 XEUS_LUA_CONCATENATE(.,XEUS_LUA_CONCATENATE(XEUS_LUA_VERSION_MINOR,   \
                                  XEUS_LUA_CONCATENATE(.,XEUS_LUA_VERSION_PATCH)))))

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
