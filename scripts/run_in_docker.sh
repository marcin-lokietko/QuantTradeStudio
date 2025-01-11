#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

#. ${SCRIPTS_PATH}/execute_command_in_docker.sh "/algo-trader/build/debug/${MAIN_EXE_NAME} /algo-trader/logs /algo-trader/keys"
${SCRIPTS_PATH}/execute_command_in_docker.sh "cd /algo-trader/frontend && npm install && ng serve --host 0.0.0.0"
