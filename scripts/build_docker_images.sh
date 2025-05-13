#!/bin/bash
set -e

OPTIONS=""
for inputArg in "$@"
do
    if [ $inputArg = "noCache" ]; then
        OPTIONS="--no-cache"
    fi
done

. $(dirname $(realpath -s $0))/.common.sh

docker build ${OPTIONS} -t algo-trader-backend ${SCRIPTS_PATH}/backend
docker build ${OPTIONS} -t algo-trader-frontend ${SCRIPTS_PATH}/frontend
docker build ${OPTIONS} -t algo-trader-backend-tests ${SCRIPTS_PATH}/run_backend_tests
docker build ${OPTIONS} -t algo-trader-e2e-tests ${SCRIPTS_PATH}/run_e2e_tests
