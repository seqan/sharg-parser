# Quick Setup (using CMake) {#setup}

<!--
SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
SPDX-License-Identifier: CC-BY-4.0
-->

<b>Learning Objective:</b><br>
In this short guide you will learn how to set up Sharg and how to compile a small example to test whether everything
works.

\tutorial_head{Easy, 30 Minutes, No prerequisites, }

[TOC]

# Software
Requirements:
  - gcc >= 11 or clang >= 17
  - cmake >= 3.16
  - git

## Installing a Compiler

Sharg requires GCC >= 11 or LLVM/Clang >= 17. Presently, VisualStudio/MSVC is **not supported**.
We'll offer a brief tutorial on compiler updates through common package managers and suggest some web-based
alternatives. For further details, consult your OS documentation.

<div class="tabbed">
- <b class="tab-title">Ubuntu</b>
```bash
# Installs gcc-13
sudo add-apt-repository --no-update --yes ppa:ubuntu-toolchain-r/ppa
sudo add-apt-repository --no-update --yes ppa:ubuntu-toolchain-r/test
sudo apt-get update
sudo apt install g++-13
```
See https://apt.llvm.org/ if you prefer to use LLVM's Clang.
- <b class="tab-title">Conda</b>
To avoid interference with system packages, we recommend creating a new environment when using conda.
```bash
conda create -n conda_gcc_env -c conda-forge gcc_linux-64
conda activate conda_gcc_env
```
This will put GCC in a separate environment `conda_gcc_env` which can be activated via `conda activate conda_gcc_env`
and deactivated via `conda deactivate`.
- <b class="tab-title">Homebrew</b>
```bash
brew install gcc@13
brew install llvm@17
```
- <b class="tab-title">Macports</b>
```bash
sudo port install gcc13
sudo port install llvm-17
```
- <b class="tab-title">Windows</b>
The Windows Subsystem for Linux offers an easy way to run a Linux distribution under Windows.
Follow [Microsoft's setup guide](https://docs.microsoft.com/en-us/windows/wsl/about) to install WSL and then follow
the steps listed for Linux-based systems.
- <b class="tab-title">gitpod.io</b>
[gitpod.io](https://gitpod.io) allows you to edit, compile and run code from within your browser. The free version includes 50
hours of use per month, which is plenty for our tutorials. A GitHub account is required.
[Click here](https://gitpod.io/#https://github.com/seqan/sharg-parser) to open Sharg in gitpod.
- <b class="tab-title">GitHub Codespaces</b>
[GitHub Codespaces](https://github.com/codespaces) offer a service similar to gitpod, including a free monthly quota.
[Click here](https://codespaces.new/seqan/sharg-parser) to open Sharg in Codespaces.
Please note that you may have to manually clone the submodules by running `git submodule update --init`.

</div>

<br>
\note After installing, `g++ --version` should print the desired GCC version.
      If not, you may have to use, for example, `g++-11 --version` or even specify the full path to your compiler.

Similarly, you may need to install CMake and git, e.g. `sudo apt install cmake git`.

# Directory Structure
In this section we will use the `tree` command to show the directory structure. This program may not be installed
on your system. If so, you may wish to install it or verify the directory structure in other ways, e.g. by using
`ls -l`.

For this project, we recommend following directory layout:

```
tutorial
├── build
├── sharg-parser
└── source
```

To set these directories up you can follow this script (note the <b>\--recurse-submodules</b> when cloning Sharg):
```bash
mkdir tutorial
cd tutorial
mkdir build
mkdir source
git clone --recurse-submodules https://github.com/seqan/sharg-parser.git
```

The output of the command `tree -L 2` should now look like this:
```
.
├── build
├── sharg-parser
│   ├── CHANGELOG.md
│   ├── CMakeLists.txt
│   ├── ...
│   └── test
└── source
```

# Compiling and Running

To test whether everything works, we will now compile and run a small example.

First we create the file `hello_world.cpp` in the `source` directory with the following contents:

\include test/external_project/src/hello_world.cpp

To compile it, we first create a `CMakeLists.txt` file in the `source` directory:
<!-- Parsing the snippet like this to avoid verbatim includes of the snippet identifiers if we used nested snippets. -->
<!-- Snippet start -->
\dontinclude test/external_project/sharg_setup_tutorial/CMakeLists.txt
\until cmake_minimum_required
\skipline project
\until target_link_libraries
<!-- Snippet end -->

The directories should now look like this:

```
.
├── build
├── sharg-parser
│   ├── CHANGELOG.md
│   ├── CMakeLists.txt
│   ├── ...
│   └── test
└── source
    ├── CMakeLists.txt
    └── hello_world.cpp
```

Now we can switch to the directory `build` and run:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ../source
make
./hello_world
```

This should output a basic help page. Note that the build type is specified with `-DCMAKE_BUILD_TYPE=Release`.
Specifying `Release` enables an optimized build where no debug information is available. Release mode is therefore
suitable for the end user. Programs built using `-DCMAKE_BUILD_TYPE=Debug` will run slower, but also make the detection
of errors easier. `Debug` is suitable for contributors, and we recommend using it while working with our
[Tutorials](usergroup1.html).

\anchor remark_cmake_cxx_compiler
\remark Depending on the standard C++ compiler on your system, you may need to specify the compiler via
`-DCMAKE_CXX_COMPILER=`, for example:
```bash
cmake -DCMAKE_CXX_COMPILER=/path/to/executable/g++-13 ../source
```

# Adding a new source file to your project

If you create a new `cpp` file and want to compile it, you need to add another `add_executable` and
`target_link_libraries` directive to you `CMakeLists.txt`.
For example, after adding `another_program.cpp` your `CMakeLists.txt` may look like this:
\snippet test/external_project/sharg_setup_tutorial/CMakeLists.txt adding_files

# Encountered issues

* **Using conda's gcc package:** ``/usr/lib/x86_64-linux-gnu/libstdc++.so.6: version 'CXXABI_1.3.11' not found``<br>
  Try setting `LD_LIBRARY_PATH`:
  ```bash
  export LD_LIBRARY_PATH=<conda_install_path>/envs/conda_gcc_env/lib/
  ```
  where `<conda_install_path>` must be replaced by the path yo your conda installation.<br>
  Usually this corresponds to the path printed by `conda info --base` and may look similar to `/home/user/miniconda3/`.

* **Assembler not found:** `... could not understand flag m ...`<br>
  Try adding `/usr/bin` to your `PATH`:
  ```bash
  export PATH=/usr/bin:$PATH
  ```
  and run `cmake` again.
