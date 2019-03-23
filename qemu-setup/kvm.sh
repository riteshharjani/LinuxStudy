if [ "$#" -ne 4 ]; then
  echo "Usage: $0 <kernel_bzImage> <initramfs> <shared_path> <ext4_image>" >&2
  exit 1
fi
KERNEL=$1
INITRAMFS=$2
SHARED=$3
EXT4_IMG=$4

kvm -kernel $KERNEL  -initrd $INITRAMFS  -nographic -name "EXT4_IMG" -m 1024 \
-append "console=ttyAMA0,115200 console=tty highres=off console=ttyS0"\
 -enable-kvm \
 -s \
 -fsdev local,id=shared_test_dev,path=$SHARED,security_model=none -device virtio-9p-pci,fsdev=shared_test_dev,mount_tag=host_shared \
 -hda $EXT4_IMG  -device e1000,netdev=user.0 -netdev user,id=user.0,hostfwd=tcp::5555-:23
 # -serial stdio -serial telnet:localhost:4444,server,nowait
