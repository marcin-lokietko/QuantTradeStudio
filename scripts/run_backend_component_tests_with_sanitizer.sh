#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

mkdir -p ${LOGS_DIR}/sanitizer

# ==7332==HINT: LeakSanitizer does not work under ptrace (strace, gdb, etc)
export ASAN_OPTIONS="detect_leaks=0:verbosity=1:abort_on_error=0"
export BACKEND_CMD="/algo-trader/build/sanitizer/src/AlgoTrader /algo-trader/tests/backend/configuration.json > /algo-trader/logs/sanitizer/sanitizer_output.txt 2>&1"
docker compose --file ${SCRIPTS_PATH}/run_backend_component_tests/docker-compose.yml up -d --build
TEST_CONTAINER_ID=$(docker ps -aqf "name=run_backend_component_tests-backend-tests-1")
docker logs --follow ${TEST_CONTAINER_ID}

# Check the exit code of the test container
TEST_EXIT_CODE=$(docker inspect ${TEST_CONTAINER_ID} --format='{{.State.ExitCode}}')

# Stop and remove containers
docker compose --file ${SCRIPTS_PATH}/run_backend_component_tests/docker-compose.yml down

SANITIZER_REPORT_FILE="${LOGS_DIR}/sanitizer/sanitizer_output.txt"
SANITIZER_SUCCCESS_SUMMARY="ABORTING"
if grep -qF -- "$SANITIZER_SUCCCESS_SUMMARY" "$SANITIZER_REPORT_FILE"; then
  echo "Address sanitizer report contains errors. Please check the sanitizer_output.txt report"
  exit 1
fi

# Exit with the test container's exit code
if [ "$TEST_EXIT_CODE" -ne 0 ]; then
  echo "BE component tests failed with exit code $TEST_EXIT_CODE"
  exit $TEST_EXIT_CODE
fi

echo "BE component tests passed and address sanitizer reported no errors"
