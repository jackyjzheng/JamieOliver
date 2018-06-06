#!/bin/bash
USERNAME="ubuntu"
ADVANCE_HOSTS="18.232.91.179 18.206.171.173"
INCREMENT_HOSTS="34.235.152.153 18.204.245.243 34.228.196.116 52.23.250.133 54.208.204.102 54.82.246.180 34.201.6.179 18.205.7.217"
ADVANCE_SCRIPT="chmod u+x scriptToRun.sh; ./scriptToRun.sh"
INCREMENT_SCRIPT="chmod u+x scriptToRun_increment.sh; ./scriptToRun_increment.sh"

KEYPATH="/Users/dennisfong/test.pem"
for HOSTNAME in ${ADVANCE_HOSTS} ; do
		scp -i ${KEYPATH} "scriptToRun.sh" ${USERNAME}"@"${HOSTNAME}":~"
    ssh -i ${KEYPATH} -t -l ${USERNAME} ${HOSTNAME} "${ADVANCE_SCRIPT}"
done

for HOSTNAME in ${INCREMENT_HOSTS} ; do
		scp -i ${KEYPATH} "scriptToRun_increment.sh" ${USERNAME}"@"${HOSTNAME}":~"
    ssh -i ${KEYPATH} -t -l ${USERNAME} ${HOSTNAME} "${INCREMENT_SCRIPT}"
done