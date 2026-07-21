#pragma once

#include <fstream>
#include <vector>
#include <string>

#include "formats/elf.hpp"

class Parser {
public:
    explicit Parser(std::string filePath);

private:
    void parse();
    void analyzeProgramHeaders();
    void analyzeSectionHeaders();
    void analyzeEntryPoint();
    void analyzeBytes();

    std::string m_filePath;
    std::ifstream m_file;
    elf::Header64 m_header{};

    std::vector<elf::ProgramHeader64> m_programHeaders;
    std::vector<elf::ProgramHeader64> m_gnuStackPheaders;
    
    std::vector<elf::SectionHeader64> m_sectionHeaders;
};
