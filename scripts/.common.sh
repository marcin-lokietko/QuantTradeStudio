#!/bin/bash
set -e

SCRIPTS_PATH=$(dirname $(realpath -s $0))

SRC_DIR=${SCRIPTS_PATH}"/../src"
BUILD_MAIN_DIR=${SCRIPTS_PATH}"/../build"

BUILD_DEBUG_PATH=${BUILD_MAIN_DIR}"/debug"
BUILD_PRODUCTION_PATH=${BUILD_MAIN_DIR}"/production"
BUILD_SANITIZER_PATH=${BUILD_MAIN_DIR}"/sanitizer"
BUILD_PROFILER_PATH=${BUILD_MAIN_DIR}"/profiler"

MAIN_EXE_NAME="cppSandbox"

LOGS_DIR=${SCRIPTS_PATH}"/../logs"
