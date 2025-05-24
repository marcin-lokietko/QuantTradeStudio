#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

mkdir -p ${BACKEND_BUILD_MAIN_DIR}

BUILD_TYPE="debug"
if [ -n "$1" ]; then
  BUILD_TYPE="$1"
fi

. ${SCRIPTS_PATH}/execute_command_in_docker.sh backend "/algo-trader/scripts/build.sh ${BUILD_TYPE}" 0
