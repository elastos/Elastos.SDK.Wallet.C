#!/bin/bash

set -o errexit
set -o nounset

download_tarball()
{
	if [ ! -e "$TARBALL_DIR/.$SQLITE_NAME" ]; then
		sqlite_url="$SQLITE_BASE_URL/$SQLITE_TARBALL";
		echo sqlite "$sqlite_url" --output "$TARBALL_DIR/$SQLITE_TARBALL";
		curl "$sqlite_url" --output "$TARBALL_DIR/$SQLITE_TARBALL";
		echo "$sqlite_url" > "$TARBALL_DIR/.$SQLITE_NAME";
	fi

	loginfo "$SQLITE_TARBALL has been downloaded."
}

build_sqlite()
{
	mkdir -p "$SQLITE_BUILDDIR" && cd "$SQLITE_BUILDDIR";
	loginfo "change directory to $SQLITE_BUILDDIR";

	print_input_log;

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

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
SCRIPT_DIR=$(dirname "$CURRENT_DIR");

SQLITE_BUILDDIR="$BUILD_DIR/sqlite";

