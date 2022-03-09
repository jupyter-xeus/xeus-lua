#############################################################################
#Copyright (c) 2021,
#
#Distributed under the terms of the BSD 3-Clause License.
#
#The full license is in the file LICENSE, distributed with this software.
#############################################################################

cmake_minimum_required(VERSION 3.4.3)
project(xeus-lua)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(XEUS_LUA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(STRINGS "${XEUS_LUA_INCLUDE_DIR}/xeus-lua/xeus_lua_config.hpp" version_defines
     REGEX "#define XEUS_LUA_VERSION_(MAJOR|MINOR|PATCH)")
foreach (ver ${version_defines})
    if (ver MATCHES "#define XEUS_LUA_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XEUS_LUA_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif ()
endforeach ()
set(${PROJECT_NAME}_VERSION
    ${XEUS_LUA_VERSION_MAJOR}.${XEUS_LUA_VERSION_MINOR}.${XEUS_LUA_VERSION_PATCH})
message(STATUS "Building xeus-lua v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

if (NOT DEFINED XEUS_LUA_KERNELSPEC_PATH)
    set(XEUS_LUA_KERNELSPEC_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/")
endif ()



configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xlua/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xlua/kernel.json"
)

# Build options
# =============

# Compilation options
OPTION(XEUS_LUA_DISABLE_ARCH_NATIVE "disable -march=native flag" OFF)
OPTION(XEUS_LUA_DISABLE_TUNE_GENERIC "disable -mtune=generic flag" OFF)

option(XEUS_LUA_BUILD_STATIC "Build xeus-lua static library" OFF)
OPTION(XEUS_LUA_BUILD_SHARED "Split xlua build into executable and library" ON)
OPTION(XEUS_LUA_BUILD_EXECUTABLE "Build the xlua executable" ON)
OPTION(XEUS_LUA_USE_LUAJIT "Use LuaJit" OFF)

OPTION(XEUS_LUA_USE_SHARED_XEUS "Link xlua  with the xeus shared library (instead of the static library)" ON)
OPTION(XEUS_LUA_USE_SHARED_XEUS_LUA "Link xlua  with the xeus shared library (instead of the static library)" ON)
OPTION(XLUA_WITH_XWIDGETS "Build xeus-lua with xwidgets" ON)
OPTION(XLUA_USE_SHARED_XWIDGETS "Link xlua  with the xwidgets shared library (instead of the static library)" ON)
OPTION(XEUS_LUA_EMSCRIPTEN_WASM_BUILD "Build for wasm with emscripten" OFF)

OPTION(XLUA_WITH_XCANVAS      "Build xeus-lua with xcanvas" ON)
OPTION(XLUA_USE_SHARED_XCANVAS "Link xlua  with the xcanvas shared library (instead of the static library)" ON)

if(XEUS_LUA_EMSCRIPTEN_WASM_BUILD)
    set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_emscripten;${CMAKE_MODULE_PATH}")
    add_compile_definitions(XEUS_LUA_EMSCRIPTEN_WASM_BUILD)
    message("Build with emscripten")
    SET(XEUS_LUA_DISABLE_ARCH_NATIVE ON)
    SET(XEUS_LUA_DISABLE_TUNE_GENERIC ON)
    SET(XEUS_LUA_BUILD_STATIC ON)
    SET(XEUS_LUA_BUILD_SHARED OFF)
    SET(XEUS_LUA_BUILD_EXECUTABLE OFF)
    SET(XEUS_LUA_USE_SHARED_XEUS OFF)
    SET(XEUS_LUA_USE_SHARED_XEUS_LUA OFF)
    SET(XEUS_LUA_BUILD_TESTS OFF)
    SET(XLUA_WITH_XCANVAS ON)
    SET(XLUA_WITH_XWIDGETS ON)
    SET(XLUA_USE_SHARED_XWIDGETS OFF)
    SET(XLUA_USE_SHARED_XCANVAS OFF)
endif()

# Dependencies
# ============

set(xtl_REQUIRED_VERSION 0.7.0)
set(xeus_REQUIRED_VERSION 2.4.0)

set(xwidgets_REQUIRED_VERSION 0.26.1)
set(xcanvas_REQUIRED_VERSION 0.2.2)
set(xproperty_REQUIRED_VERSION 0.11.0)
set(Lua_REQUIRED_VERSION 5.3.4)

if (NOT TARGET xtl)
    find_package(xtl ${xtl_REQUIRED_VERSION} REQUIRED)
endif ()
if (NOT TARGET xeus AND NOT TARGET xeus-static)
    find_package(xeus ${xeus_REQUIRED_VERSION} REQUIRED)
endif ()

if(NOT XEUS_LUA_USE_LUAJIT)
    if (NOT TARGET Lua) # not sure about these lines
        find_package(Lua ${lua_REQUIRED_VERSION} REQUIRED)
    endif ()
else()
    if (NOT TARGET LuaJIT) # not sure about these lines
        find_package(LuaJIT  REQUIRED)
    endif ()
endif()

# Flags
# =====
include(CheckCXXCompilerFlag)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4141")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4267 /wd4715 /wd4146 /wd4129")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    if(NOT XEUS_LUA_EMSCRIPTEN_WASM_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
    endif()
    if (XEUS_LUA_DISABLE_ARCH_NATIVE AND NOT XEUS_LUA_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic")
    elseif (XEUS_LUA_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif ()


    CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
    if (HAS_CPP_17_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    else ()
        message(FATAL_ERROR "Unsupported compiler -- xeus requires C++17 support!")
    endif ()

endif ()

if(XEUS_LUA_USE_LUAJIT)
    add_compile_definitions(XEUS_LUA_USE_LUAJIT)
    add_compile_definitions(SOL_LUAJIT=1)
endif()


if(XLUA_WITH_XWIDGETS)
    add_compile_definitions(XLUA_WITH_XWIDGETS)
endif()

if(XLUA_WITH_XWIDGETS OR XLUA_WITH_XCANVAS)
    if (NOT TARGET xwidgets)
        find_package(xwidgets ${xwidgets_REQUIRED_VERSION} REQUIRED)
        find_package(xproperty ${xproperty_REQUIRED_VERSION} REQUIRED)
    endif ()
endif()


if(XLUA_WITH_XCANVAS)
    add_compile_definitions(XLUA_WITH_XCANVAS)
endif()

if(XLUA_WITH_XCANVAS)
    if (NOT TARGET xcanvas)
        find_package(xcanvas REQUIRED)
    endif ()
endif()

if(XEUS_LUA_EMSCRIPTEN_WASM_BUILD)
    # ENV (https://github.com/emscripten-core/emscripten/commit/6d9681ad04f60b41ef6345ab06c29bbc9eeb84e0)
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=[ENV']\"")
endif()

# Source files
# ============

set(XEUS_LUA_HEADERS
    include/xeus-lua/xeus_lua_config.hpp
    include/xeus-lua/xinterpreter.hpp
)

set(XEUS_LUA_SRC
    src/xinterpreter.cpp
    src/xextend.cpp
    src/xcomplete.cpp
    src/xio.cpp
    src/xdisplay.cpp
    src/xjson.cpp
    src/xtime.cpp
)

if(XLUA_WITH_XWIDGETS)
    set(XEUS_LUA_SRC
        ${XEUS_LUA_SRC}
        src/xwidgets.cpp
    )
endif()

if(XLUA_WITH_XCANVAS)
    set(XEUS_LUA_SRC
        ${XEUS_LUA_SRC}
        src/xcanvas.cpp
    )
endif()


set(XEUS_LUA_MAIN_SRC
    src/main.cpp
)

if(NOT XEUS_LUA_USE_LUAJIT)
    SET(XEUS_LUA_LUA_LIBRARIES   ${LUA_LIBRARIES})
    SET(XEUS_LUA_LUA_INCLUDE_DIR ${LUA_INCLUDE_DIR})
else()
    SET(XEUS_LUA_LUA_LIBRARIES ${LUAJIT_LIBRARIES})
    SET(XEUS_LUA_LUA_INCLUDE_DIR ${LUAJIT_INCLUDE_DIR})
endif()

# Targets and link - Macros
# =========================

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

macro(xeus_lua_set_common_options target_name)
    if (MSVC)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4141)
        target_compile_options(${target_name} PUBLIC /wd4018 /wd4267 /wd4715 /wd4146 /wd4129)
    endif ()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")

        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)

        # Mtune generic/native
        if (XEUS_LUA_DISABLE_ARCH_NATIVE AND NOT XEUS_LUA_DISABLE_TUNE_GENERIC)
            target_compile_options(${target_name} PUBLIC -mtune=generic)
        elseif (XEUS_LUA_DISABLE_TUNE_GENERIC)
        else ()
            target_compile_options(${target_name} PUBLIC -march=native)
        endif ()

        # 17 flag
        CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
        if (HAS_CPP_17_FLAG)
            target_compile_features(${target_name} PRIVATE cxx_std_17)
        else ()
            message(FATAL_ERROR "Unsupported compiler -- xeus-lua requires 17 support!")
        endif ()
    endif ()

    if (APPLE)
        set_target_properties(${target_name} PROPERTIES
            MACOSX_RPATH ON
        )
    else ()
        set_target_properties(${target_name} PROPERTIES
            BUILD_WITH_INSTALL_RPATH 1
            SKIP_BUILD_RPATH FALSE
        )
    endif ()

    set_target_properties(${target_name} PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
endmacro()

# Common macro kernels (xlua )
macro(xeus_lua_set_kernel_options target_name)
    if (XEUS_LUA_USE_SHARED_XEUS_LUA)
        target_link_libraries(${target_name} PRIVATE xeus-lua)
        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else ()
        target_link_libraries(${target_name} PRIVATE xeus-lua-static)
    endif()
    if(NOT XEUS_LUA_EMSCRIPTEN_WASM_BUILD)
        find_package(Threads)
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${XEUS_LUA_LUA_LIBRARIES})
    else()
        target_link_libraries(${target_name} PRIVATE ${XEUS_LUA_LUA_LIBRARIES})
    endif()
endmacro()

# Common macro for shared and static library xeus-lua
macro(xeus_lua_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    add_library(${target_name} ${linkage_upper} ${XEUS_LUA_SRC} ${XEUS_LUA_HEADERS})
    xeus_lua_set_common_options(${target_name})

    set_target_properties(${target_name} PROPERTIES
                          PUBLIC_HEADER "${XEUS_LUA_HEADERS}"
                          PREFIX ""
                          VERSION ${${PROJECT_NAME}_VERSION}
                          SOVERSION ${XEUS_LUA_VERSION_MAJOR}
                          OUTPUT_NAME "lib${output_name}")

    target_compile_definitions(${target_name} PUBLIC "XEUS_LUA_EXPORTS")

    target_compile_features(${target_name} PRIVATE cxx_std_17)



    target_include_directories(${target_name}
                               PUBLIC
                               ${XEUS_LUA_LUA_INCLUDE_DIR}
                               $<BUILD_INTERFACE:${XEUS_LUA_INCLUDE_DIR}>
                               $<INSTALL_INTERFACE:include>)

    if (XEUS_LUA_USE_SHARED_XEUS)
        set(XEUS_LUA_XEUS_TARGET xeus)
    else ()
        set(XEUS_LUA_XEUS_TARGET xeus-static)
    endif ()

    if(XLUA_WITH_XWIDGETS OR XLUA_WITH_XCANVAS)
        if (XLUA_USE_SHARED_XWIDGETS)
            set(XLUA_XWIDGETS_TARGET xwidgets)
        else ()
            set(XLUA_XWIDGETS_TARGET xwidgets-static)
        endif ()
        set(XLUA_XPROPERTY_TARGET xproperty)
    else()
        set(XLUA_XPROPERTY_TARGET "")
        set(XLUA_XWIDGETS_TARGET "")
    endif()


    if(XLUA_WITH_XCANVAS)
        if(XLUA_USE_SHARED_XCANVAS)
            set(XLUA_XCANVAS_TARGET xcanvas)
        else()
            set(XLUA_XCANVAS_TARGET xcanvas-static)
        endif()
    else()
        set(XLUA_XCANVAS_TARGET "")
    endif()

    target_link_libraries(${target_name} PUBLIC ${XEUS_LUA_XEUS_TARGET} xtl
        ${XLUA_XWIDGETS_TARGET}
        ${XLUA_XPROPERTY_TARGET}
        ${XLUA_XCANVAS_TARGET}
    )
    if (WIN32 OR CYGWIN)
        #
    elseif (APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif ()
    if(NOT XEUS_LUA_EMSCRIPTEN_WASM_BUILD)
        find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${XEUS_LUA_LUA_LIBRARIES})
    else()
        target_link_libraries(${target_name} PRIVATE ${XEUS_LUA_LUA_LIBRARIES})
    endif()

endmacro()

# xeus-lua
# ===========

set(XEUS_LUA_TARGETS "")

if (XEUS_LUA_BUILD_SHARED)
    # Build libraries
    xeus_lua_create_target(xeus-lua SHARED xeus-lua)
    list(APPEND XEUS_LUA_TARGETS xeus-lua)
endif ()

if (XEUS_LUA_BUILD_STATIC)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        xeus_lua_create_target(xeus-lua-static STATIC xeus-lua-static)
    else ()
        xeus_lua_create_target(xeus-lua-static STATIC xeus-lua)
    endif ()
    list(APPEND XEUS_LUA_TARGETS xeus-lua-static)
endif ()

# xeus-lua
# =======
if (XEUS_LUA_BUILD_EXECUTABLE)
    add_executable(xlua ${XEUS_LUA_MAIN_SRC})
    target_compile_features(xlua PRIVATE cxx_std_17)
    xeus_lua_set_common_options(xlua)
    xeus_lua_set_kernel_options(xlua)
endif()

if(XEUS_LUA_EMSCRIPTEN_WASM_BUILD)
    message(STATUS ${XEUS_LUA_LUA_LIBRARIES})
    add_executable(xeus_kernel src/main_emscripten_kernel.cpp )
    target_link_libraries(xeus_kernel xeus-lua-static)
    target_compile_features(xeus_kernel PRIVATE cxx_std_17)
endif()


# Installation
# ============

include(CMakePackageConfigHelpers)

set(XEUS_LUA_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeus-luaConfig.cmake")

# Install xeus-lua and xeus-lua-static
if (XEUS_LUA_BUILD_SHARED)
    install(TARGETS ${XEUS_LUA_TARGETS}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xeus-lua)

    # Makes the project importable from the build directory
    export(EXPORT ${PROJECT_NAME}-targets
           FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
endif ()

# Install xlua
if (XEUS_LUA_BUILD_EXECUTABLE)
    install(TARGETS xlua
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Configuration and data directories for jupyter and xeus-lua
    set(XJUPYTER_DATA_DIR "share/jupyter"    CACHE STRING "Jupyter data directory")

    # Install xlua Jupyter kernelspec
    set(KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
    install(DIRECTORY ${KERNELSPEC_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE)


    # Extra path for installing Jupyter kernelspec
    if (XEXTRA_JUPYTER_DATA_DIR)
        install(DIRECTORY ${KERNELSPEC_DIR}
                DESTINATION ${XEXTRA_JUPYTER_DATA_DIR}
                PATTERN "*.in" EXCLUDE)
    endif ()
endif ()

# Configure 'xeus-luaConfig.cmake' for a build tree
set(XEUS_LUA_CONFIG_CODE "####### Expanded from \@XEUS_LUA_CONFIG_CODE\@ #######\n")
set(XEUS_LUA_CONFIG_CODE "${XEUS_LUA_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set(XEUS_LUA_CONFIG_CODE "${XEUS_LUA_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure 'xeus-luaConfig.cmake.in for an install tree
set(XEUS_LUA_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${XEUS_LUA_CMAKECONFIG_INSTALL_DIR})

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION ${XEUS_LUA_CMAKECONFIG_INSTALL_DIR})

if (XEUS_LUA_BUILD_SHARED)
    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION ${XEUS_LUA_CMAKECONFIG_INSTALL_DIR})
endif ()
