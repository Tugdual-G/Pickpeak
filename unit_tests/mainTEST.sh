#!/usr/bin/env sh
data="griddata2.asc"
margin=1
(cd .. ; make) &&
echo margin="$margin";
cat "$data"; "../core/pickpeak" -i "$data" -m "$margin" -R 3 -o peaks.json;
jq '.features[] | .geometry + .properties | del(.type)' -c < peaks.json
