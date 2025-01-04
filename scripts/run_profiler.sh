#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

cd ${BUILD_PROFILER_PATH}
./${MAIN_EXE_NAME} ${LOGS_DIR}
gprof ${BUILD_PROFILER_PATH}/${MAIN_EXE_NAME} ${BUILD_PROFILER_PATH}/gmon.out > ${BUILD_PROFILER_PATH}/gprof_analysis.txt