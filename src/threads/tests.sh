#!/bin/bash
make clean
make

rm build/tests/threads/priority-donate-one.result
rm build/tests/threads/priority-donate-lower.result
rm build/tests/threads/priority-donate-multiple.result
rm build/tests/threads/priority-donate-nest.result
rm build/tests/threads/priority-donate-multiple2.result
rm build/tests/threads/priority-donate-sema.result
rm build/tests/threads/priority-donate-chain.result
rm build/tests/threads/priority-change.result
rm build/tests/threads/priority-condvar.result
rm build/tests/threads/priority-fifo.result
rm build/tests/threads/priority-preempt.result
rm build/tests/threads/priority-sema.result

rm build/tests/threads/alarm-multiple.result
rm build/tests/threads/alarm-single.result
rm build/tests/threads/alarm-priority.result
rm build/tests/threads/alarm-zero.result
rm build/tests/threads/alarm-negative.result
rm build/tests/threads/alarm-simultaneous.result

make build/tests/threads/priority-donate-one.result
make build/tests/threads/priority-donate-lower.result
make build/tests/threads/priority-donate-multiple.result
make build/tests/threads/priority-donate-nest.result
make build/tests/threads/priority-donate-multiple2.result
make build/tests/threads/priority-donate-sema.result
make build/tests/threads/priority-donate-chain.result
make build/tests/threads/priority-change.result
make build/tests/threads/priority-condvar.result
make build/tests/threads/priority-fifo.result
make build/tests/threads/priority-preempt.result
make build/tests/threads/priority-sema.result

make build/tests/threads/alarm-multiple.result
make build/tests/threads/alarm-single.result
make build/tests/threads/alarm-priority.result
make build/tests/threads/alarm-zero.result
make build/tests/threads/alarm-negative.result
make build/tests/threads/alarm-simultaneous.result

egrep 'PASS|FAIL' build/tests/threads/priority*.result
egrep 'PASS|FAIL' build/tests/threads/alarm*.result
