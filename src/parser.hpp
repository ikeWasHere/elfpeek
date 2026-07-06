#pragma once

#include <fstream>
#include <string>

#include "formats/elf.hpp"

class Parser {
public:
    explicit Parser(std::string filePath);

private:
    void parse();

    std::string m_filePath;
    std::ifstream m_file;
    elf::Header64 m_header{};
};
