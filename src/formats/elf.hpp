#pragma once

#include <cstdint>

namespace elf {

// ELF Magic number indices
constexpr int EI_MAG0   = 0;
constexpr int EI_MAG1   = 1;
constexpr int EI_MAG2   = 2;
constexpr int EI_MAG3   = 3;

constexpr int EI_CLASS = 4;
constexpr int EI_NIDENT = 16;

constexpr uint16_t ET_EXEC = 2;
constexpr uint16_t ET_DYN = 3;

constexpr uint8_t ELFCLASS32 = 1;
constexpr uint8_t ELFCLASS64 = 2;

constexpr uint16_t EM_X86_64  = 62;
constexpr uint16_t EM_AARCH64 = 183;


// The standard 64-bit ELF Header structure
struct __attribute__((packed)) Header64 {
    unsigned char e_ident[EI_NIDENT];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint64_t      e_entry;
    uint64_t      e_phoff;
    uint64_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
};

constexpr uint32_t PT_LOAD = 1;

constexpr uint32_t PT_GNU_STACK = 0x6474e551; // Stack execution permissions

constexpr uint32_t PF_X = 1; // Executable
constexpr uint32_t PF_W = 2; // Writable
constexpr uint32_t PF_R = 4; // Readable

struct __attribute__((packed)) ProgramHeader64 {
    uint32_t p_type;   
    uint32_t p_flags;  
    uint64_t p_offset; 
    uint64_t p_vaddr;  
    uint64_t p_paddr;  
    uint64_t p_filesz; 
    uint64_t p_memsz;  
    uint64_t p_align;  
};


constexpr uint32_t SHT_NOBITS = 8; // Type for uninitialized data sections (.bss)

constexpr uint64_t SHF_EXECINSTR = 0x4;  // Section contains executable code

struct __attribute__((packed)) SectionHeader64 {
    uint32_t   sh_name;
    uint32_t   sh_type;
    uint64_t   sh_flags;
    uint64_t   sh_addr;
    uint64_t   sh_offset;
    uint64_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    uint64_t   sh_addralign;
    uint64_t   sh_entsize;
};


}





