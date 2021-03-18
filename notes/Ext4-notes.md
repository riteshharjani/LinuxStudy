# Linux notes

## Experiments with tools

### links

### useful cmds
1. perf cmds

<details>
<summary>I could use some help...</summary>

```c#
public class Order
{
    public int OrderId { get; set; }
    public int CustomerId { get; set; }

    public List<int> Products { get; set; }
}
```

</details>

<details open>
<summary> perf cmds </summary>

```shell
##Setting probe
## Note that kernel vmlinux is already copied to /boot/vmlinux
# -a is for add event, Use -f is to force
# -s is for source code (using host_shared) since this is run on qemu
# -m ext4 // for [module](module.md)
# -g can be added like (-ag) in perf probe itself to record the call trace


# this record all call traces for a process a.out
sudo /home/riteshh/linux/tools/perf/perf record -ag /home/riteshh/a.out /mnt/testfile 0

1. sudo /home/riteshh/linux/tools/perf/perf report # this should give us timing report of each call

2. sudo /home/riteshh/linux/tools/perf/perf script # this should give us all call stack traces

=> note that perf probe is only when we want to record the arguments/variables value.
otherwise above two are sufficient to debug any call stack trace or perf problem



# list all functions which you want to record
sudo perf probe -s ~/host_shared/src/linux -F

# showsthe line no. for function you want to record
sudo perf probe -s ~/host_shared/src/linux -L read_swap_cache_async

# pmem_do_write
	sudo perf probe -s ~/host_shared/source/ext4/ -a 'pmem_do_write page_off:u32 sector:u64 len:u32'

# pmem_make_request
sudo perf probe -s ~/host_shared/source/ext4/ -a 'pmem_make_request bio->bi_iter.bi_size:u32 bio->bi_vcnt:u32 bio->bi_io_vec->bv_len:u32'

## ext4_dax/page_mkwrite
# below expression filter is only for write
sudo perf probe -s ~/host_shared/source/ext4/ -a 'ext4_dax_huge_fault vmf->vma->vm_file->f_path.dentry->d_iname:string vmf->flags:x32 vmf->vma->vm_flags:x64' \
								--filter 'vmf->flags & 0x1' --filter 'vmf->vma->vm_flags & 0x8'

# ext4_page_mkwrite
sudo perf probe -s ~/host_shared/source/ext4/ -a 'ext4_page_mkwrite vmf->vma->vm_file->f_path.dentry->d_iname:string vmf->page->index:s64'

# ext4_dax_fault
sudo perf probe -s ~/host_shared/source/ext4/ -a 'ext4_dax_fault vmf->vma->vm_file->f_path.dentry->d_iname:string'

# ext4_file_write_iter
sudo perf probe -s ~/host_shared/source/ext4/ -a 'ext4_file_write_iter name=iocb->ki_filp->f_path.dentry->d_iname:string count=from->count:u64'

# sched event
sudo perf record -e 'sched:sched_process_*' -a sleep 5

# f2fs_fill_super
sudo perf probe -s ~/host_shared/source/ext4 -a 'f2fs_fill_super sb->s_flags:x64'

# record iomap issue of overflow
sudo perf probe -s ~/host_shared/src/linux/ -a '__bio_try_merge_page:10 bio page page->index bio->bi_iter.bi_size len same_page[0]'

## record perf event with fio run for iomap overflow case
sudo perf record -e probe:__bio_try_merge_page_L10 -a --filter 'bi_size > 0xff000000' sudo fio --rw=write --bs=1M --numjobs=1 --name=/mnt/testfile --size=24G --ioengine=libaio

#output of fio causing overflow in __bio_try_merge_page
# fio  5667 [005]  1575.328501: probe:__bio_try_merge_page_L10: (c000000000aa054c) bio=0xc0000013e0b442c0 page=0xc00c0000030c2740 index=0x28d35 bi_size=0xfffff000 len=0x1000 same_page=0x1

# list all events (including tracepoint events)
sudo perf list

`recording`
# all cpus, no callgraph
sudo perf record -e probe:pmem* -e probe:ext4* -a dd if=/dev/zero of=/mnt/filefs count=1 bs=16M

# all cpus, with callgraph -g option
sudo perf record -e probe:pmem* -e probe:ext4* -a dd if=/dev/zero of=/mnt/filefs count=1 bs=16M

# to record all the call trace graph which a process is doin


`analyzing`
sudo perf script

# -g enables the call graph
sudo perf top -g

#sometimes this is useful too
sudo perf report --no-children
```

</details>

## Notes


## TODOS
