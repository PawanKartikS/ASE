#!/bin/bash

echo
echo "Running unit tests"

fail=0

for file in Tests/*.c;
do
	echo "Running tests: $file"
	gcc -std=c11 -Wall "$file" Flags/flags.c Glob/glob.c Math/mathop.c Memory/mem.c Parser/parse.c Stack/stack.c
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