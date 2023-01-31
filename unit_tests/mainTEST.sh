#!/usr/bin/env sh
(cd .. ; make);
cat griddata.asc ; ../core/pickpeak -i griddata.asc -m 0 -R 5000 -o peaks.json;
cat peaks.json | jq '.features[] | .geometry + .properties | del(.type)' -c
