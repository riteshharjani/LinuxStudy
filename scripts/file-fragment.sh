#!/bin/bash                                                                         
#set -x
fsize=$(stat --printf="%s" testfile)
off=4096
len=4096                                                                            
file="testfile2"                                                                    

echo "filefrag info before fpunch"
filefrag -v $file

# this loop iterates over file offsets and fpunches the alternates file blocks
# this will keep the file size as same but will creates holes at the given offsets within the file
while [ $off -lt $fsize ]; do                                                       
        xfs_io -c "fpunch $off $len" $file
        off=$(($off+2*$len))
done

echo "filefrag info after fpunch"
filefrag -v $file
