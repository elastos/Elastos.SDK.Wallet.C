#!/bin/bash

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
source "$CURRENT_DIR/base.sh";

SYSTEM_NAME="iOS"
SYSTEM_ABIS=(arm64 x86_64)
BUILD_DIR="$BUILD_BASE_DIR/$SYSTEM_NAME/$TARGET_ARCH";
TARBALL_DIR="$BUILD_BASE_DIR/tarball";
OUTPUT_DIR="$BUILD_ROOT_DIR/$SYSTEM_NAME/$TARGET_ARCH";
mkdir -p "$TARBALL_DIR";

XCODE="/Applications/Xcode.app/Contents/Developer"
if [ ! -d "$XCODE" ]; then
	echo "You have to install Xcode and the command line tools first"
	exit 1
fi

export CC="$XCODE/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang"
export IPHONEOS_DEPLOYMENT_TARGET="8"

ARCH_LIST=(arm64 x86_64)
PLATFORM_LIST=(iPhoneOS iPhoneSimulator)
SDK_LIST=(iPhoneOS12.1 iPhoneSimulator12.1)
for idx in "${!SYSTEM_ABIS[@]}"; do
	if [[ "${SYSTEM_ABIS[$idx]}" = "${TARGET_ARCH}" ]]; then
		LIST_IDX=${idx}
		break;
	fi
done

ARCH=${ARCH_LIST[$LIST_IDX]}
SYSROOT="$XCODE/Platforms/${PLATFORM_LIST[$LIST_IDX]}.platform/Developer/SDKs/${SDK_LIST[$LIST_IDX]}.sdk"
export CFLAGS="-arch ${ARCH_LIST[$LIST_IDX]} -pipe -Os -gdwarf-2 -isysroot $SYSROOT -miphoneos-version-min=${IPHONEOS_DEPLOYMENT_TARGET} -fembed-bitcode -Werror=partial-availability"
export LDFLAGS="-arch ${ARCH_LIST[$LIST_IDX]} -isysroot $SYSROOT"
if [ "${PLATFORM_LIST[$LIST_IDX]}" = "iPhoneSimulator" ]; then
	export CPPFLAGS="-D__IPHONE_OS_VERSION_MIN_REQUIRED=${IPHONEOS_DEPLOYMENT_TARGET%%.*}0000"
fi
