#!/bin/sh

# EQ7 from
# http://puredata.hurleur.com/sujet-1795-parametric-got

[ $# -eq 1 ] || { echo >&2 "Usage: $0 <file.pd>"; exit 1; }

PD_EXTENDED_LIBS="/usr/lib/pd-extended/extra"
LOCAL_LIBS="/usr/local/lib/pd"

LIBRARIES="-path ./lib"
LIBRARIES="$LIBRARIES -path $LOCAL_LIBS/pduino" # arduino
LIBRARIES="$LIBRARIES -path $PD_EXTENDED_LIBS/tof" # folderpanel

if pgrep jackd 2>&1 > /dev/null; then
    SAMPLERATE=$(jack_samplerate)
    pd-extended $LIBRARIES -r $SAMPLERATE -jack $1
else
    pd-extended $LIBRARIES -r 48000 -alsa $1
fi
