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
# Get CL argument for whether or not we should pre-document the executable we are about to run                         #
# -------------------------------------------------------------------------------------------------------------------- #

should_pre_print=0
while getopts "v" opt; do
    case ${opt} in
        v)
            should_pre_print=1
            ;;
        \?)
            exit 1
            ;;
    esac
done

# -------------------------------------------------------------------------------------------------------------------- #
# Error and success messages, need the pre print flag for spacing                                                      #
# -------------------------------------------------------------------------------------------------------------------- #

# These look bad when declared here but they are all aligned for nicely printing
P_EF="${FGRED}PASSED      (should fail)${NORMAL}"
ME_EF="${FGYELLOW}MEMERR      (should fail)${NORMAL}"
F_EF="${NORMAL}failed      (should fail)${NORMAL}"
P_EP="${NORMAL}passed      (should pass)${NORMAL}"
ME_EP="${FGYELLOW}MEMERR      (should pass)${NORMAL}"
F_EP="${FGRED}FAILED      (should pass)${NORMAL}"

if [ $should_pre_print -eq 0 ]; then
    P_EF="${FGRED}PASSED (should fail)${NORMAL}"
    ME_EF="${FGYELLOW}MEMERR (should fail)${NORMAL}"
    F_EF="${NORMAL}failed (should fail)${NORMAL}"
    P_EP="${NORMAL}passed (should pass)${NORMAL}"
    ME_EP="${FGYELLOW}MEMERR (should pass)${NORMAL}"
    F_EP="${FGRED}FAILED (should pass)${NORMAL}"
fi

# -------------------------------------------------------------------------------------------------------------------- #
# Where we actually store the results (initialize an empty list)                                                       #
# -------------------------------------------------------------------------------------------------------------------- #

failed_commands=()

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
# Helper function to actually run the test and check the error result                                                  #
# -------------------------------------------------------------------------------------------------------------------- #

run_test_update_failed_commands () {
    local arg_test_command="$1"
    local arg_command_to_run="$2"
    local arg_is_valgrind="$3"

    local arg_test_name=$(basename "$arg_test_command")

    # Align this output with our error/pass result outputs
    if [ $should_pre_print -eq 1 ]; then
      if [ "$arg_is_valgrind" -eq 0 ]; then
          #    "passed      (should pass) : "
          echo "executing                 : $arg_test_name"
      else
          #    "passed      (should pass) : "
          echo "valgrinding               : $arg_test_name"
      fi
    fi

    $command_to_run > /dev/null 2>&1
    local status=$?

    local did_expected=1
    local result_message="PLACEHOLDER_RESULT"
    if [[ " ${expected_failure_list[*]} " =~ " ${arg_test_name} " ]]; then # If we should have failed
        if [ "$status" -eq 0 ]; then
            result_message=$P_EF
            failed_commands+=( "$command_to_run" )
            did_expected=0
        elif [ "$status" -eq 54 ]; then
            result_message=$ME_EF
            failed_commands+=( "$command_to_run" )
            did_expected=0
        else
            result_message=$F_EF
        fi
    else # Else (we should have succeeded)
        if [ "$status" -eq 0 ]; then
            result_message=$P_EP
        elif [ "$status" -eq 54 ]; then
            result_message=$ME_EP
            failed_commands+=( "$command_to_run" )
            did_expected=0
        else
            result_message=$F_EP
            failed_commands+=( "$command_to_run" )
            did_expected=0
        fi
    fi
   
    if [ "$arg_is_valgrind" -eq 0 ]; then
        echo "$result_message : $arg_test_name"
    else
        echo "$result_message : $arg_test_name - valgrind"
    fi

    if [ "$did_expected" -eq 1 ]; then
        return 0
    else
        return 1
    fi
}

# -------------------------------------------------------------------------------------------------------------------- #
# Determine which tests to run, and run them all, making note of which ones failed and in what fashion                 #
# -------------------------------------------------------------------------------------------------------------------- #

# Create list of all tests from the ft and ut directories
uts_to_run=(./bin/ut/*)
fts_to_run=(./bin/ft/*)
all_tests_to_run=("${uts_to_run[@]}" "${fts_to_run[@]}")

# Do the thing (execute all tests, and take note of failed commands)
for test_command in "${all_tests_to_run[@]}"; do
    test_name=$(basename "$test_command")

    if [[ "$test_name" == "*" ]]; then
        continue
    fi

    # ------------------------------------------------------------------------------------------------------ #
    # First pass: run the test without valgrind to ensure the logic is valid                                 #
    # ------------------------------------------------------------------------------------------------------ #
    
    command_to_run="$test_command"

    run_test_update_failed_commands "$test_command" "$command_to_run" 0

    passed=$?

    if [ $passed -eq 1 ]; then
        continue
    fi

    # ------------------------------------------------------------------------------------------------------ #
    # Second pass: run the test with valgrind to ensure the memory usage is valid                            #
    # ------------------------------------------------------------------------------------------------------ #

    if [[ ! " ${ignore_memory_list[*]} " =~ " ${test_name} " ]]; then
        command_to_run="valgrind --tool=memcheck --enable-debuginfod=no --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --error-exitcode=54 $command_to_run"

        run_test_update_failed_commands "$test_command" "$command_to_run" 1
    fi
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
