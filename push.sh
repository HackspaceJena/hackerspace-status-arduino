#!/bin/sh -e

DEV='/dev/ttyUSB0'
DIR='push.d'
stty -F "$DEV" 9600 -brkint -icrnl -imaxbel -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke

if [ -c	 "$DEV" ]; then
	sleep 2 && echo '?' >> "$DEV" &
fi

while true; do
	if [ -c	 "$DEV" ]; then
		read state < "$DEV"
		case "$state" in
			ON|HALF|OFF)
				run-parts --arg="$state" "$DIR"
			;;
		esac
	else
		sleep 10
	fi
done
