#!/bin/bash

set -e
set -x
#DEV="pmem1s"
DEV="loop0"
BS=65536
#BS=4096
MNT_DEVICE="/dev/$DEV"
echo $MNT_DEVICE
avail_size=0
THREADS=80
FACT=0.80
FSIZE=0

function setup_mount() {
	set +e
	echo ======= setting up mount ====================
	umount mnt/
	mkfs.ext4 -F -b $BS /dev/loop0
	mount -t ext4 /dev/loop0 mnt/
	set -e
}
function cal_size() {
	tot_avail_size=$(df --block-size=1| awk -v dev="$DEV" '$0 ~ dev { print $4 }')
	avail_size=$(echo $tot_avail_size*$FACT | bc)
	FSIZE=$(echo $avail_size/$THREADS | bc)
	echo "tot=$tot_avail_size, avail=$avail_size, fsize=$FSIZE"
}

function build_test() {
	gcc test-ext4-mballoc.c -lpthread -o test_ext4_mb
}



function run_test() {
	i=1
	while [ $i ]
	do
	    echo "================= ITER $i ======================"
	    sleep 1
#	    dmesg -c >> dmesg-newlogs.txt
	    i=$(echo $i+1 |bc)
	    umount mnt
	    mkfs.ext4 -F -b $BS $MNT_DEVICE
	    mount -t ext4 $MNT_DEVICE mnt/
	    cd mnt/
	    cp ../test_ext4_mb .
	    ./test_ext4_mb -t $THREADS -s $FSIZE
	    ret=$?
#	    dmesg -c >> dmesg-newlogs.txt
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
}

setup_mount
cal_size
build_test
run_test
exit
