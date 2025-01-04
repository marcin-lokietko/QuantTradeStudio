#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

rm -rf ${BUILD_MAIN_DIR}
rm -rf ${LOGS_DIR}