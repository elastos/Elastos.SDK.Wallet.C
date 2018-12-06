#!/bin/bash

set -o errexit
set -o nounset

print_usage()
{
	echo '
NAME
       build-sqlite

SYNOPSIS
       build-sqlite [options]
       Example: ./build-sqlite.sh

DESCRIPTION
       Auto build sqlite for script.

OPTIONS
       -m, --arch=('${GETOPT_ARCHS[@]}')
                 Required. platform abi.

       -h, --help
                 Optional. Print help infomation and exit successfully.';
}

parse_options()
{
	options=$($CMD_GETOPT -o m:h \
							--long "arch:,help" \
							-n 'build-sqlite' -- "$@");
	eval set -- "$options"
	while true; do
		case "$1" in
			(-m | --arch)
				TARGET_ARCH=$2;
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

	found=false
	for item in "${GETOPT_ARCHS[@]}"; do
		if [ "$item" == "$TARGET_ARCH" ]; then
			found=true
		fi
	done
	if [[ $found != true ]]; then
		print_input_log;
		logerr_and_exit "arch is not correct. use -h or --help for prompt.";
	fi
}

print_input_log()
{
	logtrace "*********************************************************";
	logtrace " Input infomation";
	logtrace "    sqlite version  : $SQLITE_VERSION";
	logtrace "    arch            : $TARGET_ARCH";
	logtrace "    debug verbose   : $DEBUG_VERBOSE";
	logtrace "*********************************************************";
}


CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
SCRIPT_DIR=$(dirname "$CURRENT_DIR");
source "$SCRIPT_DIR/build-common/base.sh";

TARGET_ARCH="unknown";

