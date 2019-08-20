#!/bin/bash

set -o errexit
set -o nounset

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
PROJECT_DIR=$(dirname "$CURRENT_DIR")
DEPENDS_DIR="$PROJECT_DIR/config";

cd "$PROJECT_DIR";
git submodule init;
git submodule update;

build_extfunc_depends()
{
	"$DEPENDS_DIR/scripts/build-curl.sh" $@;
    if [[ $CFG_TARGET_PLATFORM == "iOS" ]]; then
        loginfo "Ignore to build sqlite for iOS, use buildin."
    else
        "$DEPENDS_DIR/scripts/build-sqlite.sh" $@;
    fi

	#CFG_BUILD_ROOT
	"$DEPENDS_DIR/scripts/build-Elastos.SDK.Keypair.C.sh" $@;
	
}

export CFG_PROJECT_NAME="Elastos.SDK.Wallet.C";
export CFG_PROJECT_DIR="$PROJECT_DIR";
export CFG_CMAKELIST_DIR="$PROJECT_DIR";
source "$DEPENDS_DIR/scripts/build.sh" $@;

