#!/bin/bash

set -o errexit
set -o nounset

print_usage()
{
	echo '
NAME
       build-curl-android

SYNOPSIS
       build-curl-android [options]
       Example: ./build-curl-android.sh

DESCRIPTION
       Auto build curl for android script.

OPTIONS
       -h, --help
                 Optional. Print help infomation and exit successfully.';
}

parse_options()
{
	options=$($CMD_GETOPT -o h \
												--long "help" \
												-n 'build-curl-android' -- "$@");
	eval set -- "$options"
	while true; do
		case "$1" in
			(-h | --help)
				print_usage;
				exit 0;
				;;
			(- | --)
				shift;
				break;
				;;
			(*)
				echo "Internal error!";
				exit 1;
				;;
		esac
	done
}

print_input_log()
{
	logtrace "*********************************************************";
	logtrace " Input infomation";
	logtrace "    curl version    : $CURL_VERSION";
	logtrace "    debug verbose   : $DEBUG_VERBOSE";
	logtrace "*********************************************************";
}

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
	if [ ! -e "$CURL_BUILDDIR/$CURL_NAME" ]; then
		tar xf "$TARBALL_DIR/$CURL_TARBALL";
	fi
	loginfo "$CURL_TARBALL has been unpacked."
	cd "$CURL_BUILDDIR/$CURL_NAME";

	./configure --prefix=$OUTPUT_DIR/$ARCH \
		--host=arm-linux-androideabi \
		--target=arm-linux-androideabi \
		--with-sysroot=$ANDROID_TOOLCHAIN/sysroot \
		--with-ssl=$OUTPUT_DIR/$ARCH \
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
		--disable-tftp

	make -j$MAX_JOBS -C lib libcurl.la && \
	make -C lib install-libLTLIBRARIES && \
	make -C include/curl install-pkgincludeHEADERS && \
	make install-pkgconfigDATA
}

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
source "$SCRIPT_DIR/base.sh";
source "$SCRIPT_DIR/setenv-android.sh";

CURL_BASE_URL="https://curl.haxx.se/download";
CURL_VERSION="7.62.0";
CURL_NAME="curl-$CURL_VERSION";
CURL_TARBALL="$CURL_NAME.tar.gz";
CURL_BUILDDIR="$BUILD_DIR/curl";

main_run()
{
	loginfo "parsing options";
	parse_options $@;

	# build openss first
	"$SCRIPT_DIR/build-openssl-android.sh"

	cd "$PROJECT_DIR";
	loginfo "change directory to $PROJECT_DIR";

	print_input_log;

	mkdir -p "$CURL_BUILDDIR" && cd "$CURL_BUILDDIR";
	download_tarball;

	build_curl;

	loginfo "DONE !!!";
}

main_run $@;
