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
  for target in $(ls toolchain) ; do
    t=${target%%.cmake}
    echo -e "\t$t"
  done
  echo
  case $HOST in
  Linux|MINGW64*)
    targets=""
    if [ -n "$(command -v c++)" ]; then
      targets="$targets linux64"
      if [ -n "$(c++ -m32 --print-file-name=libstdc++.so)" ]; then
        targets="$targets linux32"
      fi
    fi
    if [ -n "$(command -v g++-10)" ]; then
      targets="$targets linux64-gcc10"
      if [ -n "$(g++-10 -m32 --print-file-name=libstdc++.so)" ]; then
        targets="$targets linux32-gcc10"
      fi
    fi
    if [ -n "$(command -v g++-11)" ]; then
      targets="$targets linux64-gcc11"
      if [ -n "$(g++-11 -m32 --print-file-name=libstdc++.so)" ]; then
        targets="$targets linux32-gcc11"
      fi
    fi
    if [ -n "$(command -v g++-12)" ]; then
      targets="$targets linux64-gcc12"
      if [ -n "$(g++-12 -m32 --print-file-name=libstdc++.so)" ]; then
        targets="$targets linux32-gcc12"
      fi
    fi
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
    if [ -n "$(command -v g++-10)" ]; then
      targets="$targets macos64-brew-gcc10"
    fi
    if [ -n "$(command -v g++-11)" ]; then
      targets="$targets macos64-brew-gcc11"
    fi
    if [ -n "$(command -v g++-12)" ]; then
      targets="$targets macos64-brew-gcc12"
    fi
    if [ -n "$(command -v g++-mp-10)" ]; then
      targets="$targets macos64-ports-gcc10"
    fi
    if [ -n "$(command -v g++-mp-11)" ]; then
      targets="$targets macos64-ports-gcc11"
    fi
    if [ -n "$(command -v g++-mp-12)" ]; then
      targets="$targets macos64-ports-gcc12"
    fi
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

for target in $targets ; do
  if [ ! -r $PWD/toolchain/${target}.cmake ]; then
    echo "The toolchain file does not exist: $PWD/toolchain/${target}.cmake"
    exit 1
  else
    TOOLCHAIN="-DCMAKE_TOOLCHAIN_FILE=$PWD/toolchain/${target}.cmake"
  fi
  case $target in
  linux*)
    extension=so
    SANITIZE="-DUBSAN=ON -DASAN=ON"
    ;;
  macos*)
    extension=dylib
    SANITIZE="-DUBSAN=ON -DASAN=ON"
    ;;
  i686*mingw32)
    extension=dll
    SANITIZE=""
    libgcc_path=$($target-g++ --print-file-name=libgcc_s_dw2-1.dll)
    libgcc_path=$(realpath "$libgcc_path")
    libgcc_path=$(dirname "$libgcc_path")
    libwinpthread_path=$($target-g++ --print-file-name=libwinpthread-1.dll)
    libwinpthread_path=$(realpath "$libwinpthread_path")
    libwinpthread_path=$(dirname "$libwinpthread_path")
    export WINEPATH="$libwinpthread_path;$libgcc_path"
    ;;
  x86_64*mingw32)
    extension=dll
    SANITIZE=""
    ;;
  *)
    echo "Unknown target platform: $target"
    exit 1
  esac
  BUILD_DIR=build-$target-debug
  echo "Configuring debug build for $target"
  cmake $TOOLCHAIN -DCMAKE_BUILD_TYPE=Debug $SANITIZE -S . -B "$BUILD_DIR"
  echo "Building debug configuration for $target"
  cmake --build "$BUILD_DIR"
  echo "Testing debug configuration for $target"
  (cd "$BUILD_DIR" ; ctest --output-on-failure)
  ## Create a link to it:
  if [ ! -e libtable-dbg.${extension} ]; then
	  ln -s "$BUILD_DIR"/src/libtable.${extension} libtable-dbg.${extension}
  fi

  BUILD_DIR=build-$target-release
  echo "Configuring optimized release build for $target"
  cmake $TOOLCHAIN -DCMAKE_BUILD_TYPE=Release -S . -B "$BUILD_DIR"
  echo "Building optimized release configuration for $target"
  cmake --build "$BUILD_DIR"
  echo "Testing optimized release configuration for $target"
  (cd "$BUILD_DIR" ; ctest --output-on-failure)
  ## Create a link to it:
  if [ ! -e libtable.${extension} ]; then
	  ln -s "$BUILD_DIR"/src/libtable.${extension} libtable.${extension}
  fi
done
