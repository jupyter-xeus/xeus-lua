.. Copyright (c) 2017, Martin Renou, Johan Mabille, Sylvain Corlay, and
   Wolf Vollprecht

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Build and configuration
=======================

General Build Options
---------------------

Building the xeus-lua library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``xeus-lua`` build supports the following options:

- ``XLUA_BUILD_SHARED``: Build the xeus-lua shared library. **Enabled by default**.
- ``XLUA_BUILD_STATIC``: Build the xeus-lua static library. **Enabled by default**.

Xeus-lua must link with xeus dynamically or statically.

- ``XLUA_USE_SHARED_XEUS``: Link with the xeus shared library (instead of the static library). **Enabled by default**.

Building the kernel
~~~~~~~~~~~~~~~~~~~

The package includes an option for producing a kernel: an executable ``xlua`` 

- ``XLUA_BUILD_XLUA_EXECUTABLE``: Build the xlua executable. **Enabled by default**.

The executable can use a shared or a static library:

- ``XLUA_USE_SHARED_XEUS_LUA``: Link xlua with the xeus-lua shared library. **Enabled by default**.

If ``XLUA_USE_SHARED_XEUS_LUA`` is disabled, xlua will be linked statically with xeus-lua.

