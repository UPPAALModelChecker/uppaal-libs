#!/usr/bin/env bash

set -e

HOST=$(uname -s)

case $HOST in
  Linux|MINGW64*)
    CORES=$(nproc)
    ;;
  Darwin)
    CORES=$(sysctl -n hw.ncpu)
    ;;
  *)
    echo "Unknown hosting platform: $HOST"
    exit 1
esac

targets="$@"

if [ -z "$targets" ]; then
  echo "Expected a target platform as an argument. The following are supported:"
  for target in $(ls cmake/toolchain) ; do
    t=${target%%.cmake}
    echo -e "\t$t"
  done
  echo
  case $HOST in
  Linux|MINGW64*)
    targets=""
    if [ -n "$(command -v c++)" ]; then
      targets="$targets linux64"
      if echo 'int main(){}' | c++ -m32 -xc++ - -o /dev/null 2>&1 ; then
        targets="$targets linux32"
      fi
    fi
    for COMPILER in g++-16 g++-15 g++-14 g++-13 g++-12 g++-11 g++-10 ; do
      if [ -n "$(command -v "$COMPILER")" ]; then
        targets="$targets linux64-${COMPILER/++-/cc}"
        if echo 'int main(){}' | ${COMPILER} -m32 -xc++ - -o /dev/null 2>&1 ; then
          targets="$targets linux32-${COMPILER/++-/cc}"
        fi
      fi
    done
    if [ -n "$(command -v x86_64-w64-mingw32-g++)" ]; then
      targets="$targets x86_64-w64-mingw32"
    fi
    if [ -n "$(command -v i686-w64-mingw32-g++)" ]; then
      targets="$targets i686-w64-mingw32"
    fi
    ;;
  Darwin)
    targets=""
    if [ -n "$(command -v c++)" ]; then
      targets="$targets macos64"
    fi
    for COMPILER in g++-16 g++-15 g++-14 g++-13 g++-12 g++-11 g++-10 ; do
      if [ -n "$(command -v "$COMPILER")" ]; then
        targets="$targets macos64-brew-${COMPILER}"
      fi
    done
    for COMPILER in g++-mp-16 g++-mp-15 g++-mp-14 g++-mp-13 g++-mp-12 g++-mp-11 g++-mp-10 ; do
      if [ -n "$(command -v "$COMPILER")" ]; then
        targets="$targets macos64-ports-${COMPILER}"
      fi
    done
    ;;
  *)
    echo "Unknown hosting platform"
    exit 1
  esac
  echo -e "Guessing target platform(s):\n\t$targets"
fi

if [ -z "$CMAKE_GENERATOR" ]; then
  if [ -n "$(command -v ninja)" ] ; then
    export CMAKE_GENERATOR=Ninja
  else
    echo "Ninja build system is recommended, please install it."
  fi
fi
if [ -z "$CMAKE_BUILD_PARALLEL_LEVEL" ]; then
  export CMAKE_BUILD_PARALLEL_LEVEL=$CORES
fi
if [ -z "$CTEST_PARALLEL_LEVEL" ]; then
  export CTEST_PARALLEL_LEVEL=$CORES
fi
if [ -z "$CTEST_TEST_LOAD" ]; then
  export CTEST_TEST_LOAD=$CORES
fi
if [ -z "$CTEST_OUTPUT_ON_FAILURE" ]; then
  export CTEST_OUTPUT_ON_FAILURE=1
fi

for target in $targets ; do
  unset CMAKE_TOOLCHAIN_FILE
  unset SANITIZE
  if [ ! -r $PWD/cmake/toolchain/${target}.cmake ]; then
    echo "The toolchain file does not exist: $PWD/cmake/toolchain/${target}.cmake"
    exit 1
  else
    export CMAKE_TOOLCHAIN_FILE="$PWD/cmake/toolchain/${target}.cmake"
  fi
  case $target in
  linux*)
    extension=so
    SANITIZE="-DSSP=ON -DUBSAN=ON -DASAN=ON"
    ;;
  macos64-brew-gcc*)
    extension=dylib
    SANITIZE="-DSSP=ON"
    ;;
  macos*)
    extension=dylib
    SANITIZE="-DSSP=ON -DUBSAN=ON -DASAN=ON"
    ;;
  i686*mingw32)
    extension=dll
    SANITIZE="-DSSP=ON"
    export WINEARCH=win32
    export WINEPREFIX="${HOME}/.wine32"
    if [ ! -d "$WINEPREFIX" ]; then wine cmd /c echo "Prepared WINEPREFIX"; fi
    libgcc_path=$($target-g++ --print-file-name=libgcc_s_dw2-1.dll)
    libgcc_path=$(realpath "$libgcc_path")
    libgcc_path=$(dirname "$libgcc_path")
    libwinpthread_path=$($target-g++ --print-file-name=libwinpthread-1.dll)
    libwinpthread_path=$(realpath "$libwinpthread_path")
    libwinpthread_path=$(dirname "$libwinpthread_path")
    export WINEPATH="${libwinpthread_path};${libgcc_path}"
    echo "WINEARCH=$WINEARCH"
    echo "WINEPATH=$WINEPATH"
    echo "WINEPREFIX=$WINEPREFIX"
    ;;
  x86_64*mingw32)
    extension=dll
    SANITIZE="-DSSP=ON"
    export WINEARCH=win64
    export WINEPREFIX="${HOME}/.wine64"
    if [ ! -d "$WINEPREFIX" ]; then wine cmd /c echo "Prepared WINEPREFIX"; fi
    libgcc_path=$($target-g++ --print-file-name=libgcc_s_seh-1.dll)
    libgcc_path=$(realpath "$libgcc_path")
    libgcc_path=$(dirname "$libgcc_path")
    libwinpthread_path=$($target-g++ --print-file-name=libwinpthread-1.dll)
    libwinpthread_path=$(realpath "$libwinpthread_path")
    libwinpthread_path=$(dirname "$libwinpthread_path")
    export WINEPATH="${libwinpthread_path};${libgcc_path}"
    echo "WINEPREFIX=$WINEPREFIX"
    echo "WINEPATH=$WINEPATH"
    echo "WINEARCH=$WINEARCH"
    ;;
  *)
    echo "Unknown target platform: $target"
    exit 1
  esac
  export CMAKE_BUILD_TYPE=Debug
  BUILD_DIR=build-$target-$CMAKE_BUILD_TYPE
  echo "Configuring debug build for $target"
  echo "  CMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE"
  cmake -S . -B "$BUILD_DIR" $SANITIZE
  echo "Building debug configuration for $target"
  cmake --build "$BUILD_DIR" --config $CMAKE_BUILD_TYPE
  echo "Testing debug configuration for $target"
  ctest --test-dir "$BUILD_DIR" -C $CMAKE_BUILD_TYPE
  ## Create a link to it:
	ln -snf "${BUILD_DIR}/src/libtable.${extension}" "libtable-dbg.${extension}"

  export CMAKE_BUILD_TYPE=Release
  BUILD_DIR=build-$target-$CMAKE_BUILD_TYPE
  echo "Configuring optimized release build for $target"
  cmake -S . -B "$BUILD_DIR"
  echo "Building optimized release configuration for $target"
  cmake --build "$BUILD_DIR" --config $CMAKE_BUILD_TYPE
  echo "Testing optimized release configuration for $target"
  ctest --test-dir "$BUILD_DIR" -C $CMAKE_BUILD_TYPE
  ## Create a link to it:
	ln -snf "${BUILD_DIR}/src/libtable.${extension}" "libtable.${extension}"
done
