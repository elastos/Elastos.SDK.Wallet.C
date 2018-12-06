#!/bin/bash

set -o errexit
set -o nounset

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
SCRIPT_DIR=$(dirname "$CURRENT_DIR");
GETOPT_ARCHS=(arm64 x86_64)
source "$SCRIPT_DIR/build-sqlite/getopt-sqlite.sh";

main_run()
{
	loginfo "parsing options";
	parse_options $@;

	source "$SCRIPT_DIR/build-common/setenv-ios.sh";
	source "$SCRIPT_DIR/build-sqlite/setenv-sqlite.sh";

	download_tarball;

	build_sqlite \
		--host=arm-apple-darwin;

	loginfo "DONE !!!";
}

main_run $@;
