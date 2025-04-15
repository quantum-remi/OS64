# AMD64 OS Development Plan (C23, Limine, SMT)

## Core Principles
- **Robustness**: 
- **No AI Code**: 
- **Good Docs**:


# Directory Structure

```text
os/
├── boot/                     # Limine bootloader configuration and binaries
├── docs/                     # Project documentation and notes
├── src/                      # All source code
│   ├── userspace/            # Userspace programs and libraries
│   ├── libs/                 # Shared libraries used by kernel and userspace
│   └── kernel/               # Kernel codebase
│       ├── asm/              # Assembly entry points and low-level setup
│       ├── cpu/              # CPU-specific setup (GDT, IDT, IRQs, ISRs, RTC)
│       ├── drivers/          # Core drivers (serial I/O, GOP framebuffer, APIC)
│       ├── fs/               # Filesystem implementations (ext2, FAT32)
│       ├── mm/               # Memory management (paging, heap, PMM, VMM)
│       ├── libk/             # Kernel utility functions (memcpy, printf, etc.)
│       ├── multitasking/     # Process scheduling and context switching
│       ├── networking/       # Network stack (IP, TCP, UDP, HTTP)
│       ├── utils/            # Miscellaneous utilities and tools
│       │   └── shell/        # Built-in shell implementation
│       └── init.c            # Kernel entry point (kmain loop)
├── target/                   # Build outputs, ISO images, and intermediate files
└── tools/                    # Build scripts, Limine source, and helpers

