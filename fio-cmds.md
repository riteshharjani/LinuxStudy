### Below fio command for long duration write (2 files)
fio --iodepth=8 --ioengine=sync --size=1024M --time_based \
  --rate=1256k --bs=14K --name=quick --runtime=1000000m --name=mixed \
  --runtime=2000000m --rw=rw
