#!/bin/bash

echo
echo "Running unit tests"

fail=0

for file in tests/*.c;
do
	echo "Running tests: $file"
	gcc -std=c11 -Wall "$file" flags.c glob.c mathop.c mem.c parse.c stack.c
	./a.out

	if [ $? -eq 1 ]
	then
		echo "Unit test fail: $file"
		fail=$(( $fail + 1 ))
	fi
done

rm a.out

echo
echo "Total failed test cases: $fail"