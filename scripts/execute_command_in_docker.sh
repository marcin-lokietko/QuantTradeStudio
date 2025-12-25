#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

DOCKER_FLAGS=""
if [ -t 1 ]; then
  # If stdout is a terminal, run interactively
  DOCKER_FLAGS="-it"
fi

PORT_MAPPING=""
if [ -n "$3" ] && [ "$3" != "0" ]; then
  PORT_MAPPING="-p $3:$3"
fi

docker run \
    -u $(id -u):$(id -g) \
    -v ${BACKEND_DIR}:/quant-trade-studio/backend \
    -v ${FRONTEND_DIR}:/quant-trade-studio/frontend \
    -v ${SCRIPTS_PATH}:/quant-trade-studio/scripts \
    -v ${BACKEND_BUILD_MAIN_DIR}:/quant-trade-studio/build \
    -v ${LOGS_DIR}:/quant-trade-studio/logs \
    -v ${KEYS_DIR}:/quant-trade-studio/keys \
    -v ${CONFIG_DIR}:/quant-trade-studio/config \
    $PORT_MAPPING \
    $DOCKER_FLAGS \
    quant-trade-studio-$1 \
    bash -c "$2"