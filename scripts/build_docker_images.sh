#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

docker build -t algo-trader ${SCRIPTS_PATH}
