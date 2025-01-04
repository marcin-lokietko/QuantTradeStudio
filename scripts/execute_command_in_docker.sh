#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

docker run \
    -u $(id -u) \
    -v ${SRC_DIR}:/algo-trader/src \
    -v ${SCRIPTS_PATH}:/algo-trader/scripts \
    -v ${BUILD_PATH}:/algo-trader/build \
    -v ${LOGS_DIR}:/algo-trader/logs \
    -it algo-trader \
    $1
