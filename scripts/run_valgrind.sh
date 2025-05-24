#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

valgrind --leak-check=yes ${BUILD_DEBUG_PATH}/src/${MAIN_EXE_NAME} /algo-trader/config/dockerDevelConfig.json
