#!/bin/sh

case "$1" in

	start)
		start-stop-daemon -S -n aesdsocket -a /usr/bin/aesdsocket -- -d
		
		if [ $? -eq 0 ]; then
			echo "Server started"
		else
			echo "Server failed to start"
			exit 1
		fi 
		
		;;
		
	stop)
		start-stop-daemon -K -n aesdsocket
		
		if [ $? -eq 0 ]; then
			echo "Server halted"
		else
			echo "Server failed to halt"
			exit 1
		fi
		
		;;
		
esac

exit 0
