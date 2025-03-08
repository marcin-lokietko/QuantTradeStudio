#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

${SCRIPTS_PATH}/execute_command_in_docker.sh frontend "cd /algo-trader/frontend && npm install && ng serve --host 0.0.0.0" 4200
