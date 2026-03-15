# Boonix
Un*x for Baboons

### Dependencies
kernel:
    * gmake
    * gcc/clang
    * xorriso or mtools
rebourne, utils and termfun:
    * rust stable toolchain

### (KERNEL) Toolchain selection
The `TOOLCHAIN` and `TOOLCHAIN_PREFIX` `make` variables can be used to set the toolchain. 
For example:
```
make TOOLCHAIN=llvm
```
or:
```
make TOOLCHAIN_PREFIX=x86_64-elf-
```

### Architecture support
This project is currently x86-only(though the userspace apps should run on any architecture).

### Makefile targets
`make all`: builds the kernel and makes a test ISO.
`make all-hdd`: builds the kernel and makes a test HDD.
`make run`: same as `make all` but it runs it in qemu with OVMF.
`make run-hdd`: same as `make all-hdd` but it runs it in qemu with OVMF.
`make run-bios`: same as `make all` but it runs it with qemu with the default BIOS.
`make run-hdd-bios`: same as `make all-hdd` but it runs it in qemu with the default BIOS.
