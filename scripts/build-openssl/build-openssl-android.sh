#!/bin/bash

set -o errexit
set -o nounset

CURRENT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
SCRIPT_DIR=$(dirname "$CURRENT_DIR");
source "$SCRIPT_DIR/build-common/setenv-android.sh";
source "$SCRIPT_DIR/build-openssl/setenv-openssl.sh";

main_run()
{
	prepare_build_openssl;

	export ANDROID_NDK="$ANDROID_TOOLCHAIN";
	build_openssl \
		./Configure \
		android-arm \

	loginfo "DONE !!!";
}

main_run $@;
