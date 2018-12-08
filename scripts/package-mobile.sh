#!/bin/bash

set -o errexit
set -o nounset

build_all()
{
	local target_platforms=(Android
	                        iOS);
	                        #$(uname -s));

	local target_abis=(armeabi-v7a  arm64-v8a  x86_64
	                   -            arm64      x86_64
	                   -            -          x86_64);
	local target_abi_type=$((${#target_abis[@]} / ${#target_platforms[@]}));

	for pidx in "${!target_platforms[@]}"; do
		for (( tidx = 0; tidx < $target_abi_type; tidx++)); do
			aidx=$((${target_abi_type} * ${pidx} + ${tidx}));
			#echo "==========================$aidx";
			platform="${target_platforms[pidx]}";
			abi="${target_abis[aidx]}";
			if [[ "$abi" == "-" ]]; then
				continue;
			fi

			echo "Build for $platform ($abi)";
			"$SCRIPT_DIR/build.sh" --platform=$platform --arch=$abi --enable-static;
		done
	done
}

package_android()
{
	local pkg_android_dir="$PACKAGE_DIR/Android";

	local abi_list=$(ls "$BUILD_ROOT_DIR/Android");
	for abi in $abi_list; do
		mkdir -p "$pkg_android_dir/jni/$abi";
		cp -rv "$BUILD_ROOT_DIR/Android/$abi/lib/lib"*.a "$pkg_android_dir/jni/$abi/";
	done

	cd "$pkg_android_dir";
	zip -r "${PACKAGE_DIR}/${PROJECT_NAME}.aar" *;
	loginfo "Success to create ${PACKAGE_DIR}/${PROJECT_NAME}.aar";
}

package_ios()
{
	local pkg_ios_dir="$PACKAGE_DIR/iOS";

	local package_list=()
	local abi_list=$(ls "$BUILD_ROOT_DIR/iOS");
	for abi in $abi_list; do
		mkdir -p "$pkg_ios_dir/$abi";
		cp -rv "$BUILD_ROOT_DIR/iOS/$abi/lib/lib"*.a "$pkg_ios_dir/$abi/";

		echo "lipo $abi library ..."
		cd "$pkg_ios_dir/$abi/";
		libtool -static -o "${pkg_ios_dir}/$abi/${PROJECT_NAME}.raw" *.a

		package_list+=("$pkg_ios_dir/$abi/$PROJECT_NAME.raw")
	done

	echo "Creating ${PROJECT_NAME}.framework"
	mkdir -p "${PACKAGE_DIR}/${PROJECT_NAME}.framework/Headers";
	#cp -r include/$PROJECT_NAME/* $PROJECT_NAME.framework/Headers/
	cd "$pkg_ios_dir/";
	lipo -create ${package_list[@]} -output "$PACKAGE_DIR/${PROJECT_NAME}.framework/${PROJECT_NAME}"

	loginfo "Success to create ${PACKAGE_DIR}/${PROJECT_NAME}.framework";
}

main_run()
{
#	build_all;

	loginfo "Remove previous packages in $PACKAGE_DIR";
	rm -rf "$PACKAGE_DIR";

	package_android;

	package_ios;
}

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
source "$SCRIPT_DIR/build-common/base.sh";
PROJECT_NAME="Elastos.SDK.Wallet.C"
PACKAGE_DIR="$BUILD_BASE_DIR/package";

main_run $@;
