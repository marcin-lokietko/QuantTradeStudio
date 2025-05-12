#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

${SCRIPTS_PATH}/execute_command_in_docker.sh frontend "cd /algo-trader/frontend && npm install && npm run test-not-interactive" 0
