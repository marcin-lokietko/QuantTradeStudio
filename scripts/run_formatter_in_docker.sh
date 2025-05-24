#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

FORMATTER_TASK="validate"
if [ -n "$1" ]; then
  FORMATTER_TASK="$1"
fi

. ${SCRIPTS_PATH}/execute_command_in_docker.sh backend "/algo-trader/scripts/run_formatter.sh ${FORMATTER_TASK}" 0
