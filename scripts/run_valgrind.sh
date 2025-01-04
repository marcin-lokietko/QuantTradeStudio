#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

valgrind --leak-check=yes ${BUILD_DEBUG_PATH}/${MAIN_EXE_NAME} ${LOGS_DIR}
