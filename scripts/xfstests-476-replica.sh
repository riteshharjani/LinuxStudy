#!/bin/bash

# This test is a small replica of xfstests 476 for 1024 blocksize
#./this_test 0
#./this_test 1
#./this_test 2


function run_test()
{
        i=0
        while [ $i ]; do
                echo "iter=$i"
                i=$((i+1))
                ltp/fsstress -w -d /mnt1/scratch -n 10000 -p 8 
                fsck -t ext4 -nf /dev/loop3
                ret=$?
                if [ $ret -ne 0 ]
                then
                        echo "error returned from fsck = $ret"
                        echo "Press Enter to continue..."
                        read dummy
                fi
                sleep 1
        done
        
}


function unmount_fs()
{
        umount /dev/loop3
}

function mount_fs()
{
        sudo mkfs.ext4 -F -b 1024 /dev/loop3
        sudo mount -t ext4 /dev/loop3 /mnt1/scratch
}

arr=(unmount_fs mount_fs run_test)

i=$1
echo "running ${arr[$i]}"
cmd=${arr[$i]}
$cmd
