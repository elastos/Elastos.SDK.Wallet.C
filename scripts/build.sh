#!/bin/bash

set -o errexit
set -o nounset

build_project()
{
	mkdir -p "$PROJECT_BUILDDIR" && cd "$PROJECT_BUILDDIR";
	loginfo "change directory to $PROJECT_BUILDDIR";

	cd "$PROJECT_BUILDDIR";
	cmake  "$PROJECT_DIR" \
		-DCMAKE_INSTALL_PREFIX="$OUTPUT_DIR" \
		-DBUILD_SHARED_LIBS=$BUILD_SHARED_LIBS \
		-DTARGET_PLATFORM=$TARGET_PLATFORM \
		-DTARGET_ABI=$TARGET_ABI;

	make -j$MAX_JOBS  && make install;
}

main_run()
{

	# build dependencies first.
	#"$SCRIPT_DIR/build-curl.sh" $@;
	#"$SCRIPT_DIR/build-sqlite.sh" $@;

	loginfo "parsing options";
	parse_options $@;

	case "$TARGET_PLATFORM" in
		(Android)
			source "$SCRIPT_DIR/build-common/setenv-android.sh";
			BUILD_SHARED_LIBS=OFF
			;;
		(iOS)
			source "$SCRIPT_DIR/build-common/setenv-ios.sh";
			BUILD_SHARED_LIBS=OFF
			;;
		(*)
			source "$SCRIPT_DIR/build-common/setenv-unixlike.sh";
			BUILD_SHARED_LIBS=ON
			;;
	esac

	PROJECT_NAME="Elastos.SDK.Wallet.C";
	PROJECT_BUILDDIR="$BUILD_DIR/$PROJECT_NAME";

	build_project $@;

	loginfo "DONE !!!";
}

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);

source "$SCRIPT_DIR/build-common/getopt.sh";

main_run $@;
