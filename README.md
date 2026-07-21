# elfpeek

A command line static analysis tool that parses 64-bit ELF binaries from scratch and flags structural and behavioral indicators commonly associated with malware, packers, and memory corruption exploits.

Built to explore how malware analysts and reverse engineers reason about a binary before ever running it — by inspecting its ELF headers, segment permissions, and layout for red flags.

## Why this project

Static binary analysis sits at the intersection of systems programming, security, and reverse engineering. `elfpeek` was built to get hands on with:

- The **ELF format** at the byte level — reading raw headers directly from disk with `reinterpret_cast` and manual offsets rather than relying on an existing parsing library.
- **Malware analysis heuristics** used by real tools (e.g. CFF Explorer, `checksec`, YARA-style triage) to spot suspicious binaries before execution.
- **Memory safety and exploit mitigation concepts** — W^X violations, executable stacks, and stripped metadata used to hinder analysis.

## What it detects

Given a path to an ELF binary, `elfpeek` parses the ELF, program, and section headers and reports:

| Check                                | What it catches                                                                                                          |
| ------------------------------------ | ------------------------------------------------------------------------------------------------------------------------ |
| **W^X segment violations**           | Loadable segments that are simultaneously writable and executable — a classic red flag for shellcode/self-modifying code |
| **Segment size discrepancies**       | `p_memsz` significantly larger than `p_filesz`, indicating a packer/stager that inflates memory at runtime               |
| **Missing `PT_GNU_STACK`**           | Absence of the segment that marks the stack non-executable, suggesting missing exploit mitigations                       |
| **Duplicate `PT_GNU_STACK` entries** | Malformed/anomalous program headers, a possible evasion tactic against naive parsers                                     |
| **Executable stack**                 | `PT_GNU_STACK` explicitly marked executable, enabling classic stack-based buffer overflow exploitation                   |
| **Entry point validation**           | Confirms `e_entry` maps into a loadable segment, and flags entry points that land in writable memory                     |
| **Stripped/missing section headers** | Detects binaries with no section header table, a common technique to hinder static analysis tools                        |

## Example

```
$ ./elfpeek tests/malware/x86__64__lsb__unix-system-server
[ALERT]: Write & Execute violation in segment:
[ALERT]: Suspicious segment size discrepancy. Potential Packer/Stager
Disk Size: 4096 bytes
Ram Size: 106496 bytes
Size Difference: 102400 bytes
[ALERT]: Entry point 0x1040 in writable segment! Likely malware.
```

## Design notes

- **Zero external dependencies.** ELF structures (`Header64`, `ProgramHeader64`, `SectionHeader64`) are hand-rolled in `src/formats/elf.hpp` to mirror the on-disk layout exactly, using packed structs and raw `ifstream` reads.
- **Fail-fast validation.** Magic bytes, ELF class (64-bit only), binary type, and machine architecture are all validated before any analysis runs, with clear exceptions surfaced to the user.
- Currently scoped to **64-bit x86-64 executables and shared objects** (`ET_EXEC` / `ET_DYN`); this scope is intentional to keep the parsing logic correct and well tested before expanding to 32-bit or other architectures.

## Building

Requires a C++17 >= compiler and CMake 3.21+.

```
make build      # configure + build (build/elfpeek)
make run FILE=path/to/binary
make rebuild    # clean + build
```

Or build a release binary directly:

```
cmake --preset release
cmake --build --preset release
```

## Roadmap

- 32-bit ELF and ARM support
- Import/dynamic symbol table analysis (suspicious API usage, e.g. `ptrace`, `mprotect`)
- Entropy analysis per section to flag packed/encrypted payloads
- JSON output mode for integration into larger pipelines
