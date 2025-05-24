#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

mkdir -p ${LOGS_DIR}/profiler

# When the backend is run, then the gmon.out profiler output is created in the working directory of the backend container.
# The default working directory is root, so we first need to change it to a directory, which is mounted to the host
export BACKEND_CMD="cd /algo-trader/build/profiler/src && ./AlgoTrader /algo-trader/tests/backend/configuration.json"
docker compose --file ${SCRIPTS_PATH}/run_backend_component_tests/docker-compose.yml up -d --build
TEST_CONTAINER_ID=$(docker ps -aqf "name=run_backend_component_tests-backend-tests-1")
docker logs --follow ${TEST_CONTAINER_ID}

# Check the exit code of the test container
TEST_EXIT_CODE=$(docker inspect ${TEST_CONTAINER_ID} --format='{{.State.ExitCode}}')

# Stop and remove containers
docker compose --file ${SCRIPTS_PATH}/run_backend_component_tests/docker-compose.yml down

# It's needed to process this file to get readable output.
gprof ${BUILD_PROFILER_PATH}/src/AlgoTrader ${BUILD_PROFILER_PATH}/src/gmon.out > ${LOGS_DIR}/profiler/gprof_analysis.txt

# Exit with the test container's exit code
if [ "$TEST_EXIT_CODE" -ne 0 ]; then
  echo "BE component tests failed with exit code $TEST_EXIT_CODE"
  exit $TEST_EXIT_CODE
else
  echo "BE component tests passed"
fi
