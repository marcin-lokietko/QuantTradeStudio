#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh

docker build -t algo-trader-backend ${SCRIPTS_PATH}/backend
docker build -t algo-trader-frontend ${SCRIPTS_PATH}/frontend
docker build -t algo-trader-tests ${SCRIPTS_PATH}/tests

