#!/bin/bash

if [ $# -eq 0 ]; then
    echo "You must provide an executable (and optionally its arguments) to run via command line arguments"
    exit 1
fi

while true; do
    clear

    echo "Executing: $@"

    "$@"

    if [ $? -ne 0 ]; then
        break
    fi
done
