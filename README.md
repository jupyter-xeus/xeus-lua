# ![xeus-lua](docs/source/xeus-lua.svg)

[![Azure Pipelines](https://dev.azure.com/derthorstenbeier/xeus-lua/_apis/build/status/jupyter-xeus.xeus-lua?branchName=main)](https://dev.azure.com/derthorstenbeier/xeus-lua/_build/latest?definitionId=12&branchName=main)
[![Appveyor](https://ci.appveyor.com/api/projects/status/de8van3pfjrr4b0e?svg=true)](https://ci.appveyor.com/project/DerThorsten/xeus-lua-17on4)

[![Documentation Status](http://readthedocs.org/projects/xeus-lua/badge/?version=latest)](https://xeus-lua.readthedocs.io/en/latest/?badge=latest)
<!-- [![Binder](https://mybinder.org/badge_logo.svg)](https://mybinder.org/v2/gh/DerThorsten/xeus-lua/main?urlpath=/lab/tree/notebooks/xeus-lua.ipynb) -->
[![Join the Gitter Chat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/QuantStack/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

`xeus-lua` is a Jupyter kernel for Lua based on the native implementation of the
Jupyter protocol [xeus](https://github.com/jupyter-xeus/xeus).

## Disclaimer

This is still under development and in progress. 
Things might change.
The readme, conda integration and examples are still in development.

## Installation

xeus-lua has **not yet** been packaged for the mamba (or conda) package manager, but will be soon.

To ensure that the installation works, it is preferable to install `xeus-lua` in a
fresh environment. It is also needed to use a
[miniforge](https://github.com/conda-forge/miniforge#mambaforge) or
[miniconda](https://conda.io/miniconda.html) installation because with the full
[anaconda](https://www.anaconda.com/) you may have a conflict with the `zeromq` library
which is already installed in the anaconda distribution.

The safest usage is to create an environment named `xeus-lua`

```bash
mamba create -n xeus-lua
source activate xeus-lua
```

### Installing from conda-forge

not yet

### Installing from source

Or you can install it from the sources, you will first need to install dependencies

```bash
mamba install cmake xeus xwidgets nlohmann_json cppzmq xtl lua  jupyterlab -c conda-forge
```

Then you can compile the sources (replace `$CONDA_PREFIX` with a custom installation
prefix if need be)

```bash
mkdir build && cd build
cmake .. -D CMAKE_PREFIX_PATH=$CONDA_PREFIX -D CMAKE_INSTALL_PREFIX=$CONDA_PREFIX -D CMAKE_INSTALL_LIBDIR=lib
make && make install
```

## Trying it online

To try out xeus-lua interactively in your web browser, just click on the binder link:

[![Binder](binder-logo.svg)](https://mybinder.org/v2/gh/jupyter-xeus/xeus-lua/main?urlpath=/lab/tree/notebooks/game_of_life.ipynb)

## Usage

Launch the Jupyter notebook with `jupyter notebook` or Jupyter lab with `jupyter lab`
and launch a new Python notebook by selecting the **xlua** kernel.

**Code execution and variable display**:

TODO

**Output streams**:

TODO

**Input streams**:

TODO

**Error handling**:

TODO

**Inspect**:

TODO

**Code completion**:

TODO

**Rich display**:

TODO

**And of course widgets**:

TODO

## Documentation

To get started with using `xeus-lua`, check out the full documentation (once its available :-) )

http://xeus-lua.readthedocs.io

## Why a lua kernel in 2021?

- 

## What are the advantages of using xeus-lua over other lua kernels?

-  todo

## Dependencies

`xeus-lua` depends on

- [xeus](https://github.com/jupyter-xeus/xeus)
- [xtl](https://github.com/xtensor-stack/xtl)
- [xwidgets](https://github.com/jupyter-xeus/xwidgets)
- [lua](https://www.lua.org/)

| `xeus-lua`    | `xeus`        | `xwidgets`    | `xtl`        | `cppzmq` | `nlohmann_json` | `lua`        | 
| ------------- | ------------- | ------------- | ------------ | -------- | --------------- | ------------ | 
| master        | >=1.0.3,<0.26 | >=0.25.1      | >=0.7.0,<0.8 | ~4.4.1   | >=3.6.1,<4.0    | >=5.2.0      | 
| 0.3.2         | >=1.0.3,<0.26 | >=0.25.1      | >=0.7.0,<0.8 | ~4.4.1   | >=3.6.1,<4.0    | >=5.2.0      | 


## Contributing

See [CONTRIBUTING.md](./CONTRIBUTING.md) to know how to contribute and set up a
development environment.

## License

We use a shared copyright model that enables all contributors to maintain the copyright
on their contributions.

This software is licensed under the BSD-3-Clause license. See the [LICENSE](LICENSE)
file for details.
