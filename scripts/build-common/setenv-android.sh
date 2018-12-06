#!/bin/bash

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
source "$CURRENT_DIR/base.sh";

SYSTEM_NAME="Android"
SYSTEM_ABIS=(armeabi-v7a arm64-v8a x86_64)
BUILD_DIR="$BUILD_BASE_DIR/$SYSTEM_NAME/$TARGET_ARCH";
TARBALL_DIR="$BUILD_BASE_DIR/tarball";
OUTPUT_DIR="$BUILD_ROOT_DIR/$SYSTEM_NAME/$TARGET_ARCH";
mkdir -p "$TARBALL_DIR";

if [ -z "$ANDROID_NDK_HOME" ]; then
	echo "Please set your ANDROID_NDK_HOME environment variable first"
	exit 1
fi

if [[ "$ANDROID_NDK_HOME" == .* ]]; then
	echo "Please set your ANDROID_NDK_HOME to an absolute path"
	exit 1
fi

SDK_LIST=(19 21 21)
ARCH_LIST=(arm arm x86_64)
TOOLCHAIN_LIST=(arm-linux-android-clang3.6 aarch64-linux-android-clang3.6 x86_64-linux-android-clang3.6)
for idx in "${!SYSTEM_ABIS[@]}"; do
	if [[ "${SYSTEM_ABIS[$idx]}" = "${TARGET_ARCH}" ]]; then
		LIST_IDX=${idx}
		break;
	fi
done

#Configure toolchain
ANDROID_SDK=${SDK_LIST[$LIST_IDX]};
ANDROID_TOOLCHAIN="$BUILD_DIR/toolchain.$TARGET_ARCH";
if [ ! -e "$BUILD_DIR/.toolchain.$TARGET_ARCH" ]; then
	rm -rf "$ANDROID_TOOLCHAIN"
	$ANDROID_NDK_HOME/build/tools/make-standalone-toolchain.sh \
		--arch=${ARCH_LIST[$LIST_IDX]} \
		--platform=android-${SDK_LIST[$LIST_IDX]} \
		--toolchain=${TOOLCHAIN_LIST[$LIST_IDX]} \
		--stl=libc++ --install-dir="$ANDROID_TOOLCHAIN";

	touch "$BUILD_DIR/.toolchain.$TARGET_ARCH";
fi

export PATH="$ANDROID_TOOLCHAIN/bin:$PATH"
export CFLAGS="-D__ANDROID_API__=$ANDROID_SDK"

