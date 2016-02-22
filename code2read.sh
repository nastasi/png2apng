#!/bin/bash
out="$1"
IFS='
'
ct=0
for i in $(cat | sed 's/^ *//g;s/ \+/ /g'); do
    ty="$(echo "$i" | cut -d ' ' -f 1)"
    if [ "$ty" = "uint32_t" ]; then
        delta=4
    elif [ "$ty" = "uint16_t" ]; then
        delta=2
    elif [ "$ty" = "uint8_t" ]; then
        delta=1
    fi
    name="$(echo "$i" | cut -d ' ' -f 2 | sed 's/;$//g')"
    echo "    memcpy(&(${out}->${name}), &(pc->data[${ct}]), ${delta});"
    if [ $delta -eq 4 ]; then
        echo "    ${out}->${name} = ntohl(${out}->${name});"
    elif [ $delta -eq 2 ]; then
        echo "    ${out}->${name} = ntohs(${out}->${name});"
    fi
    ct=$((ct + delta))
done
