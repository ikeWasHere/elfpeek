#include <iostream>
#include <stdexcept>
#include <utility>
#include <iomanip>

#include "parser.hpp"
 
Parser::Parser(std::string filePath) : m_filePath(std::move(filePath)) {
    m_file.open(m_filePath, std::ios::binary);
    if (!m_file) {
        throw std::runtime_error("Could not open file: " + m_filePath);
    }

    parse();
}

void Parser::parse() {
    m_file.read(reinterpret_cast<char*>(&m_header), sizeof(m_header));
    if (!m_file) {
        throw std::runtime_error("Failed to read ELF header from: " + m_filePath);
    }

    // Checking file compatability 
    if (m_header.e_ident[elf::EI_MAG0] != 0x7F ||
        m_header.e_ident[elf::EI_MAG1] != 'E' ||
        m_header.e_ident[elf::EI_MAG2] != 'L' ||
        m_header.e_ident[elf::EI_MAG3] != 'F') {
        throw std::runtime_error("Not a valid ELF binary: " + m_filePath);
    } else if (m_header.e_ident[elf::EI_CLASS] != elf::ELFCLASS64) {
        throw std::runtime_error("Not 64-bit architecture: " + m_filePath);
    } else if (m_header.e_type != elf::ET_EXEC && m_header.e_type != elf::ET_DYN) {
        throw std::runtime_error("Not a supported executable or shared library: " + m_filePath);
    } else if (m_header.e_machine != elf::EM_X86_64) {
        throw std::runtime_error("Only the x86 architecture is supported for now \n");
    }

    // Analyze program headers
    m_file.seekg(m_header.e_phoff);

    for (uint16_t i = 0; i < m_header.e_phnum; i++) {
        elf::ProgramHeader64 ph;
        m_file.read(reinterpret_cast<char*>(&ph), sizeof(ph));

        if (ph.p_type == elf::PT_LOAD) {
            m_programHeaders.push_back(ph);
            continue;
        }
        
        if (ph.p_type == elf::PT_GNU_STACK) {
            m_gnuStackPheaders.push_back(ph);
        }
    }

    if (m_programHeaders.size() < 1) {
        throw std::runtime_error("File has no loadable segments" + m_filePath);
    }

    analyzeProgramHeaders();
    analyzeEntryPoint();
}

void Parser::analyzeProgramHeaders() {
    for (const auto& ph : m_programHeaders) {
        bool isWritable = (ph.p_flags & elf::PF_W);
        bool isExecutable = (ph.p_flags & elf::PF_X);

        // Write & Execute check
        if (isWritable && isExecutable) {
            std::cout << "[ALERT]: Write & Execute violation in segment: \n" << std::hex << ph.p_flags;
        }

        // Potential process hollowing | packer | staging bahavior
        if (ph.p_memsz > ph.p_filesz) {
            uint64_t sizeDifference = ph.p_memsz - ph.p_filesz;
            bool isLargePad = sizeDifference > 102400; // 100KB
            bool isDoubleSize = ph.p_memsz >= ph.p_filesz * 2;

            if (isDoubleSize || isLargePad) {
                std::cout << "[ALERT]: Suspicious segment size discrepancy. Potential Packer/Stager \n" 
                    "Disk Size: " << std::dec << ph.p_filesz << " bytes\n"
                    "Ram Size: " << ph.p_memsz << " bytes\n"
                    "Size Difference: " << sizeDifference << " bytes\n";
            }
        }
    }

    if (m_gnuStackPheaders.empty()) {
            std::cout << "[ALERT]: Missing configuration, No PT_GNU_STACK segment found, potential stack based buffer overflows.\n" ;
    } else if (m_gnuStackPheaders.size() > 1) {
        std::cout << "[ALERT]: Potential evasion tactic, multiple PT_GNU_STACK segments.\n";
    }

    for (const auto& ph : m_gnuStackPheaders) {
        if (ph.p_flags & elf::PF_X) {
            std::cout<< "[ALERT]: Potential buffer overflow, executable call stack\n";
        }
    }
}

void Parser::analyzeEntryPoint() { 
    for (const auto& ph: m_programHeaders) {
        bool entryPointFound =  (m_header.e_entry >= ph.p_vaddr) && 
            (m_header.e_entry < ph.p_vaddr + ph.p_memsz);
        
        if (!entryPointFound) continue;
        
        if(ph.p_flags & elf::PF_W) {
            std::cout << "[ALERT]: Entry point 0x: " << std::hex << m_header.e_entry << " in writable segment! Likely malware.";
            return;
        } 

        return;
    }

    std::cout << "[ALERT]: Entry point 0x: " << std::hex << m_header.e_entry <<  " does not map to any loadable segment, binary will cause segmentation fault.";
}
