#!/bin/bash

if [ ! -f testfile ]; then
	echo "No testfile"
	exit 0
fi

if [ ! -d /tmp/ ]; then
	echo "No tmp folder"
	exit 0
fi

if [ ! -d /tmp/fiofiles ]; then
	mkdir /tmp/fiofiles
fi


T1="/tmp/fiofiles/randw/run_4K"
T2="/tmp/fiofiles/randw/run_1M"

T3="/tmp/fiofiles/randrw/run_4K"
T4="/tmp/fiofiles/randrw/run_1M"


mkdir -p $T{1..4}
if [ ! -d $T1 ] || [ ! -d $T4 ]; then
	echo "$T1 || $T4 not created"
	exit 0
fi

threads=(1 2 4 8 12 16 24)
uidx=$(date '+%d-%m-%Y-%H-%M-%S')

mount |grep ext4
# Randwrites-4K
echo "************* TEST: Randwrites-4K**********"
for thread in "${threads[@]}"
do
	iter=1
	while [ $iter -ne 4 ]; do
		echo "iter: $iter"
		fio -name=fio-run -filename=./testfile -direct=1 -iodepth=1 -thread -rw=randwrite -ioengine=psync -bs=4K -size=10G -numjobs=$thread -group_reporting=1 -runtime=120 -o "$T1/fio-4K-$iter-$thread-$uidx"
		echo ""
		#touch $T1/fio-4K-$iter-$thread-$uidx
		let iter=$iter+1
	done
done

#Randwrites-1M
echo "************* TEST: Randwrites-1M**********"
for thread in "${threads[@]}"
do
	iter=1
	while [ $iter -ne 4 ]; do
		echo "iter: $iter"
		fio -name=fio-run -filename=./testfile -direct=1 -iodepth=1 -thread -rw=randwrite -ioengine=psync -bs=1M -size=10G -numjobs=$thread -group_reporting=1 -runtime=120 -o "$T2/fio-1M-$iter-$thread-$uidx"
		echo ""
		#touch $T2/fio-1M-$iter-$thread-$uidx
		let iter=$iter+1
	done
done


#Randrw-4K
echo "************* TEST: Randread-write-4K**********"
for thread in "${threads[@]}"
do
	iter=1
	while [ $iter -ne 4 ]; do
		echo "iter: $iter"
		fio -name=fio-run -filename=./testfile -direct=1 -iodepth=1 -thread -rw=randrw -ioengine=psync -bs=4K -size=10G -numjobs=$thread -group_reporting=1 -runtime=120 -o "$T3/fio-4K-$iter-$thread-$uidx"
		echo ""
		#touch $T3/fio-4K-$iter-$thread-$uidx
		let iter=$iter+1
	done
done

#Randrw-1M
echo "************* TEST: Randread-write-1M**********"
for thread in "${threads[@]}"
do
	iter=1
	while [ $iter -ne 4 ]; do
		echo "iter: $iter"
		fio -name=fio-run -filename=./testfile -direct=1 -iodepth=1 -thread -rw=randrw -ioengine=psync -bs=1M -size=10G -numjobs=$thread -group_reporting=1 -runtime=120 -o "$T4/fio-1M-$iter-$thread-$uidx"
		echo ""
		#touch $T4/fio-1M-$iter-$thread-$uidx
		let iter=$iter+1
	done
done


mkdir -p fio-output/v5-rc5/dioread-nolock/
mv /tmp/fiofiles fio-output/v5-rc5/dioread-nolock/
sync

echo "Test Completed Successfully"
