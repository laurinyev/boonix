# Boonix
Un*x for Baboons written in Rust and C.

## Usage
```
git clone https://github.com/laurinyev/boonix.git
cd boonix
make run
```
(note: this currently only launches a headless QEMU instance with the barebones kernel,<\br> check out the other projects' READMEs for more info on how to run them)

## Components
`kernel/` - the unnamed Boonix kernel™

`apps/`:
- `rebourne/` - bash-compatible system shell
- `utils/` - various system utilities
- `termfun/` - terminal fun stuff

More information in their respective READMEs

## Dependencies
Kernel:
* gmake
* gcc/llvm + binutils
* xorriso or mtools

Userspace applications:
* Rust stable toolchain

## (KERNEL) Toolchain selection
The `TOOLCHAIN` and `TOOLCHAIN_PREFIX` `make` variables can be used to set the toolchain. 
For example:
```
make TOOLCHAIN=llvm
```
or:
```
make TOOLCHAIN_PREFIX=x86_64-elf-
```

## Architecture support
This project is currently x86-only (userspace apps may run on any architecture).

## Makefile targets
* `all`: builds the kernel + makes a test ISO.
* `run`: `all` + runs it in qemu with OVMF.
* `debug`: `all` + runs it with debug flags (and OVMF).

suffixes(must be applied in order if stacked; not applicable to `debug` because I'm lazy 💀):
1. `-hdd`: makes it create a test HDD instead of test ISO(requires `mtools`)
2. `-bios`: makes it use the default QEMU firmware instead of OVMF.

### Contributing
Contributions are welcome! Just open a pull request and I'll see if I can merge it.

## Credits
* [os_things](https://github.com/laurinyev/os_things): source of the x86.h header
* [nanoprintf](https://github.com/charlesnicholson/nanoprintf/tree/main): source of the nanoprintf header
* [limine-c-template](https://codeberg.org/Limine/limine-c-template): source of the kernel build system

## No AI used
Wanna see something that is vibecoded? Take a look at [this](https://github.com/laurinyev/vibe_coding_whatever)!

(Please don't click the link if you don't like vibecoded projects)

## Licensing
This operating system is part of the public domain! You can do whatever you want with it under the nonexistent terms of the BSD0 license. See LICENSE file for details.
