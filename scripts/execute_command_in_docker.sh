#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

DOCKER_FLAGS=""
if [ -t 1 ]; then
  # If stdout is a terminal, run interactively
  DOCKER_FLAGS="-it"
fi

docker run \
    -u $(id -u) \
    -v ${BACKEND_DIR}:/algo-trader/backend \
    -v ${FRONTEND_DIR}:/algo-trader/frontend \
    -v ${SCRIPTS_PATH}:/algo-trader/scripts \
    -v ${BACKEND_BUILD_MAIN_DIR}:/algo-trader/build \
    -v ${LOGS_DIR}:/algo-trader/logs \
    -v ${KEYS_DIR}:/algo-trader/keys \
    -v ${CONFIG_DIR}:/algo-trader/config \
    -p $3:$3 \
    $DOCKER_FLAGS \
    algo-trader-$1 \
    bash -c "$2"
