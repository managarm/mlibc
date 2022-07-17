#!/bin/bash
# USAGE: put files and ports in the arrays below and export the abi you want to use


declare -a files=()
declare -a ports=()

for file in "${files[@]}"; do
	for port in "${ports[@]}"; do
	    ln -rsiv abis/$abi/$file sysdeps/$port/include/abi-bits/$file
	done
done
