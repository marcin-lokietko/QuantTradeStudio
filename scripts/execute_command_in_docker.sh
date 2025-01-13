#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

docker run \
    -u $(id -u) \
    -v ${BACKEND_DIR}:/algo-trader/backend \
    -v ${FRONTEND_DIR}:/algo-trader/frontend \
    -v ${SCRIPTS_PATH}:/algo-trader/scripts \
    -v ${BACKEND_BUILD_MAIN_DIR}:/algo-trader/build \
    -v ${LOGS_DIR}:/algo-trader/logs \
    -v ${KEYS_DIR}:/algo-trader/keys \
    -p 4200:4200 \
    -p 5000:5000 \
    -p 8080:8080 \
    -it algo-trader-$1 \
    bash -c "$2"
