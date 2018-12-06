#!/bin/bash

set -o errexit
set -o nounset

print_usage()
{
	echo '
NAME
       getopt-script

SYNOPSIS
       getopt-script [options]

DESCRIPTION
       getopt script.

OPTIONS
       -p, --platform=(Android | iOS)
                 Optional. target platform. If unspecified, use [`uname -m`] as default.

       -m, --arch=(ARCH)
                 Optional. target platform abi.
                 For native compile, use [x86_64] as default.
                 For Android, use [armeabi-v7a] as default.
                 For iOS, use [arm64] as default.

       -h, --help
                 Optional. Print help infomation and exit successfully.';
}

parse_options()
{
	options=$($CMD_GETOPT -o p:m:h \
							--long "platform:,arch:,help" \
							-n 'getopt-script' -- "$@");
	eval set -- "$options"
	while true; do
		case "$1" in
			(-p | --platform)
				TARGET_PLATFORM=$2;
				shift 2;
				;;
			(-m | --arch)
				TARGET_ABI=$2;
				shift 2;
				;;
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

	if [[ -z "$TARGET_ABI" ]]; then
		case "$TARGET_PLATFORM" in
			(Android)
				TARGET_ABI="armeabi-v7a";
				;;
			(iOS)
				TARGET_ABI="arm64";
				;;
			(*)
				TARGET_ABI="x86_64";
				;;
		esac
	fi

	print_input_log;
}

print_input_log()
{
	logtrace "*********************************************************";
	logtrace " Input infomation";
	logtrace "    platform        : $TARGET_PLATFORM";
	logtrace "    abi             : $TARGET_ABI";
	logtrace "    debug verbose   : $DEBUG_VERBOSE";
	logtrace "*********************************************************";
}


CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
SCRIPT_DIR=$(dirname "$CURRENT_DIR");
source "$SCRIPT_DIR/build-common/base.sh";

TARGET_PLATFORM=$(uname -s);
TARGET_ABI=;

