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

    // Checking magic numbers for ELF match
    if (m_header.e_ident[elf::EI_MAG0] != 0x7F ||
        m_header.e_ident[elf::EI_MAG1] != 'E' ||
        m_header.e_ident[elf::EI_MAG2] != 'L' ||
        m_header.e_ident[elf::EI_MAG3] != 'F') {
        throw std::runtime_error("Not a valid ELF binary: " + m_filePath);
    }

    std::cout << "Success: Valid 64-bit ELF binary identified!\n";

    // std::cout << "This is the entry: 0x" << std::hex << m_header.e_entry << "\n";
    // std::cout << "0x" << std::hex <<  m_header.e_phoff << "\n";
}
