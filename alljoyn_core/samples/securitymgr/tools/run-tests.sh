#!/bin/bash

# # 
#    Copyright (c) 2016 Open Connectivity Foundation and AllJoyn Open
#    Source Project Contributors and others.
#    
#    All rights reserved. This program and the accompanying materials are
#    made available under the terms of the Apache License, Version 2.0
#    which accompanies this distribution, and is available at
#    http://www.apache.org/licenses/LICENSE-2.0


set -x
set -e
SELF_DIR=$(cd $(dirname $0) > /dev/null; pwd)

AJN_SM_PATH=$(readlink -f "${SELF_DIR}/..")

AJN_DAEMON_PNAME="alljoyn-daemon"

if [ -z "${VARIANT}" ]; then
    VARIANT="debug"
fi

if [ -d "${AJN_SM_PATH}/build/linux/x86/${VARIANT}" ]; then
    PLATFORM="x86"
elif [ -d "${AJN_SM_PATH}/build/linux/x86_64/${VARIANT}" ]; then
    PLATFORM="x86_64"
else
    printf "Tests are not built yet!\nMake sure to export GTEST_DIR to the right Gtest home path and then build again, e.g.: $> scons CPU=x86 BINDINGS=cpp,c WS=off\n"
    exit 1
fi

PLATFORM_ROOT="${AJN_SM_PATH}/build/linux/${PLATFORM}/${VARIANT}"
TEST_ROOT="${PLATFORM_ROOT}/test/"

if [ "${VARIANT}" = "debug" ]; then  
    if ! nm "${TEST_ROOT}/agent/unit_test/secmgrctest" | grep BundledRouter &> /dev/null; then
        if [ "$(pidof ${AJN_DAEMON_PNAME})" ]; then
            echo "alljoyn-daemon is active...running tests..."
        else
             echo "Please start an alljoyn-daemon to be able to run the tests !"
             exit 1
        fi
    fi
fi

LIB_PATH="${PLATFORM_ROOT}/dist/security/lib:${PLATFORM_ROOT}/dist/cpp/lib"

export LD_LIBRARY_PATH="${LIB_PATH}"

# make sure coverage starts clean
if [ ! -z "$(which lcov)" ]; then
    lcov --directory "${PLATFORM_ROOT}" --zerocounters
fi

kill $(pidof multipeer_claim) || true
kill $(pidof secmgrctest) || true

# running unit tests
# we are doing some magic here to run each test in its own process as we still have some issues to run them in one go (AS-207)
echo "[[ Cleaning old Gtest results if any ]]"
rm -rf "${TEST_ROOT}"/gtestresults/
echo "[[ Running unit tests ]]"
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
for target in "agent"
do
    STORAGE_PATH=/tmp/secmgr.db "${TEST_ROOT}"/$target/unit_test/secmgrctest --gtest_output=xml:"${TEST_ROOT}"/gtestresults/
done

# running system tests
# echo "[[ Running system tests ]]"
#STORAGE_PATH=/tmp/secmgr.db "${TEST_ROOT}"/agent/multipeer_claim/run.sh

#Kill any remaining multipeer_claim processes hanging around.
kill $(pidof multipeer_claim) || true
# generate coverage report (lcov 1.10 or better required for --no-external)
if [ ! -z "$(which lcov)" ]; then
    if [ $(lcov --version | cut -d'.' -f2) -ge 10 ]; then
        EXTRA_ARGS="--no-external"
    fi
        COVDIR="${AJN_SM_PATH}"/build/coverage
        declare -a arr=("agent" "storage")
        for target in "${arr[@]}"
            do
            mkdir -p "${COVDIR}"/"$target"src > /dev/null 2>&1
            mkdir -p "${COVDIR}"/"$target"inc > /dev/null 2>&1

            lcov --quiet --capture -b "${AJN_SM_PATH}"/$target/src --directory \
            "${PLATFORM_ROOT}"/lib/$target $EXTRA_ARGS --output-file "${COVDIR}"/secmgr_"$target"_src.info || true
            lcov --quiet --capture -b "${AJN_SM_PATH}"/$target/inc --directory \
            "${PLATFORM_ROOT}"/lib/$target $EXTRA_ARGS --output-file "${COVDIR}"/secmgr_"$target"_inc.info || true
            genhtml --quiet --output-directory "${COVDIR}"/"$target"src "${COVDIR}"/secmgr_"$target"_src.info || true 
            genhtml --quiet --output-directory "${COVDIR}"/"$target"inc "${COVDIR}"/secmgr_"$target"_inc.info || true
            done
fi

exit 0