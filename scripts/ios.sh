
export TARGET_PLATFORM=ios;

if [[ "$1" == "x86_64" ]]; then
    export TARGET_ABI=x86_64;
else
    echo "Unimplemented.";
    exit 1;
fi


OSX_SDK="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator12.0.sdk";
if [[ ! -z "$2" ]]; then
  OSX_SDK=$2;
fi
export OSX_SDK;

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
PROJECT_DIR=$(dirname "$SCRIPT_DIR");
TARGET_DIR="obj/${TARGET_PLATFORM}/${TARGET_ABI}";
OBJ_DIR=${PROJECT_DIR}/${TARGET_DIR};

cd "$PROJECT_DIR";
mkdir -p "${TARGET_DIR}";
cd "$OBJ_DIR";
