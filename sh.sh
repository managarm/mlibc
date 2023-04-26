for x in $(find *); do
       if test -f $x; then	
	unset con
	con=$(cat $x | grep strftime) 
	if test -n "$con"; then
		echo $x 
	fi
       fi
done
