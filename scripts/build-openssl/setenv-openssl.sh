#!/bin/bash

set -o errexit
set -o nounset

print_usage()
{
	echo '
NAME
       build-openssl

SYNOPSIS
       build-openssl [options]
       Example: ./build-openssl.sh

DESCRIPTION
       Auto build openssl for script.

OPTIONS
       -h, --help
                 Optional. Print help infomation and exit successfully.';
}

parse_options()
{
	options=$($CMD_GETOPT -o h \
												--long "help" \
												-n 'build-openssl' -- "$@");
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
	$@ --prefix=$OUTPUT_DIR/$ARCH \
		no-asm \
		no-shared \
		no-cast \
		no-idea \
		no-camellia;

	#make -j$MAX_JOBS && make install_engine
	make install_dev
}

prepare_build_openssl()
{
	loginfo "parsing options";
	parse_options $@;

	cd "$PROJECT_DIR";
	loginfo "change directory to $PROJECT_DIR";

	print_input_log;

	mkdir -p "$OPENSSL_BUILDDIR" && cd "$OPENSSL_BUILDDIR";
	download_tarball;
}

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
SCRIPT_DIR=$(dirname "$CURRENT_DIR");
source "$SCRIPT_DIR/build-common/base.sh";

OPENSSL_BUILDDIR="$BUILD_DIR/openssl";
