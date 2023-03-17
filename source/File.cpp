#include "File.h"

#include <fstream>
#include <iostream>
#include <windows.h>
#include <string>

std::string File::ExePath()
{
    char buff[MAX_PATH];
    GetModuleFileName(nullptr, buff, MAX_PATH );
    const std::string::size_type position = std::string( buff ).find_last_of( "\\/" );
    return std::string( buff ).substr( 0, position);
}

std::string File::ReadFile(const char* Filepath)
{
    std::ifstream file;
    file.open(Filepath, std::ios::in);

    std::string data;

    if (file.is_open())
    {
        //Log::Write(Log::Message, "Reading file %s", Filepath);
        getline(file, data, (char)26);
        file.close();
    }

    return data;
}

bool File::WriteToFile(const char* Filepath, const char* Text, std::ios_base::openmode)
{
    std::ofstream file;
    file.open(Filepath, std::ios::app);

    if (!file)
    {
        //Log::WriteLine("Failure to open or create file: %s\n", Filepath);
        return false;
    }

    if (file.is_open())
    {
        file << Text << std::endl;

        file.close();
    }

    return true;
}
