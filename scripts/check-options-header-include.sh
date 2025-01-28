#!/bin/bash

shopt -s lastpipe

errors_found=0

find . -wholename '*include/*.h' -print0 | while read -rd $'\0' file; do
	uses=$(grep -c -E "__MLIBC_(ANSI|BSD|POSIX|LINUX|GLIBC)_OPTION" "$file")
	if [ "$uses" -ne 0 ]; then
		does_include=$(grep -c "#include <mlibc-config.h>" "$file")
		if [ "$does_include" -eq 0 ]; then
			echo "'$file' does not include mlibc-config.h while it does use mlibc option macros"
			errors_found+=1
		fi
	fi
done

exit $errors_found
