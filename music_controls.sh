#!/bin/bash

if [ $# -eq 0 ]; then
    echo "acceptable commands: play-pause, next and previous info"
    exit 1
fi

if [[ "$(pidof spotify)" ]]; then 
    playerctl --player=spotify $1
elif [[ "$(pidof cmus)" ]]; then 
    if [ "$1" = "play-pause" ]; then
        cmus-remote --pause
        #notify-send "$(cmus-remote -Q | grep -Ew 'tag (artist|title)' | cut -d ' ' -f 3-)"
    elif [ "$1" = "next" ]; then
        cmus-remote --next
        #notify-send "$(cmus-remote -Q | grep -Ew 'tag (artist|title)' | cut -d ' ' -f 3-)"
    elif [ "$1" = "previous" ]; then
        cmus-remote --prev
    elif [ "$1" = "info" ]; then
        notify-send "$(cmus-remote -Q | grep -Ew 'tag (artist|title)' | cut -d ' ' -f 3-)"
    fi
fi
