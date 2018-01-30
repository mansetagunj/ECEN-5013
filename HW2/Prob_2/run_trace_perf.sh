#!/bin/bash



EXE_FILE=prob2.out


STRACE_LOG=strace_prob2_log
LTRACE_LOG=ltrace_prob2_log
PERF_LOG=perf_prob2_log

echo "-------------------------------------------------"
echo "Running strace..."
#strace command:
strace -C -r -tt -T -o $STRACE_LOG ./$EXE_FILE

echo "strace output stored in $STRACE_LOG"
echo "-------------------------------------------------"


echo "-------------------------------------------------"
echo "Running ltrace..."
#ltrace command:
ltrace -n4 -T -r -S -o $LTRACE_LOG ./$EXE_FILE

echo "ltrace output stored in $LTRACE_LOG"
echo "-------------------------------------------------"

echo "-------------------------------------------------"
echo "Running perf..."
#perf command:
perf stat -o $PERF_LOG ./$EXE_FILE
echo "perf output stored in $PERF_LOG"
echo "-------------------------------------------------"
