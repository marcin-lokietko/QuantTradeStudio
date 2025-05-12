#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

docker-compose -f ${SCRIPTS_PATH}/run_backend_tests/docker-compose.yml up -d --build
TEST_CONTAINER_ID=$(docker ps -aqf "name=tests_backend-tests_1")
docker logs --follow ${TEST_CONTAINER_ID}
