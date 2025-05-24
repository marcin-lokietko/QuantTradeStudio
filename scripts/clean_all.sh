#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

rm -rf ${BACKEND_BUILD_MAIN_DIR}
rm -rf ${LOGS_DIR}
