#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

mkdir -p ${LOGS_DIR}/valgrind

export BACKEND_CMD="valgrind --leak-check=yes /quant-trade-studio/build/debug/src/QuantTradeStudio /quant-trade-studio/tests/backend/configuration.json > /quant-trade-studio/logs/valgrind/valgrind_output.txt 2>&1"
docker compose --file ${SCRIPTS_PATH}/run_backend_component_tests/docker-compose.yml up -d --build
TEST_CONTAINER_ID=$(docker ps -aqf "name=run_backend_component_tests-backend-tests-1")
docker logs --follow ${TEST_CONTAINER_ID}

# Application running under Valgrind needs to be stopped gracefully for the valgrind report to be generated.
# "docker kill --signal=SIGINT ${BACKEND_CONTAINER_ID}" was not delivered to the valgrind process inside the container,
# so it's needed to send it manually:
BACKEND_CONTAINER_ID=$(docker ps -aqf "name=run_backend_component_tests-backend-1")
VALGRIND_PID_IN_DOCKER=$(docker exec ${BACKEND_CONTAINER_ID} ps aux | grep 'valgrind.bin.*QuantTradeStudio' | awk '{print $2}')
docker exec $BACKEND_CONTAINER_ID kill -SIGINT $VALGRIND_PID_IN_DOCKER

# Check the exit code of the test container
TEST_EXIT_CODE=$(docker inspect ${TEST_CONTAINER_ID} --format='{{.State.ExitCode}}')

# Stop and remove containers
docker compose --file ${SCRIPTS_PATH}/run_backend_component_tests/docker-compose.yml down

# Exit with the test container's exit code
if [ "$TEST_EXIT_CODE" -ne 0 ]; then
  echo "BE component tests failed with exit code $TEST_EXIT_CODE"
  exit $TEST_EXIT_CODE
fi

VALGRIND_REPORT_FILE="${LOGS_DIR}/valgrind/valgrind_output.txt"
VALGRIND_SUCCCESS_SUMMARY="ERROR SUMMARY: 0 errors from 0 contexts"
if grep -qF -- "$VALGRIND_SUCCCESS_SUMMARY" "$VALGRIND_REPORT_FILE"; then
  echo "Tests passed and Valgrind reported no errors"
  exit 0
fi

echo "Valgrind report contains errors. Please check the valgrind_output.txt report"
exit 1
