# Boonix
Un*x for Baboons written in Rust and C.

### Components
`kernel/` - the unnamed Boonix kernel™

`apps/`
- `rebourne/` - the system shell meant to be bash-compatible
- `utils/` - various system utilities
- `termfun/` - terminal fun stuff

### Dependencies
kernel:
* gmake
* gcc/clang
* xorriso or mtools

userspace applications:
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
This project is currently x86-only (userspace apps may run on any architecture).

### Makefile targets
`make all`: builds the kernel and makes a test ISO.
`make all-hdd`: builds the kernel and makes a test HDD.
`make run`: same as `make all` but it runs it in qemu with OVMF.
`make run-hdd`: same as `make all-hdd` but it runs it in qemu with OVMF.
`make run-bios`: same as `make all` but it runs it with qemu with the default BIOS.
`make run-hdd-bios`: same as `make all-hdd` but it runs it in qemu with the default BIOS.
`make debug`: same as `make run` but it runs it with debug flags.

### Contributing
Contributions are welcome! Just open a pull request and I'll see if I can merge it.

## Credits
* [os_things](https://github.com/laurinyev/os_things): source of the x86.h header
* [nanoprintf](https://github.com/charlesnicholson/nanoprintf/tree/main): source of the nanoprintf header
* [limine-c-template](https://codeberg.org/Limine/limine-c-template): source of the kernel build system

### Licensing
This operating system is part of the public domain! You can do whatever you want with it under the nonexistent terms of the BSD0 license. See LICENSE file for details.
