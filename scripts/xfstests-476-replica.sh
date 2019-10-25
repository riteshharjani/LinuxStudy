#!/bin/bash

# This test is a small replica of xfstests 476 for 1024 blocksize
#./this_test 0
#./this_test 1
#./this_test 2

SCRATCH_DEV=/dev/loop7
SCRATCH_DIR=/mnt1/scratch
BLOCK_SIZE=4096
THREADS=24
OPS=100000
FSTYP=ext4

function run_test()
{
        unmount_fs
        i=0
        while [ $i ]; do
                echo "iter=$i"
                mount_fs
                i=$((i+1))
                ltp/fsstress -w -d $SCRATCH_DIR -n $OPS -p $THREADS
                unmount_fs # It is required to run fsck after unmount so that there is no dirty data and proper unmount
                fsck -t $FSTYP -nf $SCRATCH_DEV
                ret=$?
                if [ $ret -ne 0 ]
                then
                        echo "error returned from fsck = $ret"
                        echo "Press Enter to continue..."
                        read dummy
                else
                        echo "return=$ret"
                fi
                sleep 1
        done
        
}


function unmount_fs()
{
        umount $SCRATCH_DEV
        mount |grep loop
}

function mount_fs()
{
    case $FSTYP in
        xfs)
            echo "xfs filesystem"
            mkfs.xfs -f -b size=$BLOCK_SIZE $SCRATCH_DEV
            mount -t xfs $SCRATCH_DEV $SCRATCH_DIR
            mount |grep loop
            ;;
        ext4)
            echo "Testing ext4 fs"
            mkfs.ext4 -F -b $BLOCK_SIZE $SCRATCH_DEV
            mount -t ext4 $SCRATCH_DEV $SCRATCH_DIR
            mount |grep loop
            ;;
        *)
            echo "unknown fs"
            exit
            ;;
    esac
}

arr=(unmount_fs mount_fs run_test)

i=$1
echo "running ${arr[$i]}"
cmd=${arr[$i]}
$cmd
