#!/bin/bash

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
source "$CURRENT_DIR/base.sh";

SYSTEM_NAME="iOS"
SYSTEM_ARCH="arm arm64"
BUILD_DIR="$BUILD_BASE_DIR/$SYSTEM_NAME";
TARBALL_DIR="$BUILD_BASE_DIR/tarball";
OUTPUT_DIR="$BUILD_ROOT_DIR/$SYSTEM_NAME";
mkdir -p "$TARBALL_DIR";

XCODE="/Applications/Xcode.app/Contents/Developer"
if [ ! -d "$XCODE" ]; then
	echo "You have to install Xcode and the command line tools first"
	exit 1
fi

export CC="$XCODE/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
export IPHONEOS_DEPLOYMENT_TARGET="8"
ARCHS=(arm64)
HOSTS=(arm)
PLATFORMS=(iPhoneOS)
SDK=(iPhoneOS)

i=0
ARCH=${ARCHS[$i]}
export CFLAGS="-arch $ARCH -pipe -Os -gdwarf-2 -isysroot $XCODE/Platforms/${PLATFORMS[$i]}.platform/Developer/SDKs/${SDK[$i]}.sdk -miphoneos-version-min=${IPHONEOS_DEPLOYMENT_TARGET} -fembed-bitcode -Werror=partial-availability"
export LDFLAGS="-arch $ARCH -isysroot $XCODE/Platforms/${PLATFORMS[$i]}.platform/Developer/SDKs/${SDK[$i]}.sdk"
if [ "${PLATFORMS[$i]}" = "iPhoneSimulator" ]; then
	export CPPFLAGS="-D__IPHONE_OS_VERSION_MIN_REQUIRED=${IPHONEOS_DEPLOYMENT_TARGET%%.*}0000"
fi
