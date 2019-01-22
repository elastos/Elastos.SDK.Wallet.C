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
	"$DEPENDS_DIR/scripts/build-sqlite.sh" $@;

	#CFG_BUILD_ROOT
	"$DEPENDS_DIR/scripts/build-Elastos.SDK.Keypair.C.sh" $@;
	
}

export CFG_PROJECT_NAME="Elastos.SDK.Wallet.C";
export CFG_PROJECT_DIR="$PROJECT_DIR";
source "$DEPENDS_DIR/scripts/build.sh" $@;

