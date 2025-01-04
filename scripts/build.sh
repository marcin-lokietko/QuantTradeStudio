#!/bin/bash
set -e

validInputArgs=(production debug skipUT sanitizer profiler)

for inputArg in "$@"
do
    found=0
    for validArg in ${validInputArgs[*]}
    do
        if [ $inputArg = $validArg ]; then
            found=1
        fi
    done

    if [ $found = 0 ]; then
        printf "Error: argument \""$inputArg"\" is not supported.\n"
        printf "Supported arguments: "
        for validArg in ${validInputArgs[*]}
        do
            printf $validArg" "
        done
        printf "\n"
        exit 1
    fi
done


BUILD_UT="ON"
for inputArg in "$@"
do
    if [ $inputArg = "skipUT" ]; then
        BUILD_UT="OFF"
    fi
done

. $(dirname $(realpath -s $0))/.common.sh

for inputArg in "$@"
do
    if [ $inputArg = "production" ]; then
        cmake -S${SRC_DIR} -B${BUILD_PRODUCTION_PATH} -DbuildType=production -DbuildUT:BOOL=${BUILD_UT}
        cmake --build ${BUILD_PRODUCTION_PATH} -- -j3
    elif [ $inputArg = "debug" ]; then
        cmake -S${SRC_DIR} -B${BUILD_DEBUG_PATH} -DbuildType=debug -DbuildUT:BOOL=${BUILD_UT}
        cmake --build ${BUILD_DEBUG_PATH} -- -j3
    elif [ $inputArg = "sanitizer" ]; then
        cmake -S${SRC_DIR} -B${BUILD_SANITIZER_PATH} -DbuildType=sanitizer -DbuildUT:BOOL=OFF
        cmake --build ${BUILD_SANITIZER_PATH} -- -j3
    elif [ $inputArg = "profiler" ]; then
        cmake -S${SRC_DIR} -B${BUILD_PROFILER_PATH} -DbuildType=profiler -DbuildUT:BOOL=OFF
        cmake --build ${BUILD_PROFILER_PATH} -- -j3
    fi
done
