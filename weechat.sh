#!/bin/zsh
if ! screen -r -D weechat >/dev/null; then 
    screen -S weechat /usr/bin/weechat "$@"
fi
