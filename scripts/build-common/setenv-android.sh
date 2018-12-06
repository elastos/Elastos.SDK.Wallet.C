#!/bin/bash

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
source "$CURRENT_DIR/base.sh";

SYSTEM_NAME="Android"
SYSTEM_ARCH="arm arm64"
BUILD_DIR="$BUILD_BASE_DIR/$SYSTEM_NAME";
TARBALL_DIR="$BUILD_BASE_DIR/tarball";
OUTPUT_DIR="$BUILD_ROOT_DIR/$SYSTEM_NAME";
mkdir -p "$TARBALL_DIR";

if [ -z "$ANDROID_NDK_HOME" ]; then
	echo "Please set your ANDROID_NDK_HOME environment variable first"
	exit 1
fi

if [[ "$ANDROID_NDK_HOME" == .* ]]; then
	echo "Please set your ANDROID_NDK_HOME to an absolute path"
	exit 1
fi

SDK=19
ARCH=arm

#Configure toolchain
ANDROID_TOOLCHAIN="$BUILD_DIR/toolchain.$ARCH";
if [ ! -e "$BUILD_DIR/.toolchain.$ARCH" ]; then
	rm -rf "$ANDROID_TOOLCHAIN"
	$ANDROID_NDK_HOME/build/tools/make-standalone-toolchain.sh --arch=$ARCH --platform=android-$SDK --stl=libc++ --install-dir="$ANDROID_TOOLCHAIN" --toolchain=arm-linux-androideabi-4.9

	touch "$BUILD_DIR/.toolchain.$ARCH";
fi

export PATH="$ANDROID_TOOLCHAIN/bin:$PATH"
export CFLAGS="-D__ANDROID_API__=$SDK"

