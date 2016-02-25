#!/bin/bash
#  set -x
out="$1"
outl="$(echo "$out" | tr A-Z a-z)"
func="$2"
IFS='
'
ct=0
if [ "$func" = "write" ]; then
    echo "memcpy(&(bf[0]), \"$out\", 4);"
    ct=4
fi
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
    if [ "$func" = "read" ]; then
        echo "    memcpy(&(${outl}->${name}), &(pc->data[${ct}]), ${delta});"
        if [ $delta -eq 4 ]; then
            echo "    ${outl}->${name} = ntohl(${outl}->${name});"
        elif [ $delta -eq 2 ]; then
            echo "    ${outl}->${name} = ntohs(${outl}->${name});"
        fi
    elif [ "$func" = "write" ]; then
        if [ $delta -eq 4 ]; then
            echo "*((uint32_t *)(&bf[${ct}])) = htonl(${outl}->${name});"
        elif [ $delta -eq 2 ]; then
            echo "*((uint16_t *)(&bf[${ct}])) = htonl(${outl}->${name});"
        elif [ $delta -eq 1 ]; then
            echo "*((uint8_t *)(&bf[${ct}])) = htonl(${outl}->${name});"
        fi
    fi
    ct=$((ct + delta))
done
