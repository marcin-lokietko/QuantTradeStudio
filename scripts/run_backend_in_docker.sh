#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

${SCRIPTS_PATH}/execute_command_in_docker.sh backend "/algo-trader/build/debug/${MAIN_EXE_NAME} /algo-trader/logs /algo-trader/keys" 5000
