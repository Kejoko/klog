#! /bin/bash

# Usage:
# Run this in the root directory of the project (like: ./scripts/format.sh),
# so this script can use the uncrustify.cfg file correctly

format_directory() {
    local dir=$1
    local files=$(find "$dir" -type f \( -name "*.c" -o -name "*.h" \))
    local count=$(echo "$files" | wc -w)
    
    if [ $count -eq 0 ]; then
        return
    fi
    
    list_flag="-F"
    if [ $count -eq 1 ]; then
        list_flag=""
    fi
    
    uncrustify --replace --no-backup -c uncrustify.cfg $list_flag $files
}

format_directory include/
format_directory src/
