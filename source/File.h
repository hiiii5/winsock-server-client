#pragma once
#include <ios>
#include <string>

class File
{
public:
    static std::string ExePath();
    
    static std::string ReadFile(const char* Filepath);
    static bool WriteToFile(const char* Filepath, const char* Text, std::ios_base::openmode = std::ios_base::out);
};
