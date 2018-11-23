
if [[  $OSTYPE != *linux* ]]; then
 echo "Not supported OSTYPE($OSTYPE)."
 exit 1
fi

export TARGET_PLATFORM=linux
export TARGET_ABI=x86_64

SCRIPT_DIR=$(cd $(dirname "${BASH_SOURCE[0]}") && pwd);
PROJECT_DIR=$(dirname "$SCRIPT_DIR");
TARGET_DIR="obj/${TARGET_PLATFORM}/${TARGET_ABI}";
OBJ_DIR=${PROJECT_DIR}/${TARGET_DIR};

cd "$PROJECT_DIR";
mkdir -p "${TARGET_DIR}";
cd "$OBJ_DIR";
