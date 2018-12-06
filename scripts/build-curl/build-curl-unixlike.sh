#!/bin/bash

set -o errexit
set -o nounset

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
SCRIPT_DIR=$(dirname "$CURRENT_DIR");
source "$SCRIPT_DIR/build-common/setenv-unixlike.sh";
source "$SCRIPT_DIR/build-curl/setenv-curl.sh";

main_run()
{
	# build openss first
	"$SCRIPT_DIR/build-openssl/build-openssl-unixlike.sh"

	prepare_build_curl;

	build_curl;

	loginfo "DONE !!!";
}

main_run $@;
