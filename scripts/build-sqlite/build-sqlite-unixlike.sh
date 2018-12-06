#!/bin/bash

set -o errexit
set -o nounset

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
SCRIPT_DIR=$(dirname "$CURRENT_DIR");
source "$SCRIPT_DIR/build-common/setenv-unixlike.sh";
source "$SCRIPT_DIR/build-sqlite/setenv-sqlite.sh";

main_run()
{
	prepare_build_sqlite;

	build_sqlite;

	loginfo "DONE !!!";
}

main_run $@;
