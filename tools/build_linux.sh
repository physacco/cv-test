#!/bin/sh

set -e

SCRIPT_DIR=$(dirname "$0")
PROJECT_DIR=$(cd "$SCRIPT_DIR"; cd ..; pwd)
BUILD_DIR=build

cd "$PROJECT_DIR"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake ..

make -j $(nproc)
