# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Commands

```sh
# Build the bootloader (produces pboot.bin then wraps it into pboot.efi)
make

# Run in QEMU (boots pboot.efi from FAT virtual disk via UEFI)
./run

# Install to /boot (real hardware)
make install

# Clean build artifacts
make clean
```

There is no test suite. Testing is done by running in QEMU with `./run`.

The `./run` script boots QEMU with KVM using `./virtual_machine/uefi.bios`. The virtual disk at `./virtual_machine/disk/` is a FAT image; `pboot.efi` must be copied there manually (or symlinked) as `EFI/BOOT/BOOTX64.EFI` before running.

## Architecture

**pboot** is a UEFI EFI application bootloader. It supports two modes:

1. **pkernel mode** — loads the custom `pkernel` raw binary at physical address `0x4000000`, builds a `BootInfo` struct, calls `ExitBootServices`, then jumps to the kernel entry point (passing `BootInfo*` in `rcx`, Microsoft ABI).
2. **Linux chainload mode** — loads a Linux kernel via `LoadImage`/`StartImage` (EFI stub chainloading) with parameters from `pboot.conf`.

### Build pipeline

The two-step build is unusual and important:

1. GCC compiles all `*.c` files with `-mabi=ms` (Microsoft ABI throughout), `-fPIC -fshort-wchar`, and links them into `pboot.bin` — a raw position-independent binary (no ELF header) using `binary.ld`.
2. FASM wraps `pboot.bin` inside a `format pe64 efi` container (`efi.s`) to produce `pboot.efi` — the valid PE32+ file that UEFI firmware can load. No actual assembly logic lives in `efi.s`; it just embeds the binary.

### Entry and execution flow

- UEFI calls `start()` in `start.c` (placed first by `binary.ld`) with `(Handle, SystemTable*)`.
- `start()` calls `main()` in `main.c`, which sequences: get loaded image → init input → setup filesystem → load config → show menu (if configured) → boot.
- `boot()` checks if the selected kernel name starts with `"pk"`: if so, calls `boot_pkernel()` (in `pkernel.c`); otherwise falls through to Linux EFI stub chainloading.

### Key data flow for pkernel boot

`boot_pkernel()` in `pkernel.c`:
1. Queries `EFI_GRAPHICS_OUTPUT_PROTOCOL` to get framebuffer info.
2. Scans `SystemTable->configuration_tables` for `EFI_ACPI_20_TABLE_GUID` to find the XSDT address.
3. Allocates pages at exactly `0x4000000` (`EFI_ALLOCATE_ADDRESS`) and reads the kernel file there.
4. Builds a `BootInfo` struct (defined in `pkernel.h`) containing `FrameBuffer`, `MemoryMapInfo`, and `xsdt_address`.
5. Calls `ExitBootServices` (requires using the memory map key from `get_memory_map_key()`).
6. Jumps to `0x4000000` with `BootInfo*` as the first argument.

**Critical sync point**: `BootInfo` in `pkernel.h` must stay identical to `pkernel.h` in the kernel root (`../pkernel.h`). Both sides of the boot boundary use this struct.

### Configuration file (`pboot.conf`)

Read from the EFI partition root at boot. Format — one directive per line, no `#` comments:

| Key | Value | Meaning |
|-----|-------|---------|
| `m` | `0`/`1` | Show menu |
| `e` | digit | Default entry index |
| `n` | `"string"` | Entry display name |
| `k` | `"string"` | Kernel filename |
| `p` | `"string"` | Kernel parameters (ends current entry) |

Entries are parsed sequentially; `p` terminates each entry. The kernel name `"pk..."` (starts with `pk`) triggers pkernel mode.

### Module map

| File(s) | Role |
|---------|------|
| `start.c` | UEFI entry point — calls `main()` |
| `main.c` | Boot sequencing; `chainload_linux_efi_stub()` |
| `pkernel.c/h` | pkernel-specific boot: ACPI lookup, memory alloc, `ExitBootServices`, jump |
| `efi.h` | Hand-written UEFI protocol structs (`SystemTable`, `BootTable`, `LoadedImageProtocol`, etc.) |
| `efi.s` | FASM wrapper — embeds `pboot.bin` into a PE64 EFI container |
| `binary.ld` | Linker script — raw binary output, `start.o` placed first |
| `types.h` | Type aliases; `BootLoaderEntry` struct (name + kernel + params, Unicode16) |
| `graphics.c/h` | `EFI_GRAPHICS_OUTPUT_PROTOCOL` — queries framebuffer address and resolution |
| `memory.c/h` | Memory map retrieval, `allocate_memory`, `copy_memory`; `MemoryMapInfo` struct |
| `files.c/h` | `EFI_SIMPLE_FILE_SYSTEM_PROTOCOL` — `open_file`, `read_file`, `read_file_to_memory`, `close_file` |
| `configuration.c/h` | Parses `pboot.conf` (ASCII→Unicode16 conversion, per-line directive parsing) |
| `menu.c/h` | Boot menu display and entry selection |
| `input.c/h` | Keyboard input via `EFI_SIMPLE_TEXT_INPUT_PROTOCOL` |
| `utils.c/h` | `log()`, `hang()`, `halt()`, string utilities |
| `console.h` | `TextOutputProtocol` struct |

### Toolchain

- **Compiler**: `cc` (GCC) — `-ffreestanding -fno-stack-check -fno-stack-protector -fPIC -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -mabi=ms`
- **Linker**: `ld` with `binary.ld` — raw binary output; `start.o` first so `start()` is at offset 0
- **Assembler**: FASM (`./tools/fasm`) — only used to create the PE64 EFI wrapper in `efi.s`
- All C code runs under Microsoft ABI (`-mabi=ms`) to match the UEFI calling convention; no ABI translation is needed
