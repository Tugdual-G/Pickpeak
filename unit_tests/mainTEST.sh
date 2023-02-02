#!/usr/bin/env sh
testdata=("griddata2.asc" "griddata.asc")
margin=(1 0)
radius=(2.5 3)

(cd .. ; make) &&
for i in 0 1 ; do
    data="${testdata[$i]}"
    m=${margin[$i]}
    R=${radius[$i]}
    (rm "peaks.json") >/dev/null 2>&1;
    echo "########### DATA ##############"
    cat "$data";
    echo "########## COMMAND ############"
    echo "../pickpeak" -i "$data" -m $m -R $R -o peaks.json
    echo "########## RESULT #############"
    if ("../pickpeak" -i "$data" -m $m -R $R -o peaks.json)
    then
        jq '.features[] | .geometry + .properties | del(.type)' -c < peaks.json

    fi
done
