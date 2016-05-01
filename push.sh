#!/bin/sh -e

DEV='/dev/ttyUSB0'
DIR='push.d'
stty -F "$DEV" 9600 -brkint -icrnl -imaxbel -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke

# Warte bis Device $DEV vorhanden ist
while [ ! -c "$DEV" ]; do
	sleep 2
done
# sende beliebiges Zeichen an $DEV
echo '?' >> "$DEV" &

while true; do
	# wenn $DEV vorhanden
	if [ -c	 "$DEV" ]; then
		# dann lese zustand
		read -r state < "$DEV"
		case "$state" in
			ON|HALF|OFF)
				run-parts --arg="$state" "$DIR"
			;;
		esac
	else
		# sonst warte
		sleep 10
	fi
done
