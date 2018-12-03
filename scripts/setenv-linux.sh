#!/bin/bash

BUILD_DIR="$BUILD_BASE_DIR/linux";
TARBALL_DIR="$BUILD_BASE_DIR/tarball";
OUTPUT_DIR="$BUILD_BASE_DIR/output/linux";
mkdir -p "$TARBALL_DIR";

if [ -z $(which clang) ]; then
	echo "You have to install clang first"
	exit 1
fi

ARCH=$(uname -m)
