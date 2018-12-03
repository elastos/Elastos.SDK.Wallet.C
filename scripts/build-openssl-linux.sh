#!/bin/bash

set -o errexit
set -o nounset

print_usage()
{
	echo '
NAME
       build-openssl-linux

SYNOPSIS
       build-openssl-linux [options]
       Example: ./build-openssl-linux.sh

DESCRIPTION
       Auto build openssl for linux script.

OPTIONS
       -h, --help
                 Optional. Print help infomation and exit successfully.';
}

parse_options()
{
	options=$($CMD_GETOPT -o h \
												--long "help" \
												-n 'build-openssl-linux' -- "$@");
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
	logtrace "    openssl version : $OPENSSL_VERSION";
	logtrace "    debug verbose   : $DEBUG_VERBOSE";
	logtrace "*********************************************************";
}

download_tarball()
{
	if [ ! -e "$TARBALL_DIR/.$OPENSSL_NAME" ]; then
		openss_url="$OPENSSL_BASE_URL/$OPENSSL_TARBALL";
		echo curl "$openss_url" --output "$TARBALL_DIR/$OPENSSL_TARBALL";
		curl "$openss_url" --output "$TARBALL_DIR/$OPENSSL_TARBALL";
		echo "$openss_url" > "$TARBALL_DIR/.$OPENSSL_NAME";
	fi

	loginfo "$OPENSSL_TARBALL has been downloaded."
}

build_openssl()
{
	if [ ! -e "$OPENSSL_BUILDDIR/$OPENSSL_NAME" ]; then
		tar xf "$TARBALL_DIR/$OPENSSL_TARBALL";
	fi
	loginfo "$OPENSSL_TARBALL has been unpacked."
	cd "$OPENSSL_BUILDDIR/$OPENSSL_NAME";
	./Configure --prefix=$OUTPUT_DIR/$ARCH \
		linux-x86_64-clang \
		no-asm \
		no-shared \
		no-cast \
		no-idea \
		no-camellia;

	#make -j$MAX_JOBS && make install_engine
	make install_dev
}

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
source "$SCRIPT_DIR/base.sh";
source "$SCRIPT_DIR/setenv-linux.sh";

OPENSSL_BASE_URL="https://www.openssl.org/source";
OPENSSL_VERSION="1.1.1a";
OPENSSL_NAME="openssl-$OPENSSL_VERSION";
OPENSSL_TARBALL="$OPENSSL_NAME.tar.gz";
OPENSSL_BUILDDIR="$BUILD_DIR/openssl";

main_run()
{
	loginfo "parsing options";
	parse_options $@;

	cd "$PROJECT_DIR";
	loginfo "change directory to $PROJECT_DIR";

	print_input_log;

	mkdir -p "$OPENSSL_BUILDDIR" && cd "$OPENSSL_BUILDDIR";
	download_tarball;

	build_openssl;

	loginfo "DONE !!!";
}

main_run $@;
