#!/usr/bin/env sh
testdata=("griddata2.asc" "griddata.asc")
margin=(1 0)

for i in 0 1 ; do
    data="${testdata[$i]}"
    m=${margin[$i]}
    (rm "peaks.json") >/dev/null 2>&1;
    (cd .. ; make) &&
    echo margin="$m";
    cat "$data";
    if ("../pickpeak" -i "$data" -m "$m" -R 3 -o peaks.json)
    then
        jq '.features[] | .geometry + .properties | del(.type)' -c < peaks.json

    fi
done
