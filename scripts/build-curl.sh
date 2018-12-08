#!/bin/bash

set -o errexit
set -o nounset

download_tarball()
{
	if [ ! -e "$TARBALL_DIR/.$CURL_NAME" ]; then
		curl_url="$CURL_BASE_URL/$CURL_TARBALL";
		echo curl "$curl_url" --output "$TARBALL_DIR/$CURL_TARBALL";
		curl "$curl_url" --output "$TARBALL_DIR/$CURL_TARBALL";
		echo "$curl_url" > "$TARBALL_DIR/.$CURL_NAME";
	fi

	loginfo "$CURL_TARBALL has been downloaded."
}

build_curl()
{
	mkdir -p "$CURL_BUILDDIR" && cd "$CURL_BUILDDIR";
	loginfo "change directory to $CURL_BUILDDIR";

	if [ ! -e "$CURL_BUILDDIR/$CURL_NAME" ]; then
		tar xf "$TARBALL_DIR/$CURL_TARBALL";
	fi
	loginfo "$CURL_TARBALL has been unpacked."
	cd "$CURL_BUILDDIR/$CURL_NAME";
	./configure --prefix=$OUTPUT_DIR \
		--with-ssl=$OUTPUT_DIR \
		--enable-static \
		--disable-shared \
		--disable-verbose \
		--enable-threaded-resolver \
		--enable-ipv6 \
		--disable-dict \
		--disable-ftp \
		--disable-gopher \
		--disable-imap \
		--disable-pop3 \
		--disable-rtsp \
		--disable-smb \
		--disable-smtp \
		--disable-telnet \
		--disable-tftp \
		$@

	make -j$MAX_JOBS -C lib libcurl.la V=1 && \
	make -C lib install-libLTLIBRARIES && \
	make -C include/curl install-pkgincludeHEADERS && \
	make install-pkgconfigDATA
}

main_run()
{

	# build openssl first.
	"$SCRIPT_DIR/build-openssl.sh" $@;

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

	CURL_BUILDDIR="$BUILD_DIR/curl";

	download_tarball;

	build_curl $CONFIG_PARAM;

	loginfo "DONE !!!";
}

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);

source "$SCRIPT_DIR/build-common/getopt.sh";

main_run $@;
