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
    } else if (m_header.e_type != elf::ET_EXEC) {
        throw std::runtime_error("Not an executable file: " + m_filePath);
    } else if (m_header.e_machine != elf::EM_X86_64) {
        throw std::runtime_error("Only x86 architecture is accepted right now \n");
    }

    // Analyze program headers
    m_file.seekg(m_header.e_phoff);

    for (uint16_t i = 0; i < m_header.e_phnum; i++) {
        elf::ProgramHeader64 ph;
        m_file.read(reinterpret_cast<char*>(&ph), sizeof(ph));

        if (ph.p_type == elf::PT_LOAD) {
            m_programHeaders.push_back(ph);
        }
    }

    if (m_programHeaders.size() < 1) {
        throw std::runtime_error("File has no loadable segments" + m_filePath);
    }

    parseProgramHeaders();
}

void Parser::parseProgramHeaders(){
    //
}
