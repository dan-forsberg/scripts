#!/bin/bash

NAME=$(date +%y%m%d-%H%M%S).png

if [[ ! -d /tmp/imgs ]]; then
	mkdir /tmp/imgs
fi;

maim -s /tmp/imgs/$NAME $@
EXIT_CODE=$?
if [[ $EXIT_CODE != 0 ]]; then
	notify-send "unable to SS (${EXIT_CODE})"
	exit $EXIT_CODE
fi

scp /tmp/imgs/$NAME ubuntu@vps:/var/www/html/img
EXIT_CODE=$?
if [[ $EXIT_CODE != 0 ]]; then
	notify-send "unable to scp (${EXIT_CODE})"
	exit $EXIT_CODE
fi

notify-send "SCP complete."

echo -n "https://domain/img/$NAME" | xclip -selection c
