#!/bin/bash

MOUNT_POINT=/mnt1/test/
SWAPFILE=/mnt1/test/swapfile
LOOP_DEVICE=/dev/loop2
FILE=/mnt1/testfs

test_clean() {
        umount $MOUNT_POINT
        losetup -d $LOOP_DEVICE
}

test_losetup() {
        if [ ! -d $MOUNT_POINT ]; then
                mkdir -p $MOUNT_POINT
        fi
        if [ ! -f $FILE ]; then
                fallocate -o 0 -l 1G $FILE
        fi
        losetup $LOOP_DEVICE $FILE
        mkfs.xfs -f -b size=1024 $FILE
        mount $LOOP_DEVICE $MOUNT_POINT
}

test_swapsetup() {
        fallocate -o 0 -l $((65536+65536+1024+1024)) $SWAPFILE
        xfs_io -c "fcollapse 64512 1024" $SWAPFILE
        xfs_io -c "fcollapse 129024 1024" $SWAPFILE
}

test_run() {
#       /home/qemu/work-tools/xfstests/src/mkswap $SWAPFILE
#       /home/qemu/work-tools/xfstests/src/swapon $SWAPFILE
        mkswap $SWAPFILE
        swapon $SWAPFILE
        swapoff $SWAPFILE
}

test_clean
test_losetup
test_swapsetup
test_run
