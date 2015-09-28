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
				for f in $(find -O3 -P "$DIR" -type f -executable | sort -n); do
					"$f" "$state" || echo "File $f exited with error $?"
				done
			;;
		esac
	else
		sleep 10
	fi
done
