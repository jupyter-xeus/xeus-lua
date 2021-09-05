############################################################################
# Copyright (c) 2016, Martin Renou, Johan Mabille, Sylvain Corlay, and     #
# Wolf Vollprecht                                                          #
# Copyright (c) 2016, QuantStack                                           #
#                                                                          #
# Distributed under the terms of the BSD 3-Clause License.                 #
#                                                                          #
# The full license is in the file LICENSE, distributed with this software. #
############################################################################

cmake_minimum_required(VERSION 3.4.3)
project(xeus-lua)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(XEUS_LUA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Versionning
# ===========

file(STRINGS "${XEUS_LUA_INCLUDE_DIR}/xeus-lua/xeus_lua_config.hpp" xlua_version_defines
     REGEX "#define XLUA_VERSION_(MAJOR|MINOR|PATCH)")
foreach (ver ${xlua_version_defines})
    if (ver MATCHES "#define XLUA_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(XLUA_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif ()
endforeach ()
set(${PROJECT_NAME}_VERSION
    ${XLUA_VERSION_MAJOR}.${XLUA_VERSION_MINOR}.${XLUA_VERSION_PATCH})
message(STATUS "Building xeus-lua v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

if (NOT DEFINED XLUA_KERNELSPEC_PATH)
    set(XLUA_KERNELSPEC_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/")
endif ()

if (NOT DEFINED XLUA_LUA_EXTRA_PACKAGE_PATH)
    set(XLUA_LUA_EXTRA_PACKAGE_PATH "${CMAKE_INSTALL_PREFIX}/share/jupyter/kernels/xlua/lua_packages")
endif ()

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xlua/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xlua/kernel.json"
)

# Build options
# =============

# Compilation options
OPTION(XLUA_DISABLE_ARCH_NATIVE "disable -march=native flag" OFF)
OPTION(XLUA_DISABLE_TUNE_GENERIC "disable -mtune=generic flag" OFF)

option(XLUA_BUILD_STATIC "Build xeus-lua static library" ON)
OPTION(XLUA_BUILD_SHARED "Split xlua build into executable and library" ON)
OPTION(XLUA_BUILD_XLUA_EXECUTABLE "Build the xlua executable" ON)

option(XLUA_WITH_XWIDGETS "Build xeus-lua with xwidgets" ON)
OPTION(XLUA_USE_SHARED_XWIDGETS "Link xlua  with the xwidgets shared library (instead of the static library)" ON)

OPTION(XLUA_USE_SHARED_XEUS "Link xlua  with the xeus shared library (instead of the static library)" ON)
OPTION(XLUA_USE_SHARED_XEUS_LUA "Link xlua  with the xeus shared library (instead of the static library)" ON)

# Test options
OPTION(XLUA_BUILD_TESTS "xeus-lua test suite" OFF)
OPTION(XLUA_DOWNLOAD_GTEST "build gtest from downloaded sources" OFF)



if(EMSCRIPTEN)
    message("Build with emscripten")
    SET(XLUA_DISABLE_ARCH_NATIVE ON)
    SET(XLUA_DISABLE_TUNE_GENERIC ON)
    SET(XLUA_BUILD_STATIC ON)
    SET(XLUA_BUILD_SHARED OFF)
    SET(XLUA_BUILD_XLUA_EXECUTABLE OFF)
    SET(XLUA_USE_SHARED_XEUS OFF)
    SET(XLUA_USE_SHARED_XEUS_LUA OFF)
    SET(XLUA_BUILD_TESTS OFF)
    SET(XLUA_DOWNLOAD_GTEST OFF)
endif()


# Dependencies
# ============

set(xtl_REQUIRED_VERSION 0.6.23)
set(xeus_REQUIRED_VERSION 0.25.0)
set(xwidgets_REQUIRED_VERSION 0.25.0)
set(xproperty_REQUIRED_VERSION 0.11.0)

set(Lua_REQUIRED_VERSION 5.3.4)

if(XLUA_WITH_XWIDGETS)
    if (NOT TARGET xwidgets)
        find_package(xwidgets ${xwidgets_REQUIRED_VERSION} REQUIRED)
        find_package(xproperty ${xproperty_REQUIRED_VERSION} REQUIRED)

    endif ()
endif()
if (NOT TARGET xtl)
    find_package(xtl ${xtl_REQUIRED_VERSION} REQUIRED)
endif ()
if (NOT TARGET xeus AND NOT TARGET xeus-static)
    find_package(xeus ${xeus_REQUIRED_VERSION} REQUIRED)
endif ()
if (NOT TARGET Lua) # not sure about these lines
    find_package(Lua ${lua_REQUIRED_VERSION} REQUIRED)
endif ()


# Flags
# =====

include(CheckCXXCompilerFlag)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4141")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4267 /wd4715 /wd4146 /wd4129")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    if(NOT EMSCRIPTEN)    
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
    endif()
    if (XLUA_DISABLE_ARCH_NATIVE AND NOT XLUA_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=generic")
    elseif (XLUA_DISABLE_TUNE_GENERIC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    endif ()

    CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)

    if (HAS_CPP14_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else ()
        message(FATAL_ERROR "Unsupported compiler -- xeus requires C++14 support!")
    endif ()
endif ()

if(XLUA_WITH_XWIDGETS)
    add_compile_definitions(XLUA_WITH_XWIDGETS)
endif()

if(EMSCRIPTEN)

    add_compile_definitions(EMSCRIPTEN)
    add_compile_definitions(EMSCRIPTEN_ZMQ_REPLACEMENT)

    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} --bind --std=c++17")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -Wno-unused-parameter -Wno-deprecated -Wno-unused-command-line-argument")
    #set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -lwebsocket.js")
    # module / import 
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s MODULARIZE=1 -s EXPORT_NAME=\"createXeusModule\" -s EXPORT_ES6=0 -s USE_ES6_IMPORT_META=0")
    # safty
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES}  -s  DEMANGLE_SUPPORT=1 -s SAFE_HEAP=0  -s ASSERTIONS=2   -s STACK_OVERFLOW_CHECK=1 ")
    # web
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s ENVIRONMENT=web,worker")
    # mem
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s TOTAL_MEMORY=512MB")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s EXIT_RUNTIME=1")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s WASM=1")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s ERROR_ON_UNDEFINED_SYMBOLS=1")

    # threads
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s USE_PTHREADS=1 -pthread  -mbulk-memory  -matomics")

    # fetch related
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES}")

    # exceptions    
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s DISABLE_EXCEPTION_CATCHING=0 ")

    # async
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s ASYNCIFY=1 -s 'ASYNCIFY_IMPORTS=[\"async_get_input_function\"]'")
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s 'ASYNCIFY_STACK_SIZE=20480'")

    # ENV (https://github.com/emscripten-core/emscripten/commit/6d9681ad04f60b41ef6345ab06c29bbc9eeb84e0)
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=[ENV']\"")

    # filesystem
    file(COPY asset_dir DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} --embed-file asset_dir")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FEATURES}")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS}  ${EMSCRIPTEN_FEATURES} ---proxy-to-worker")
endif()

# Source files
# ============

set(XEUS_LUA_SRC
    src/xinterpreter.cpp
)

set(XEUS_LUA_HEADERS
    include/xeus-lua/xeus_lua_config.hpp
    include/xeus-lua/xinterpreter.hpp
)


set(XLUA_COMMON_SRC 
    src/xextend.cpp
    src/xcomplete.cpp
    src/xio.cpp
    src/xdisplay.cpp
)

if(XLUA_WITH_XWIDGETS)
    set(XLUA_COMMON_SRC
        ${XLUA_COMMON_SRC}
        src/xwidgets.cpp
    )
endif()

set(XLUA_SRC
    src/main.cpp
     ${XLUA_COMMON_SRC}
)


# Targets and link - Macros
# =========================

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

macro(xlua_set_common_options target_name)
    if (MSVC)
        target_compilex_options(${target_name} PUBLIC /wd4251 /wd4141)
        target_compile_options(${target_name} PUBLIC /wd4018 /wd4267 /wd4715 /wd4146 /wd4129)
    endif ()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")

        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)

        # Mtune generic/native
        if (XLUA_DISABLE_ARCH_NATIVE AND NOT XLUA_DISABLE_TUNE_GENERIC)
            target_compile_options(${target_name} PUBLIC -mtune=generic)
        elseif (XLUA_DISABLE_TUNE_GENERIC)
        else ()
            target_compile_options(${target_name} PUBLIC -march=native)
        endif ()

        # C++14 flag
        CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)
        if (HAS_CPP14_FLAG)
            target_compile_features(${target_name} PRIVATE cxx_std_14)
        else ()
            message(FATAL_ERROR "Unsupported compiler -- xeus-lua requires C++14 support!")
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
macro(xlua_set_kernel_options target_name)
   

    if (XLUA_USE_SHARED_XEUS_LUA)
        target_link_libraries(${target_name} PRIVATE xeus-lua)

        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else ()
        target_link_libraries(${target_name} PRIVATE xeus-lua-static)
    endif()

    find_package(Threads) # TODO: add Threads as a dependence of xeus or xeus-static?
    target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endmacro()

# Common macro for shared and static library xeus-lua
macro(xlua_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    add_library(${target_name} ${linkage_upper} ${XEUS_LUA_SRC} ${XEUS_LUA_HEADERS})
    xlua_set_common_options(${target_name})

    set_target_properties(${target_name} PROPERTIES
                          PUBLIC_HEADER "${XEUS_LUA_HEADERS}"
                          PREFIX ""
                          VERSION ${${PROJECT_NAME}_VERSION}
                          SOVERSION ${XLUA_VERSION_MAJOR}
                          OUTPUT_NAME "lib${output_name}")

    target_compile_definitions(${target_name} PUBLIC "XEUS_LUA_EXPORTS")

    target_compile_features(${target_name} PRIVATE cxx_std_17)

    target_include_directories(${target_name}
                               PUBLIC
                               ${LUA_INCLUDE_DIR}
                               ${PYTHON_INCLUDE_DIRS}
                               $<BUILD_INTERFACE:${XEUS_LUA_INCLUDE_DIR}>
                               $<INSTALL_INTERFACE:include>)

    if (XLUA_USE_SHARED_XEUS)
        set(XLUA_XEUS_TARGET xeus)
    else ()
        set(XLUA_XEUS_TARGET xeus-static)
    endif ()

    if(XLUA_WITH_XWIDGETS)
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

    target_link_libraries(${target_name} PUBLIC ${XLUA_XEUS_TARGET} ${XLUA_XWIDGETS_TARGET} ${XLUA_XPROPERTY_TARGET})
    if (WIN32 OR CYGWIN)
        # 
    elseif (APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif ()

    find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
    target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT} ${LUA_LIBRARY})

endmacro()

# xeus-lua
# ===========

set(XEUS_LUA_TARGETS "")

if (XLUA_BUILD_SHARED)
    # Build libraries
    xlua_create_target(xeus-lua SHARED xeus-lua)
    list(APPEND XEUS_LUA_TARGETS xeus-lua)
endif ()

if (XLUA_BUILD_STATIC)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        xlua_create_target(xeus-lua-static STATIC xeus-lua-static)
    else ()
        xlua_create_target(xeus-lua-static STATIC xeus-lua)
    endif ()
    list(APPEND XEUS_LUA_TARGETS xeus-lua-static)
endif ()

# xlua
# =======

if (XLUA_BUILD_XLUA_EXECUTABLE)
    add_executable(xlua ${XLUA_SRC})
    target_link_libraries(xlua PRIVATE ${LUA_LIBRARIES})
    target_compile_features(xlua PRIVATE cxx_std_17)
    xlua_set_common_options(xlua)
    xlua_set_kernel_options(xlua)
endif()




if(EMSCRIPTEN)
    message(STATUS ${LUA_LIBRARIES})

    add_executable(xeus_lua src/main_emscripten.cpp ${XLUA_COMMON_SRC})
    target_link_libraries(xeus_lua  xeus-lua-static ${LUA_LIBRARY} )
    target_compile_features(xeus_lua PRIVATE cxx_std_17)

    add_executable(xeus_lua_kernel src/main_emscripten_kernel.cpp  ${XLUA_COMMON_SRC})
    target_link_libraries(xeus_lua_kernel  xeus-lua-static ${LUA_LIBRARY} )
    target_compile_features(xeus_lua_kernel PRIVATE cxx_std_17)
endif()



# Tests
# =====

if(XLUA_DOWNLOAD_GTEST OR XLUA_GTEST_SRC_DIR)
    set(XLUA_BUILD_TESTS ON)
endif()

if(XLUA_BUILD_TESTS)
    add_subdirectory(test)
endif()

# Installation
# ============

include(CMakePackageConfigHelpers)

set(XEUS_LUA_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeus-luaConfig.cmake")

# Install xeus-lua and xeus-lua-static
if (XLUA_BUILD_SHARED)
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
if (XLUA_BUILD_XLUA_EXECUTABLE)
    install(TARGETS xlua
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

    # Configuration and data directories for jupyter and xeus-lua
    set(XJUPYTER_DATA_DIR "share/jupyter"    CACHE STRING "Jupyter data directory")

    # Install xlua Jupyter kernelspec
    set(XLUA_KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
    install(DIRECTORY ${XLUA_KERNELSPEC_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE)


    # lua packages (json and ilua)
    set(XLUA_PACKAGES_DIR "asset_dir/lua_packages"    CACHE STRING "Jupyter data directory")
    install(DIRECTORY ${XLUA_PACKAGES_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}/kernels/xlua
            PATTERN "*.in" EXCLUDE)

    # Extra path for installing Jupyter kernelspec
    if (XEXTRA_JUPYTER_DATA_DIR)
        install(DIRECTORY ${XLUA_KERNELSPEC_DIR}
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

# Configure 'xeus-luaConfig.cmake' for an install tree
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

if (XLUA_BUILD_SHARED)
    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION ${XEUS_LUA_CMAKECONFIG_INSTALL_DIR})
endif ()
