# AMD64 OS Development Plan (C23, Limine, SMT)

## Core Principles
- **Robustness**: 
- **No AI Code**: 
- **Good Docs**:

## Project structure

os/
├── boot/                     # limine files
├── docs/                     # documentation
├── src/
│   ├── userspace/            # userspace code
│   ├── libs/
│   └── kernel/
│       ├── asm/             # asm entry files
│       ├── cpu/              # gdt, idt, irq, isr, rtc
│       ├── drivers/          # basic general use drivers (serial, gop, apic)
│       ├── fs/               # filesystems (ext2, fat32)
│       ├── mm/               # memory management
│       ├── libk/             # memcpy, memset, printf, strlen
│       ├── multitasking/     # multitasking stuff
│       ├── networking/       # networking stuff http, tcp, udp, ip
│       ├── utils/            # shell and random stuff
│       │   └── shell/
│       └── init.c             # kmain loop
├── target/                   # iso files
└── tools/                    # scripts, limine src

