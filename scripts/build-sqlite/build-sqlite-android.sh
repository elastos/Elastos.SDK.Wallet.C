#!/bin/bash

set -o errexit
set -o nounset

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
SCRIPT_DIR=$(dirname "$CURRENT_DIR");
GETOPT_ARCHS=(armeabi-v7a arm64-v8a x86_64)
source "$SCRIPT_DIR/build-sqlite/getopt-sqlite.sh";

main_run()
{
	loginfo "parsing options";
	parse_options $@;

	source "$SCRIPT_DIR/build-common/setenv-android.sh";
	source "$SCRIPT_DIR/build-sqlite/setenv-sqlite.sh";

	download_tarball;

	build_sqlite \
		--host=arm-linux-androideabi \
		--target=arm-linux-androideabi;

	loginfo "DONE !!!";
}

main_run $@;
