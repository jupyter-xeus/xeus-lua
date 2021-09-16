.. Copyright (c) 2021, Thorsten Beier
   Wolf Vollprecht

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

.. raw:: html

   <style>
   .rst-content .section>img {
       width: 30px;
       margin-bottom: 0;
       margin-top: 0;
       margin-right: 15px;
       margin-left: 15px;
       float: left;
   }
   </style>

Installation
============

With mamba (or conda)
---------------------

`xeus-lua` has been packaged for the mamba (or conda) package manager.

To ensure that the installation works, it is preferable to install `xeus-lua` in a fresh environment.
It is also needed to use a miniforge_ or miniconda_ installation because with the full anaconda_ you may have a conflict with
the `zeromq` library which is already installed in the anaconda distribution.


The safest usage is to create an environment named `xeus-lua`

.. code::

    mamba create -n xeus-lua
    source activate xeus-lua

Then you can install in this freshly created environment `xeus-lua` and its dependencies

.. code::

    mamba install xeus-lua jupyterlab -c conda-forge


From Source
-----------

You can install ``xeus-lua`` from source with cmake. This requires that you have all the dependencies installed in the same prefix.

.. code::

    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/path/to/prefix ..
    make install

On Windows platforms, from the source directory:

.. code::

    mkdir build
    cd build
    cmake -G "NMake Makefiles" -DCMAKE_INSTALL_PREFIX=/path/to/prefix ..
    nmake
    nmake install

Installing the Kernel Spec
==========================

When installing xeus-lua in a given installation prefix, the corresponding Jupyter kernelspecs are installed in the same environment and are automatically picked up by Jupyter if it is installed in the same prefix. 

However, if Jupyter is installed in a different location, it will not pick up the new kernel. The xeus-lua can be registered with the following command:

.. code::

   jupyter kernelspec install PREFIX/share/jupyter/kernels/xlua --sys-prefix

For more information on the ``jupyter kernelspec`` command, please consult the ``jupyter_client`` documentation.

.. _miniforge: https://github.com/conda-forge/miniforge#mambaforge
.. _miniconda: https://conda.io/miniconda.html
.. _anaconda: https://www.anaconda.com
.. _JupyterLab: https://jupyterlab.readthedocs.io
