#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

# E.g. to run one scenario:
# ./scripts/run_backend_component_tests.sh "--name='^Invoking /health returns OK status code$'"
# The ^ and $ anchors tell Behave to match the exact scenario name, not just a substring.
if [ -n "$1" ]; then
  export TEST_ARGUMENTS=$1
  echo "Backend CT arguments were provided: $TEST_ARGUMENTS"
else
  export TEST_ARGUMENTS=""
fi

export BACKEND_CMD="/quant-trade-studio/build/debug/src/QuantTradeStudio /quant-trade-studio/tests/backend/configuration.json" 
docker compose --file ${SCRIPTS_PATH}/run_backend_component_tests/docker-compose.yml up -d --build
TEST_CONTAINER_ID=$(docker ps -aqf "name=run_backend_component_tests-backend-tests-1")
docker logs --follow ${TEST_CONTAINER_ID} | tee ${LOGS_DIR}/backend_tests/behave_output.log

# Check the exit code of the test container
TEST_EXIT_CODE=$(docker inspect ${TEST_CONTAINER_ID} --format='{{.State.ExitCode}}')

# Stop and remove containers
docker compose --file ${SCRIPTS_PATH}/run_backend_component_tests/docker-compose.yml down

# Exit with the test container's exit code
if [ "$TEST_EXIT_CODE" -ne 0 ]; then
  echo "BE component tests failed with exit code $TEST_EXIT_CODE"
  exit $TEST_EXIT_CODE
else
  echo "BE component tests passed"
fi
