# Uppaal Libraries
![build](https://github.com/UPPAALModelChecker/uppaal-libs/actions/workflows/build.yml/badge.svg?branch=main)

Generic examples of dynamically loaded libraries for Uppaal:

* `libtable` read, manipulate and write table data via CSV files.

## Requirements

The library can be imported into Uppaal models starting from **Uppaal version 4.1.20** for Linux and **Uppaal Stratego 11** for Windows and macOS.
Either download the library files from the [releases](https://github.com/UPPAALModelChecker/uppaal-libs/releases) or checkout the source and compile on your own.

### Linux
Install:
* `git` to checkout the repository and dependencies (even if you download the source yourself).
* `cmake` version 3.15 or later.
* C++ compiler supporting C++17: look for `c++`, `g++` (version 9 or later), `clang++`.
* C library (glibc) the same or newer version than in Uppaal distribution.
* `ninja` (optional) for better compiler error messages.
e.g.:
```shell
sudo apt install git cmake g++ ninja-build
```
### Linux Cross-Compile for Windows
In addition, install the following:
* `x86_64-w64-mingw32-g++` for 64bit binaries
* `i686-w64-mingw32-g++` for 32bit binaries
* `wine` and `binfmt-support` for testing
e.g.:
```shell
sudo apt install g++-mingw-w64-x86-64-posix wine binfmt-support wine-binfmt
```
`wine` may need to know where `libwinpthread-1.dll`, `libgcc_s_seh-1.dll`, `libstdc++-6.dll` and other system libraries are.
You can either copy them into the directory next to the binaries (very tedious) or add them to `wine` `PATH`.
Here are the instructions:
- Print the locations of MinGW system libraries:
```shell
realpath $(x86_64-w64-mingw32-g++ --print-file-name=libwinpthread-1.dll)
realpath $(x86_64-w64-mingw32-g++ --print-file-name=libgcc_s_seh-1.dll)
realpath $(x86_64-w64-mingw32-g++ --print-file-name=libstdc++-6.dll)
```
- Add the paths to `PATH` variable to your `wine` registry file `$HOME/.wine/system.reg` section `[System\\CurrentControlSet\\Control\\Session Manager\\Environment]`, where `Z:` drive stands for root file system mount. For example:
```
"PATH"="Z:\\usr\\x86_64-w64-mingw32\\lib;Z:\\usr\\lib\\gcc\\x86_64-w64-mingw32\\12-posix"
```

### macOS
Install the following:
* `git` to checkout the repository and dependencies (even if you download the source yourself).
* `xcode` from App Store. Make sure to run at least once (which installs command line tools).
* `cmake` version 3.15 or later.
* Native `c++` or `g++`, or `clang++` from [HomeBrew](https://brew.sh/) or [MacPorts](https://www.macports.org/).

### Windows
Install the following:
* [git](https://git-scm.com/download/win) to check out the repository and dependencies (even if you download the source yourself).
* [cmake](https://cmake.org/download/) version 3.15 or newer to configure the build system
* [Visual Studio](https://visualstudio.microsoft.com/vs/community/) compiler, select `Desktop development with C++`


## Checkout
```shell
git clone https://github.com/UPPAALModelChecker/uppaal-libs.git
```

## Compile

### Unix
Run `compile.sh` with `$target` arguments where `$target` corresponds to a file in [toolchain](toolchain) folder.

For example, use [toolchain/linux64.cmake](toolchain/linux64.cmake) toolchain to build for 64bit Linux:
```shell
./compile.sh linux64
```
This will produce a debug build (with logging into `error.log`) in [build-linux64-debug](build-linux64-debug) and optimized release build without logging in [build-linux64-release](build-linux64-release).

Look for `libtable.so` in [build-linux64-release/src](build-linux64-release/src).

Similarly, cross-compile for Windows into [build-x86_64-mingw32-debug](build-x86_64-mingw32-debug) and [build-x86_64-mingw32-release](build-x86_64-mingw32-release):
```shell
./compile.sh x86_64-w64-mingw32
```
Look for `libtable.dll` in [build-x86_64-mingw32-release/src](build-x86_64-mingw32-release/src).


Build for `macOS` using `g++-12` from `brew` into [build-macos64-brew-gcc12-debug](build-macos64-brew-gcc12-debug) and [build-macos64-brew-gcc12-release](build-macos64-brew-gcc12-release):
```shell
./compile.sh macos64-brew-gcc12
```
Look for `libtable.dylib` in [build-macos64-brew-gcc12-release/src](build-macos64-brew-gcc12-release/src).

### Windows
Just launch `compile.bat` to compile, which will open the folder with binary files upon success.

Look for `libtable.dll` and `libtable-dbg.dll` in the project folder.

## Usage

* Put the library files (`libtable.so` on Linux, `libtable.dylib` on macOS, `libtable.dll` on Windows) next to your models files.
* Import the library into Uppaal model:
```c
import "/absolute/path/to/LIBRARY-FILE.EXT" {
    /** resets the error log path (default is errors.log in current directory) */
    int set_error_path(const string& path);
    /** create a new table, fill it with integer value and return its id: */
    int table_new_int(int rows, int cols, int value);
    /** create a new table, fill it with double value and return its id: */
    int table_new_double(int rows, int cols, double value);
    /** read the table from the csv file and return its id: */
    int table_read_csv(const string& filename, int skip_lines);
    /** write the table to csv file and return the number of rows: */
    int table_write_csv(int id, const string& filename);
    /** create a new table copy and return its id: */
    int table_copy(int id);
    /** release the resources associated with the table and return its id: */
    int table_clear(int id);
    /** resize the table with the given dimensions: */
    int table_resize_int(int id, int rows, int cols, int value);
    /** return the number of rows in the table: */
    int table_rows(int id);
    /** return the number of columns in the table: */
    int table_cols(int id);
    /** read an integer value at row:col, counted from 0: */
    int read_int(int id, int row, int col);
    /** read a double value at row:col, counted from 0: */
    double read_double(int id, int row, int col);
    /** return interpolated look up value from row with key in key_column (sorted in ascending order) from value_column */
    double interpolate(int id, double key, int key_column, int value_column);
    /** write an integer value at row:col */
    void write_int(int id, int row, int col, int value);
    /** write a double value at row:col */
    void write_double(int id, int row, int col, double value);
};
```
* Call the library to load the CSV file into a table, read the size and entries:
```c
const int PATH = set_error_path("path/to/errors.log"); // otherwise expect errors.log in current path
const int TID = table_read_csv("path/to/table.csv");
const int ROWS = table_rows(TID);
const int COLS = table_cols(TID);
int value_at_row0_col0 = read_int(TID, 0, 0);
int value_at_row1_col2 = read_int(TID, 1, 2);

typedef int[0,COLS-1] col_t;
typedef int[0,ROWS-1] row_t;
typedef int record_t[col_t];
typedef record_t table_t[row_t];

table_t table;
void read_table() {
    for (r:row_t)
        for (c:col_t)
            table[r][c] = read_int(TID, r, c);
}
```
* As the API implies, it is also possible to create, copy, resize, modify and write table data, but the modifications must be used with extreme care as they are **not side-effect-free**.

* A correct use is to not modify the table at all (**read-only** access is **side-effect-free**).

* Consider the following **bad modification** with two edges emanating form the initial location: the first edge modifies the table and the second just reads -- the model-checker may execute the first (and modify the table), then come back to explore the second edge, however the table modification is visible for the second edge because the engine could not reset the data in the external library.

* A possibly correct, but very tedious and error-prone scenario with modification is to create a separate data for each new state and then refer back to the same data when the state changes back, i.e. maintain one-to-one correspondence between system state and the data in the external library. For example, a new edge update may create/update a new table/row in the table identified by some variable value and then the same table/row should be used when the system returns to the exact same state (which can be indexed by that variable value).

## Potential issues

### Linking Issues: symbol not found, library not found

To find out the list of libraries loaded by an executable object (binary or library), use `ldd`, for example:
```sh
ldd verifyta.bin
ldd libstrategy.so
```
Note that entries like `linux-vdso.so.1` and `/lib64/ld-linux-x86-64.so.2` are overriden by the `verifyta` shell script to use the shipped library loader (`ld-linux.so`), and thus are not indicative when using `ldd`. Uppaal binaries are self-contained, i.e. it ships with all the libraries it requires. If your library needs something more, then it must be compatible with those libraries, most notably `libc`, see section below.

One can also use [`strace`](https://man7.org/linux/man-pages/man1/strace.1.html) utility on the executable to see all the system calls it makes and thus discover all the files (including shared libraries) it tries to access and thus find out which library requires a specific symbol that the executable errs on.

If some symbol is not found (symbol represents either a shared variable or a function loaded from a shared library), then one must check the library versions. Various versions may ship different sets of symbols. Normally newer library versions will also ship old/obsolete symbols to guarantee backward compatibility, therefore a newer library version is prefered.

Meanwhile executables linked against newer library versions may not work with the old library version as they do not have the newer symbols. Therefore, it is prefered that the executables are linked against older libraries. Older libraries may have performance or even critical issues, thus the balance between library versions is very subtle.

One may customize the search path the [dynamic library loader](https://man7.org/linux/man-pages/man8/ld-linux.8.html) is looking for library files during execution by setting the `LD_LIBRARY_PATH` environment variable. Also inspect `LD_RUN_PATH`, `LD_PRELOAD` variables in your environment: normally they are not used, but some setups may have already customized them and thus cause loading conflicting versions of the libraries.

The search path can be customized during compilation using `-rpath` argument to the [dynamic library linker](https://man7.org/linux/man-pages/man1/ld.1.html).

### Compatibility with `libc`

**All libraries using `libc` must use a compatible version of `libc` shipped with Uppaal**, which means the same or newer version. Newer versions are usually backward compatible with older versions (with some rare exceptions), so one is encouraged to get the [latest and greatest](https://www.gnu.org/software/libc/), meanwhile the rest of the world is forced to be conservative and provide the old ones (which have highest compatibility).

To find out the `libc` library version, run it with `--version` argument, for example:
```sh
./libc.so.6 --version
```
To find out the location of the system `libc` library, ask the compiler, for example:
```sh
g++ --print-file-name=libc.so.6
```
Which outputs something like this:
```sh
/lib/x86_64-linux-gnu/libc.so.6
```
Therefore the `libc` provided by the host system can print its version information by running:
```sh
/lib/x86_64-linux-gnu/libc.so.6 --version
```

If the `libc` provided by the host system is incompatible with the one shipped by Uppaal, then we need to compile our own `libc` and then build our library using it instead of the one provided by the host system.

For example, here are the steps to download, [compile](https://www.gnu.org/software/libc/manual/html_node/Configuring-and-compiling.html) and install `libc` version 2.31 into `$PREFIX` target path:

```sh
PREFIX=$HOME/.local
wget http://mirrors.dotsrc.org/gnu/libc/glibc-2.31.tar.bz2
tar xf glibc-2.31.tar.bz2
mkdir glibc-build
cd glibc-build
../glibc-2.31/configure CC=gcc-8 CFLAGS=-O3 --prefix=$PREFIX
make -j2
make install
```

Here we have set the installation target path `$PREFIX` to a "standard" location `$HOME/.local` for user programs, libraries and headers, but feel free to customize it. It has a similar structure to the system locations `/usr`, `/usr/local` and `/opt/local` except these are **not recommended** as they require root and **can mess up** the system's packaging system, so **do not use the system locations**, use some user directory instead.

Then all the library sources need to be compiled against the `libc` in `$PREFIX` by adding the following options to the compiler before any source files are given `-I$PREFIX/include` and add the option `-L$PREFIX/lib` before any library name to the linker (which is often the same `gcc`/`g++` thus can be combined).

Alternatively, one can simply specify the prefix to the automated build system (if one is used) which will do the above automatically:
* For `configure` add `--prefix=$PREFIX`
* For `cmake` add `-DCMAKE_PREFIX_PATH=$PREFIX`


### CMake is too old

[Download, compile and install a newer cmake](https://cmake.org/install/) into `PREFIX=$HOME/.local` and add it to your path `PATH=$HOME/.local/bin:$PATH`.


### Compiler is too old

Download, [compile and install](https://gcc.gnu.org/install/index.html) a newer compiler into the same `PREFIX=$HOME/.local` and add it to your path `PATH=$HOME/.local/bin:$PATH`. It can take a while to compile it, but it is doable and gets easier with each newer version.
