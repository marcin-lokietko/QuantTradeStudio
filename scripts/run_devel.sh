#!/bin/bash
set -e

. $(dirname $(realpath -s $0))/.common.sh
. ${SCRIPTS_PATH}/.clear_containers.sh

docker-compose -f ${SCRIPTS_PATH}/run_devel/docker-compose.yml up -d --build
BACKEND_CONTAINER_ID=$(docker ps -aqf "name=run_devel_backend_1")
docker logs --follow ${BACKEND_CONTAINER_ID}
