#!/bin/bash

set -o errexit
set -o nounset

download_tarball()
{
	if [ ! -e "$TARBALL_DIR/.$SQLITE_NAME" ]; then
		sqlite_url="$SQLITE_BASE_URL/$SQLITE_TARBALL";
		echo curl "$sqlite_url" --output "$TARBALL_DIR/$SQLITE_TARBALL";
		curl "$sqlite_url" --output "$TARBALL_DIR/$SQLITE_TARBALL";
		echo "$sqlite_url" > "$TARBALL_DIR/.$SQLITE_NAME";
	fi

	loginfo "$SQLITE_TARBALL has been downloaded."
}

build_sqlite()
{
	mkdir -p "$SQLITE_BUILDDIR" && cd "$SQLITE_BUILDDIR";
	loginfo "change directory to $SQLITE_BUILDDIR";

	if [ ! -e "$SQLITE_BUILDDIR/$SQLITE_NAME" ]; then
		tar xf "$TARBALL_DIR/$SQLITE_TARBALL";
	fi
	loginfo "$SQLITE_TARBALL has been unpacked."
	cd "$SQLITE_BUILDDIR/$SQLITE_NAME";

	#export CFLAGS="-DSQLITE_NOHAVE_SYSTEM"
	./configure --prefix=$OUTPUT_DIR \
		--enable-static \
		--disable-shared \
		--disable-static-shell \
		$@

	make -j$MAX_JOBS libsqlite3.la && make install-libLTLIBRARIES install-includeHEADERS install-pkgconfigDATA
}

main_run()
{
	loginfo "parsing options";
	getopt_parse_options $@;

	case "$CFG_TARGET_PLATFORM" in
		(Android)
			source "$SCRIPT_DIR/build-common/setenv-android.sh";
			CONFIG_PARAM="--host=$ANDROID_TOOLCHAIN --target=$ANDROID_TOOLCHAIN";
			;;
		(iOS)
			source "$SCRIPT_DIR/build-common/setenv-ios.sh";
			CONFIG_PARAM="--host=$IOS_TOOLCHAIN --target=$IOS_TOOLCHAIN";
			;;
		(*)
			source "$SCRIPT_DIR/build-common/setenv-unixlike.sh";
			CONFIG_PARAM=;
			;;
	esac

	SQLITE_BUILDDIR="$BUILD_DIR/sqlite";

	download_tarball;

	build_sqlite $CONFIG_PARAM;

	loginfo "DONE !!!";
}

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);

source "$SCRIPT_DIR/build-common/getopt.sh";

main_run $@;
