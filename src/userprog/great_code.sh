#!/bin/bash
make
pintos-mkdisk filesys.dsk --filesys-size=2 > /dev/null 2>&1
pintos -f -q > /dev/null 2>&1
pintos -p build/tests/userprog/args-none -a args-none -- -q > /dev/null 2>&1
pintos -p build/tests/userprog/halt -a halt -- -q > /dev/null 2>&1
pintos -p build/tests/userprog/exit -a exit -- -q > /dev/null 2>&1
pintos run 'args-none'
pintos run 'halt'
pintos run 'exit'

egrep 'PASS|FAIL' build/tests/userprog/*.result