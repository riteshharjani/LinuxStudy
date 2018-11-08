#Random Read
./tiotest -t 8 -f 100 -d /data/ -b 4096 -k 0 -k 1 -k 2 --direct-io -S
 
#Random Write
./tiotest -t 8 -f 100 -d /data/ -b 4096 -k 0 -k 2 -k 3 --direct-io -S
 
#Seq Read
./tiotest -t 8 -f 100 -d /data/ -b 1048576 -k 0 -k 1 -k 3 --direct-io -S
 
#Seq Write
./tiotest -t 8 -f 100 -d /data/ -b 1048576 -k 1 -k 2 -k 3 --direct-io -S
