#!/bin/bash

SDK=19

BUILD_DIR="$BUILD_BASE_DIR/android";
TARBALL_DIR="$BUILD_BASE_DIR/tarball";
OUTPUT_DIR="$BUILD_BASE_DIR/output/android";
mkdir -p "$TARBALL_DIR";

if [ -z "$ANDROID_NDK_HOME" ]; then
	echo "Please set your ANDROID_NDK_HOME environment variable first"
	exit 1
fi

if [[ "$ANDROID_NDK_HOME" == .* ]]; then
	echo "Please set your ANDROID_NDK_HOME to an absolute path"
	exit 1
fi

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

