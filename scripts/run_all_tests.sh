#!/bin/bash

########################################################################################################################
# This script is intended to be run from within the build directory                                                    #
# such that the tests are located ./bin/ut and ./bin/ft                                                                #
########################################################################################################################

# Tput color codes
FGRED=$(tput setaf 1)
FGGREEN=$(tput setaf 2)
FGYELLOW=$(tput setaf 3)
FGMAGENTA=$(tput setaf 5)
BGRED=$(tput setab 1)
BGGREEN=$(tput setab 2)
BGYELLOW=$(tput setab 3)
BGMAGENTA=$(tput setab 5)
NORMAL=$(tput sgr0) # Resets all attributes

# -------------------------------------------------------------------------------------------------------------------- #
# Determine which tests should fail, and for which tests we should ignore the valgrind information                     #
# -------------------------------------------------------------------------------------------------------------------- #

# Get list of all tests which are expected to fail
expected_failure_list=()
expected_failure_list_file="tests_expected_failure_list.txt"
if [ -f "$expected_failure_list_file" ]; then
    mapfile -t expected_failure_list < "$expected_failure_list_file"
fi

# Get list of all tests which should ignore memory failures
ignore_memory_list=()
ignore_memory_list_file="tests_ignore_memory_list.txt"
if [ -f "$ignore_memory_list_file" ]; then
    mapfile -t ignore_memory_list < "$ignore_memory_list_file"
fi

# -------------------------------------------------------------------------------------------------------------------- #
# Determine which tests to run, and run them all, making note of which ones failed and in what fashion                 #
# -------------------------------------------------------------------------------------------------------------------- #

# Create list of all tests from the ft and ut directories
uts_to_run=(./bin/ut/*)
fts_to_run=(./bin/ft/*)
all_tests_to_run=("${uts_to_run[@]}" "${fts_to_run[@]}")

# These look bad when declared here but they are all aligned for nicely printing
P_EF="${FGRED}PASSED (should fail)${NORMAL}"
ME_EF="${FGYELLOW}MEMERR (should fail)${NORMAL}"
F_EF="failed (should fail)${NORMAL}"
P_EP="passed (should pass)${NORMAL}"
ME_EP="${FGYELLOW}MEMERR (should pass)${NORMAL}"
F_EP="${FGRED}FAILED (should pass)${NORMAL}"

exit_code=0

# Do the thing (execute all tests, and take note of failed commands)
failed_commands=()
for test_command in "${all_tests_to_run[@]}"; do
    test_name=$(basename "$test_command")

    if [[ "$test_name" == "*" ]]; then
        continue
    fi

    command_to_run="$test_command"

    if [[ ! " ${ignore_memory_list[*]} " =~ " ${test_name} " ]]; then
        command_to_run="valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=54 $command_to_run"
    fi

    $command_to_run > /dev/null 2>&1
    status=$?

    result_message="PASSED"
    if [[ " ${expected_failure_list[*]} " =~ " ${test_name} " ]]; then # If we should have failed
        if [ "$status" -eq 0 ]; then
            result_message=$P_EF
            failed_commands+=( "$command_to_run" )
        elif [ "$status" -eq 54 ]; then
            result_message=$ME_EF
            failed_commands+=( "$command_to_run" )
        else
            result_message=$F_EF
        fi
    else # Else (we should have succeeded)
        if [ "$status" -eq 0 ]; then
            result_message=$P_EP
        elif [ "$status" -eq 54 ]; then
            result_message=$ME_EP
            failed_commands+=( "$command_to_run" )
        else
            result_message=$F_EP
            failed_commands+=( "$command_to_run" )
        fi
    fi

    echo "$result_message : $test_name"
done

echo ""

if [ "${#failed_commands[@]}" -eq 0 ]; then
    echo "${FGGREEN}All tests behaved expectedly!${NORMAL}"
    exit 0
fi

echo "${FGRED}Failed${NORMAL} commands:"
for failed_command in "${failed_commands[@]}"; do
    echo "  $failed_command"
done

exit 1
