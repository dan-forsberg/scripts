#!/bin/zsh
if ! screen -r -D cmus >/dev/null; then 
    screen -S cmus /usr/bin/cmus "$@"
    cmus-remote -C 'set output_plugin=pulse'
fi
