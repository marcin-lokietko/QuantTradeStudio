#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

${SCRIPTS_PATH}/execute_command_in_docker.sh backend "./quant-trade-studio/build/debug/ut/UnitTests" 0
