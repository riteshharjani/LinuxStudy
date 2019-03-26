## For xfstests-dev
1. ./configure LDFLAGS=-L/local/mnt/workspace/oldhdd-backup/rharjani/all_builds/opensource/xfs/xfsprogs-dev/myout/lib/ CPPFLAGS=-I/local/mnt/workspace/oldhdd-backup/rharjani/all_builds/opensource/xfs/xfsprogs-dev/include/ --enable-static --prefix=/local/mnt/workspace/oldhdd-backup/rharjani/all_builds/opensource/xfs/xfstests-dev/myout/
2. make
3. make install

## For xfsprogs-dev
1. ./configure --enable-static --prefix=<path>/myout/
2. make
3. make install

