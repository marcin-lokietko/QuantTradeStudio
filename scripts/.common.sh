#!/bin/bash
set -e

SCRIPTS_PATH=$(dirname $(realpath -s $0))

BACKEND_DIR=${SCRIPTS_PATH}"/../backend"
FRONTEND_DIR=${SCRIPTS_PATH}"/../frontend"
BACKEND_BUILD_MAIN_DIR=${SCRIPTS_PATH}"/../build"

BUILD_DEBUG_PATH=${BACKEND_BUILD_MAIN_DIR}"/debug"
BUILD_PRODUCTION_PATH=${BACKEND_BUILD_MAIN_DIR}"/production"
BUILD_SANITIZER_PATH=${BACKEND_BUILD_MAIN_DIR}"/sanitizer"
BUILD_PROFILER_PATH=${BACKEND_BUILD_MAIN_DIR}"/profiler"

MAIN_EXE_NAME="AlgoTrader"

LOGS_DIR=${SCRIPTS_PATH}"/../logs"
KEYS_DIR=${SCRIPTS_PATH}"/../keys"
CONFIG_DIR=${SCRIPTS_PATH}"/../config"

# Ensure directories exist. If they don't, and are created by mounting Docker's volumes, they may not be created with the correct permissions
mkdir -p ${LOGS_DIR}
mkdir -p ${KEYS_DIR}
mkdir -p ${CONFIG_DIR}

export USER_ID=$(id -u)
export GROUP_ID=$(id -g)
