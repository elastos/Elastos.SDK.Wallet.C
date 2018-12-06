#!/bin/bash

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
source "$CURRENT_DIR/base.sh";

SYSTEM_NAME=$(uname -s)
SYSTEM_ABIS=(x86_64)
BUILD_DIR="$BUILD_BASE_DIR/$SYSTEM_NAME/$TARGET_ARCH";
TARBALL_DIR="$BUILD_BASE_DIR/tarball";
OUTPUT_DIR="$BUILD_ROOT_DIR/$SYSTEM_NAME/$TARGET_ARCH";
mkdir -p "$TARBALL_DIR";

if [ -z $(which clang) ]; then
	echo "You have to install clang first"
	exit 1
fi
