#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

. ${SCRIPTS_PATH}/execute_command_in_docker.sh frontend "cd /algo-trader/frontend && npm install --cache .npm-cache && npm run verify-format" 0

if [ $? -ne 0 ]; then
  echo "Frontend format check failed. Please run 'npm run format' to fix the issues."
  exit 1
fi
exit 0
