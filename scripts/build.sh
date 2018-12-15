#!/bin/bash

set -o errexit
set -o nounset

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
PROJECT_DIR=$(dirname "$CURRENT_DIR")
DEPENDS_DIR="$PROJECT_DIR/config/scripts";

if [ ! -d "$DEPENDS_DIR" ]; then
	git submodule init;
	git submodule update;
fi

build_extfunc_depends()
{
	"$DEPENDS_DIR/build-curl.sh" $@;
	"$DEPENDS_DIR/build-sqlite.sh" $@;

	#CFG_BUILD_ROOT
	"$DEPENDS_DIR/build-Elastos.ORG.Wallet.Lib.C.sh" $@;
	
}
CFG_PROJECT_DIR="$PROJECT_DIR";
source "$DEPENDS_DIR/build.sh" $@;

