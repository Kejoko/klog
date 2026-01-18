#!/bin/bash

# This script is intended to be run from within the build directory
# such that the tests are located ./bin/ut and ./bin/ft

# Get list of all tests which are expected to fail
expected_failure_list=()
expected_failure_list_file="tests_expected_failure_list.txt"
if [ -f "$expected_failure_list_file" ]; then
    echo "$expected_failure_list_file exists"

    mapfile -t expected_failure_list < "$expected_failure_list_file"
fi
echo "Expected tests to fail:"
for test_name in "${expected_failure_list[@]}"; do
    echo "  $test_name"
done

# Get list of all tests which should ignore memory failures
ignore_memory_list=()
ignore_memory_list_file="tests_ignore_memory_list.txt"
if [ -f "$ignore_memory_list_file" ]; then
    echo "$ignore_memory_list_file exists"

    mapfile -t ignore_memory_list < "$ignore_memory_list_file"
fi
echo "Ignoring memory for tests:"
for test_name in "${ignore_memory_list[@]}"; do
    echo "  $test_name"
done

# Create list of all tests from the ft and ut directories
uts_to_run=(./bin/ut/*)
fts_to_run=(./bin/ft/*)
all_tests_to_run=("${uts_to_run[@]}" "${fts_to_run[@]}")
failed_tests=()
echo "Running tests:"
for test_command in "${all_tests_to_run[@]}"; do
    test_name=$(basename "$test_command")
    
    if [[ "$test_name" == "*" ]]; then
        continue
    fi

    command_to_run="$test_command"

    if [[ ! " ${ignore_memory_list[*]} " =~ " ${test_name} " ]]; then
        command_to_run="valgrind --tool=memcheck --leak-check=full --error-exitcode=54 $command_to_run"
    fi

    $command_to_run > /dev/null 2>&1
    status=$?
    
    if [[ " ${expected_failure_list[*]} " =~ " ${test_name} " ]]; then # If we should have failed
        # The error code should not be the valgrind code
        if [ "$status" -eq 0 ]; then
            echo "    Expected failure - but got pass           ($status) - $test_name"
        elif [ "$status" -eq 54 ]; then
            echo "    Expected failure - got memory error       ($status) - $test_name"
        else
            echo "    Expected failure - correct                ($status) - $test_name"
        fi
    else # Else (we should have succeeded)
        # The error code should be 0
        if [ "$status" -eq 0 ]; then
            echo "    Expected pass    - correct                ($status) - $test_name"
        elif [ "$status" -eq 30305 ]; then
            echo "    Expected pass    - but got memory error   ($status) - $test_name"
        else
            echo "    Expected pass    - but got failure        ($status) - $test_name"
        fi
    fi
done

# Create empty list of failed tests

# For each test
    # Run the test and output information

# Output all failed tests
