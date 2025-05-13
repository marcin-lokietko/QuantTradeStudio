#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

# "--cache .npm-cache" tells npm to use a directory that your the current user owns (this caused issues in the CI environment)
${SCRIPTS_PATH}/execute_command_in_docker.sh frontend "cd /algo-trader/frontend && npm install --cache .npm-cache && ng serve --host 0.0.0.0" 4200
