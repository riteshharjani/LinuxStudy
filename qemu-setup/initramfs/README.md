## Extract & Pack Ramdisk
cp ramdisk.img ramdisk.cpio.gz
gzip -d ramdisk.cpio.gz
mkdir ramdisk
cd ramdisk
cp -Rfp ../ramdisk.cpio .
cpio -i -F ramdisk.cpio
rm ramdisk.cpio
ls should show all the files in ramdisk. From here on add files as you like in ramdisk
Use the following commands based on your folder
cp <path>/app_to_be_added_to_ramdisk  <path>/ramdisk/
cd <path>/ramdisk
find . | cpio -o -H newc | gzip > ../ramdisk.img (you ramdisk.img will get created in ../)
