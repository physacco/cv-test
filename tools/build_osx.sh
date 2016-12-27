#!/bin/sh

set -e

SCRIPT_DIR=$(dirname "$0")
PROJECT_DIR=$(cd "$SCRIPT_DIR"; cd ..; pwd)
BUILD_DIR=build/osx

cd "$PROJECT_DIR"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# use brew-installed opencv3
export CMAKE_PREFIX_PATH=/usr/local/opt/opencv3
cmake ${PROJECT_DIR}

make -j $(sysctl -n hw.ncpu)
