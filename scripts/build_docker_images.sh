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

docker build ${OPTIONS} -t quant-trade-studio-backend ${SCRIPTS_PATH}/backend
docker build ${OPTIONS} -t quant-trade-studio-frontend ${SCRIPTS_PATH}/frontend
docker build ${OPTIONS} -t quant-trade-studio-backend-component-tests ${SCRIPTS_PATH}/run_backend_component_tests
docker build ${OPTIONS} -t quant-trade-studio-e2e-tests ${SCRIPTS_PATH}/run_e2e_tests
