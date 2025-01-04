#!/bin/bash
set -e

REMOVED_CONTAINERS=`docker ps -a -q`
if [[ "${REMOVED_CONTAINERS}" != "" ]]; then
    docker rm -f ${REMOVED_CONTAINERS}
fi
