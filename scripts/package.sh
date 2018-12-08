#!/bin/bash

set -o errexit
set -o nounset

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);

TARGET_PLATFORMS=(Android
                  iOS
                  $(uname -s))

TARGET_ABIS=(armeabi-v7a  arm64-v8a  x86_64
             -            arm64      x86_64
             -            -          x86_64);
TARGET_ABI_TYPE=$((${#TARGET_ABIS[@]} / ${#TARGET_PLATFORMS[@]}));

for pidx in "${!TARGET_PLATFORMS[@]}"; do
	for (( tidx = 0; tidx < $TARGET_ABI_TYPE; tidx++)); do
		aidx=$((${TARGET_ABI_TYPE} * ${pidx} + ${tidx}));
		#echo "==========================$aidx";
		platform="${TARGET_PLATFORMS[pidx]}";
		abi="${TARGET_ABIS[aidx]}";
		if [[ "$abi" == "-" ]]; then
			continue;
		fi

		echo "Build for $platform ($abi)";
		"$SCRIPT_DIR/build.sh" --platform=$platform --arch=$abi;
	done
done

