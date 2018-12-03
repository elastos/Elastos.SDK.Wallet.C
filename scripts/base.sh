#!/bin/bash

set -o errexit
set -o nounset

logdbg()
{
	if [[ $DEBUG_VERBOSE == false ]]; then
		return;
	fi

	echo -e " [d]: $@";
}

logtrace()
{
	echo -e "\033[1;34m [-]: $@ \033[00m";
}

loginfo()
{
	echo -e "\033[1;32m [+]: $@ \033[00m";
}

logwarn()
{
	echo -e "\033[1;33m [!]: $@ \033[00m";
}

logerr_and_exit()
{
	echo -e "\033[1;31m [x]: $@ \033[00m";
	exit 1;
}

trim() {
	local var="$*"
	var="${var#"${var%%[![:space:]]*}"}"   # remove leading whitespace characters
	var="${var%"${var##*[![:space:]]}"}"   # remove trailing whitespace characters
	echo -n "$var"
}

CMD_GETOPT="getopt";
if [ "$(uname -s)" == "Darwin" ]; then
	CMD_GETOPT="/usr/local/Cellar/gnu-getopt/1.1.6/bin/getopt";
fi

MAX_JOBS=2
if [ -f /proc/cpuinfo ]; then
	MAX_JOBS=$(grep flags /proc/cpuinfo |wc -l)
elif [ ! -z $(which sysctl) ]; then
	MAX_JOBS=$(sysctl -n hw.ncpu)
fi

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
PROJECT_DIR=$(dirname "$SCRIPT_DIR");
BUILD_BASE_DIR="$PROJECT_DIR/build";

DEBUG_VERBOSE=false;

