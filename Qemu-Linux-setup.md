 ## Commands:-
 
 1. sudo qemu-system-x86_64 -m 2048 -hda hda-bootstrap-f2fs.img -kernel bzImage -initrd ../initramfs.gz --enable-kvm --nographic -append "console=ttyS0" -s -device e1000,netdev=net0 -netdev user,id=net0,hostfwd=tcp::5555-:22
 give -S if you want to wait for gdb to come up.
 
 2. gdb -ex 'target remote localhost:1234' -ex c vmlinux
 
### To create a disk:-
IMG=qemu-image.img
DIR=mount-point.dir
qemu-img create $IMG 1g
mkfs.f2fs $IMG
mkdir $DIR
sudo mount -o loop $IMG $DIR
sudo debootstrap --arch amd64 jessie $DIR
sudo umount $DIR
rmdir $DIR
 
  ### References:-
 1. https://www.collabora.com/news-and-blog/blog/2017/01/16/setting-up-qemu-kvm-for-kernel-development/
 2. http://lockett.altervista.org/linuxboot/linuxboot.html
 3. https://en.wikibooks.org/wiki/QEMU/Images
 
