#!/bin/bash

# this script is mainly to test test-ext4-mballoc code.
# it seems if we have 2x cpu threads. Then we could see the failure
# very easily. LOGS:-
# ./test.sh: line 26: 39932 Bus error               (core dumped) ./a.out -t 96
# riteshh@linux:~> df -h
# Filesystem      Size  Used Avail Use% Mounted on
#/dev/loop0      176M  6.9M  152M   5% /home/riteshh/mnt


set -e
i=1
while [ $i ]
do
    echo "================= ITER $i ======================"
    let i=$i+1
    umount mnt
    mkfs.ext4 -F -b 65536 /dev/loop0
    mount -t ext4 /dev/loop0 mnt/
    cd mnt/
    cp ../a.out .
    ./a.out -t 96
    ret=$?
    if [ $ret -eq 0 ]
    then
        echo "Test passed\n\n"
#	break
    else
        echo "Test failed\n\n"
        break
    fi
    cd ..
    echo "\n\n"
done

