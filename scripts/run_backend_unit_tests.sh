#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

${SCRIPTS_PATH}/execute_command_in_docker.sh backend "./algo-trader/build/debug/ut/UnitTests" 0
