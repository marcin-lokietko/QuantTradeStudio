#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

# E.g. to run one scenario: run_e2e_tests.sh "--name \"My scenario name\""
if [ -n "$1" ]; then
  export TEST_ARGUMENTS=$1
  echo "E2E test arguments were provided: $TEST_ARGUMENTS"
else
  export TEST_ARGUMENTS=""
fi

docker compose --file ${SCRIPTS_PATH}/run_e2e_tests/docker-compose.yml up -d --build
TEST_CONTAINER_ID=$(docker ps -aqf "name=run_e2e_tests-e2e-tests-1")
docker logs --follow ${TEST_CONTAINER_ID}

# Check the exit code of the test container
TEST_EXIT_CODE=$(docker inspect ${TEST_CONTAINER_ID} --format='{{.State.ExitCode}}')

# Stop and remove containers
docker compose --file ${SCRIPTS_PATH}/run_e2e_tests/docker-compose.yml down

# Exit with the test container's exit code
if [ "$TEST_EXIT_CODE" -ne 0 ]; then
  echo "E2E tests failed with exit code $TEST_EXIT_CODE"
  exit $TEST_EXIT_CODE
else
  echo "E2E tests passed"
fi
