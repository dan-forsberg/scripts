#!/bin/python
import dbus
from subprocess import getoutput
from subprocess import run

returncode = run("[ ! -z \"$(pidof spotify)\" ]", shell=True).returncode

if returncode == 0:
    session_bus = dbus.SessionBus()
    spotify_bus = session_bus.get_object("org.mpris.MediaPlayer2.spotify",
                                         "/org/mpris/MediaPlayer2")
    spotify_properties = dbus.Interface(spotify_bus,
                                        "org.freedesktop.DBus.Properties")
    metadata = spotify_properties.Get("org.mpris.MediaPlayer2.Player",
                                      "Metadata")
    title = metadata['xesam:title']
    artist = metadata['xesam:artist'][0]
    print(artist + " - " + title);

else:
    artist = getoutput("cmus-remote -Q | grep -w 'tag artist' | cut -d \" \" -f 3-")
    title = getoutput("cmus-remote -Q | grep -w 'tag title' | cut -d \" \" -f 3-")
    print(str(artist) + " - " + str(title));
